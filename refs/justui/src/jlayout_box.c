#include <justui/jlayout.h>
#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include "jlayout_p.h"
#include "util.h"

#include <openlibm.h>

//---
// Flexbox-like box layout
//---

jlayout_box *jlayout_get_hbox(void *w0)
{
	J_CAST(w)
	return (w->layout == J_LAYOUT_HBOX) ? &w->layout_box : NULL;
}

jlayout_box *jlayout_get_vbox(void *w0)
{
	J_CAST(w)
	return (w->layout == J_LAYOUT_VBOX) ? &w->layout_box : NULL;
}

jlayout_box *jlayout_set_hbox(void *w0)
{
	J_CAST(w)
	w->layout = J_LAYOUT_HBOX;
	jlayout_box *l = &w->layout_box;

	l->spacing = 0;
	return l;
}

jlayout_box *jlayout_set_vbox(void *w0)
{
	J_CAST(w)
	w->layout = J_LAYOUT_VBOX;
	jlayout_box *l = &w->layout_box;

	l->spacing = 0;
	return l;
}

void jlayout_box_csize(void *w0)
{
	/* Compute as if the layout is vertical first */
	J_CAST(w)
	jlayout_box *l = &w->layout_box;
	int horiz = (w->layout == J_LAYOUT_HBOX);

	int main = 0;
	int cross = 0;

	for(int k = 0; k < w->child_count; k++) {
		jwidget *child = w->children[k];
		if(!child->visible) continue;
		jwidget_msize(child);

		main += (k > 0 ? l->spacing : 0) + (horiz ? child->w : child->h);
		cross = max(cross, (horiz ? child->h : child->w));
	}

	if(horiz) {
		w->w = main;
		w->h = cross;
	}
	else {
		w->w = cross;
		w->h = main;
	}
}

//---
// Distribution system
//
// The basic idea of space redistribution is to give each widget extra space
// proportional to their stretch rates in the relevant direction. However, the
// addition of maximum size constraints means that widgets can decline some of
// the extra space being allocated.
//
// This system defines the result of expansion as a function of the "expansion
// factor". As the expansion factor increases, every widget stretches at a
// speed proportional to its stretch rate, until it reaches its maximum size.
//
//  Extra widget size
//   |
//   +      .-------- Maximum size
//   |    .`
//   |  .` <- Slope: widget stretch rate
//   |.`
// 0 +-------+------> Expansion factor
//   0       ^
//      Breaking point
//
// The extra space allocated to widgets is the sum of this function for every
// widget considered for expansion. Since every widget has a possibly different
// breaking point, a maximal interval of expansion factor that has no breaking
// point is called a "run". During each run, the slope for the total space
// remains constant, and a unit of expansion factor corresponds to one pixel
// being allocated in the container. Thus, whenever the expansion factor
// increases of (slope), every widget (w) gets (w->stretch) new pixels.
//
// The functions below simulate the expansion by determining the breaking
// points of the widgets and allocating extra space during each run. Once the
// total extra space allocated reaches the available space, simulation stops
// and the allocation is recorded by assigning actual size to widgets.
//---

/* This "expansion" structure tracks information relating to a single child
   widget during the space distribution process. */
typedef struct {
	/* Child index */
	uint8_t id;
	/* Stretch rate, sum of stretch rates is the "slope" */
	uint8_t stretch;
	/* Maximum size augmentation */
	int16_t max;
	/* Extra space allocate in the previous runs, in pixels */
	float allocated;
	/* Breaking point for the current run, as a number of pixels to distribute
	   to the whole system */
	float breaking_point;
} exp_t;

/* Determine whether a widget can expand any further. */
static bool can_expand(exp_t *e)
{
	return (e->stretch > 0 && e->allocated < e->max);
}

/* Compute the slope for the current run. */
static uint compute_slope(exp_t elements[], size_t n)
{
	uint slope = 0;
	for(size_t i = 0; i < n; i++) {
		if(can_expand(&elements[i])) slope += elements[i].stretch;
	}
	return slope;
}

/* Compute the breaking point for every expanding widget. Returns the amount of
   pixels to allocate in order to reach the next breaking point. */
static float compute_breaking_points(exp_t elements[], size_t n, uint slope)
{
	float closest = HUGE_VALF;

	for(size_t i = 0; i < n; i++) {
		exp_t *e = &elements[i];
		if(!can_expand(e)) continue;

		/* Up to (e->max - e->allocated) pixels can be added to this widget.
		   With the factor of (slope / e->stretch), we get the number of pixels
		   to add to the container in order to reach the threshold. */
		e->breaking_point = (e->max - e->allocated) * (slope / e->stretch);
		closest = min(e->breaking_point, closest);
	}

	return closest;
}

/* Allocate floating-point space to widgets. This is the core of the
   distribution system, it produces (e->allocated) for every element. */
static void allocate_space(exp_t elements[], size_t n, float available)
{
	/* One iteration per run */
	while(available > 0) {
		/* Slope for this run; if zero, no more widget can grow */
		uint slope = compute_slope(elements, n);
		if(!slope) break;

		/* Closest breaking point, amount of space to distribute this run */
		float breaking = compute_breaking_points(elements, n, slope);
		float run_budget = min(breaking, available);

		/* Give everyone their share of run_budget */
		for(size_t i = 0; i < n; i++) {
			exp_t *e = &elements[i];
			if(!can_expand(e)) continue;

			e->allocated += (run_budget * e->stretch) / slope;

			/* Avoid floating-point errors when reaching the maximum size: test
			   (e->breaking) instead of (e->allocated), and assign (e->max) to
			   eliminate risks of rounding down */
			if(e->breaking_point == run_budget) e->allocated = e->max;
		}

		available -= run_budget;
	}
}

/* Stable insertion sort: order children by decreasing fractional allocation */
static void sort_by_fractional_allocation(exp_t elements[], size_t n)
{
	for(size_t spot = 0; spot < n - 1; spot++) {
		/* Find the element with the max fractional value in [spot..size] */
		float max_frac = 0;
		int max_frac_who = -1;

		for(size_t i = spot; i < n; i++) {
			exp_t *e = &elements[i];

			float frac = e->allocated - floorf(e->allocated);

			if(max_frac_who < 0 || frac > max_frac) {
				max_frac = frac;
				max_frac_who = i;
			}
		}

		/* Give that element the spot */
		exp_t temp = elements[spot];
		elements[spot] = elements[max_frac_who];
		elements[max_frac_who] = temp;
	}
}

/* Round allocations so that they add up to the available space */
static void round_allocations(exp_t elements[], size_t n, int available_space)
{
	/* Prepare to give everyone the floor of their allocation */
	for(size_t i = 0; i < n; i++) {
		exp_t *e = &elements[i];
		available_space -= floorf(e->allocated);
	}

	/* Sort by decreasing fractional allocation then add one extra pixel to
	   the (available_space) children with highest fractional allocation */
	sort_by_fractional_allocation(elements, n);

	for(size_t i = 0; i < n; i++) {
		exp_t *e = &elements[i];
		e->allocated = floorf(e->allocated);

		if(can_expand(e) && (int)i < available_space) e->allocated += 1;
	}
}

void jlayout_box_apply(void *w0)
{
	J_CAST(w)
	jlayout_box const *l = &w->layout_box;
	int horiz = (w->layout == J_LAYOUT_HBOX);

	if(!w->child_count) return;

	/* Content width and height */
	int cw = jwidget_content_width(w);
	int ch = jwidget_content_height(w);
	/* Allocatable width and height (which excludes spacing) */
	int total_spacing = (w->child_count - 1) * l->spacing;
	int aw = cw - (horiz ? total_spacing : 0);
	int ah = ch - (horiz ? 0 : total_spacing);
	/* Length along the main axis, including spacing */
	int length = 0;

	/* Expanding widgets' information for extra space distribution */
	size_t n = w->child_count;
	exp_t elements[n];

	bool has_started = false;
	for(size_t i = 0; i < w->child_count; i++) {
		jwidget *child = w->children[i];

		/* Maximum size to enforce: this is the acceptable size closest to our
		   allocatable size */
		int max_w = clamp(aw, child->min_w, child->max_w);
		int max_h = clamp(ah, child->min_h, child->max_h);

		/* Start by setting every child to an acceptable size */
		child->w = clamp(child->w, child->min_w, max_w);
		child->h = clamp(child->h, child->min_h, max_h);

		/* Initialize expanding widgets' information */
		elements[i].id = i;
		elements[i].allocated = 0.0f;
		elements[i].breaking_point = -1.0f;

		/* Determine natural length along the container, and stretch child
		   along the perpendicular direction if possible */

		if(!child->visible || child->floating) {
			elements[i].stretch = 0;
			elements[i].max = 0;
			continue;
		}

		if(has_started) length += l->spacing;
		if(horiz) {
			length += child->w;
			if(child->stretch_y > 0) child->h = max_h;

			elements[i].stretch = child->stretch_x;
			elements[i].max = max(max_w - child->w, 0);

			if(child->stretch_force && child->stretch_x > 0)
				elements[i].max = max(aw - child->w, 0);
		}
		else {
			length += child->h;
			if(child->stretch_x > 0) child->w = max_w;

			elements[i].stretch = child->stretch_y;
			elements[i].max = max(max_h - child->h, 0);

			if(child->stretch_force && child->stretch_y > 0)
				elements[i].max = max(ah - child->h, 0);
		}

		has_started = true;
	}

	/* Distribute extra space along the line */
	int extra_space = (horiz ? cw : ch) - length;
	allocate_space(elements, n, extra_space);
	round_allocations(elements, n, extra_space);

	/* Update widgets for extra space */
	for(size_t i = 0; i < n; i++) {
		exp_t *e = &elements[i];
		jwidget *child = w->children[e->id];
		if(!child->visible || child->floating) continue;

		if(horiz)
			child->w += e->allocated;
		else
			child->h += e->allocated;
	}

	/* Position everyone */
	int position = 0;

	for(size_t i = 0; i < n; i++) {
		jwidget *child = w->children[i];
		if(!child->visible || child->floating) continue;

		if(horiz) {
			child->x = position;
			child->y = (ch - child->h) / 2;
			position += child->w + l->spacing;
		}
		else {
			child->x = (cw - child->w) / 2;
			child->y = position;
			position += child->h + l->spacing;
		}
	}
}
