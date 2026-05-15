#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jscene.h>

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/drivers/keydev.h>
#include <gint/clock.h>
#include <gint/std/stdlib.h>
#include <gint/gint.h>
#include <gint/drivers/t6k11.h>
#include <gint/cpu.h>
#include <gint/hardware.h>

/* Type identifier for jscene */
static int jscene_type_id = -1;
/* Events */
uint16_t JSCENE_NONE;
uint16_t JSCENE_PAINT;

/* Keyboard transformation for inputs in a jscene */
static int jscene_repeater(int key, GUNUSED int duration, int count)
{
	if(key != KEY_LEFT && key != KEY_RIGHT && key != KEY_UP && key != KEY_DOWN)
		return -1;

	return (count ? 40 : 400) * 1000;
}
static keydev_transform_t jscene_tr = {
	.enabled =
		KEYDEV_TR_DELAYED_SHIFT |
		KEYDEV_TR_INSTANT_SHIFT |
		KEYDEV_TR_DELAYED_ALPHA |
		KEYDEV_TR_INSTANT_ALPHA |
		KEYDEV_TR_REPEATS,
	.repeater = jscene_repeater,
};

jscene *jscene_create(int x, int y, int w, int h, void *parent)
{
	if(jscene_type_id < 0) return NULL;

	jscene *s = malloc(sizeof *s);
	if(!s) return NULL;

	jwidget_init(&s->widget, jscene_type_id, parent);
	jwidget_set_focus_policy(s, J_FOCUS_POLICY_SCOPE);
	jwidget_set_fixed_size(s, w, h);
	jlayout_set_vbox(s);

	/* The scene is where active focus originates */
	s->widget.focused = 1;
	s->widget.active_focused = 1;

	s->x = x;
	s->y = y;

	s->queue_first = 0;
	s->queue_next = 0;
	s->lost_events = 0;
	s->mainmenu = (gint[HWCALC] != HWCALC_FXCG100);
	s->poweroff = true;
	s->autopaint = false;

	s->touch_last_x = -1;
	s->touch_last_y = -1;

	/* Prepare first layout/paint operation */
	s->widget.dirty = 1;

	return s;
}

jscene *jscene_create_fullscreen(void *parent)
{
	return jscene_create(0, 0, DWIDTH, DHEIGHT, parent);
}

jscene *jscene_owning(void *w0)
{
	J_CAST(w)

	while(w) {
		if(w->type == jscene_type_id) return (void *)w;
		w = w->parent;
	}
	return NULL;
}

void jscene_render(jscene *s)
{
	jwidget_layout(s);
	jwidget_render(s, s->x, s->y);

#if J_CONFIG_TOUCH_INSPECTOR
	if(s->touch_last_x >= 0 && s->touch_last_y >= 0) {
		jwidget *w =
			jscene_widget_at(s, s->touch_last_x, s->touch_last_y, true);
		if(w) {
			int x1 = jwidget_absolute_padding_x(w);
			int y1 = jwidget_absolute_padding_y(w);
			int x2 = x1 + jwidget_padding_width(w) - 1;
			int y2 = y1 + jwidget_padding_height(w) - 1;
			drect_border(x1, y1, x2, y2, C_NONE, 1, C_RED);

			dprint_opt(DWIDTH-1, 0, C_WHITE, C_RED, DTEXT_RIGHT, DTEXT_TOP,
				"%s", jwidget_type(w));
		}
	}
#endif
}

/* Find sub-widget based on w-local coordinates. */
static jwidget *jscene_widget_at_rec(
	jwidget *w, int x, int y, bool exclude_mp, bool visible_only)
{
	if(!w)
		return NULL;
	jwidget *found;

	jwidget_geometry const *g = jwidget_geometry_r(w);
	/* Content box (this is what children's xy are measured against) */
    int xC = g->margin.left + g->border.left + g->padding.left;
    int yC = g->margin.top + g->border.top + g->padding.top;
    uint wC = jwidget_content_width(w);
    uint hC = jwidget_content_height(w);
    /* Padding box */
    int xP = xC - g->padding.left;
    int yP = yC - g->padding.top;
    uint wP = wC + g->padding.left + g->padding.right;
    uint hP = hC + g->padding.top + g->padding.bottom;
    /* Full box (implicitly starts at 0,0) */
	uint wF = jwidget_full_width(w);
	uint hF = jwidget_full_height(w);

	/* Check if we intersect w at all */
	bool intersects;
	if(exclude_mp)
		intersects = ((uint)(x - xP) < wP) && ((uint)(y - yP) < hP);
	else
		intersects = (uint)x < wF && (uint)y < hF;
	if(!intersects)
		return NULL;

	/* If we have a stacked widget, only consider the visible child */
	jlayout_stack *stack;
	if((stack = jlayout_get_stack(w))) {
		if(stack->active < 0)
			return w;
		jwidget *child = w->children[stack->active];
		found = jscene_widget_at_rec(child, x - xC - child->x,
			y - yC - child->y, exclude_mp, visible_only);
		return found ? found : w;
	}

	/* Try to find a descendant of a floating child that intersects... */
	for(int k = 0; k < w->child_count; k++) {
		jwidget *child = w->children[k];
		if(child->visible >= visible_only && child->floating) {
			found = jscene_widget_at_rec(child, x - xC - child->x,
				y - yC - child->y, exclude_mp, visible_only);
			if(found)
				return found;
		}
	}

	/* ... or a descendant of a non-floating child */
	for(int k = 0; k < w->child_count; k++) {
		jwidget *child = w->children[k];
		if(child->visible >= visible_only && !child->floating) {
			found = jscene_widget_at_rec(child, x - xC - child->x,
				y - yC - child->y, exclude_mp, visible_only);
			if(found)
				return found;
		}
	}

	/* If no descendants do, then the best match is w itself. */
	return w;
}

jwidget *jscene_widget_at(jscene *scene, int x, int y, bool exclude_mp)
{
	return jscene_widget_at_rec(&scene->widget, x, y, exclude_mp, true);
}

//---
// Event management
//---

jevent jscene_read_event(jscene *s)
{
	cpu_atomic_start();

	if(s->queue_first == s->queue_next) {
		cpu_atomic_end();
		return (jevent){ .source = NULL, .type = JSCENE_NONE };
	}

	jevent e = s->queue[s->queue_first];
	s->queue_first = (s->queue_first + 1) % JSCENE_QUEUE_SIZE;
	cpu_atomic_end();
	return e;
}

void jscene_queue_event(jscene *s, jevent e)
{
	cpu_atomic_start();

	/* Prevent filling and overflowing the queue */
	int next = (s->queue_next + 1) % JSCENE_QUEUE_SIZE;
	if(next == s->queue_first) {
		s->lost_events++;
	}
	else {
		s->queue[s->queue_next] = e;
		s->queue_next = next;
	}

	cpu_atomic_end();
}

//---
// Keyboard focus and keyboard events
//---

void *jscene_focused_widget(jscene *s)
{
	jwidget *w = &s->widget;
	while(w->focus_scope_target)
		w = w->focus_scope_target;
	return w;
}

void jscene_set_focused_widget(jscene *s, void *w0)
{
	J_CAST(w)

	/* Unfocus only at the top level */
	if(!w) {
		jwidget_scope_set_target(s, NULL);
		return;
	}

	if(w->focus_policy == J_FOCUS_POLICY_REJECT)
		return;

	/* Check that (s) is an ancestor of (w) */
	for(jwidget *anc = w; anc != (jwidget *)s; anc = anc->parent) {
		if(anc == NULL) return;
	}

	/* Set targets in every scope along the way up */
	jwidget *scope = w;
	while(scope != NULL) {
		scope = jwidgetctx_enclosing_focus_scope(scope);
		jwidget_scope_set_target(scope, w);
		w = scope;
	}
}

void jscene_show_and_focus(jscene *scene, void *w0)
{
	J_CAST(w)

	/* Ensure the widget is visible */
	jwidget_set_visible(w, true);

	/* Force stacked layouts all the wat up to [scene] to show [w] */
	jwidget *current = w;
	jwidget *parent = w->parent;

	while(parent != (jwidget *)scene) {
		jlayout_stack *stack = jlayout_get_stack(parent);
		if(stack) {
			int pos = jwidget_child_position(parent, current);
			if(stack->active != pos) {
				stack->active = pos;
				parent->update = true;
			}
		}

		current = parent;
		parent = current->parent;
	}

	/* Give the widget focus */
	jscene_set_focused_widget(scene, w);
}

bool jscene_process_key_event(jscene *scene, key_event_t event)
{
	/* If the event is a touch down, start by moving the focus. */
#if J_CONFIG_TOUCH
	if(event.type == KEYEV_TOUCH_DOWN) {
		jwidget *w = jscene_widget_at(scene, event.x - scene->x,
			event.y - scene->y, true);
		if(w && jwidget_accepts_focus(w))
			jscene_set_focused_widget(scene, w);
	}
#endif

	jwidget *candidate = jscene_focused_widget(scene);
	jevent e = { .type = JWIDGET_KEY, .key = event };

	while(candidate) {
		if(jwidget_event(candidate, e))
			return true;
		candidate = jwidgetctx_enclosing_focus_scope(candidate);
	}

	return false;
}

bool jscene_process_event(GUNUSED jscene *scene, jevent event)
{
	if(!event.source) return false;
	jwidget *candidate = ((jwidget *)event.source)->parent;

	while(candidate) {
		if(jwidget_event(candidate, event)) return true;
		candidate = candidate->parent;
	}

	return false;
}

void jscene_set_mainmenu(jscene *scene, bool mainmenu)
{
	scene->mainmenu = mainmenu;
}

void jscene_set_poweroff(jscene *scene, bool poweroff)
{
	scene->poweroff = poweroff;
}

void jscene_set_autopaint(jscene *scene, bool autopaint)
{
	scene->autopaint = autopaint;
}

/* Poll events until an unhandled event is found, and return it. If there are
   no more, return an event of type JSCENE_NONE. */
static jevent poll_next_unhandled_event(keydev_t *d, jscene *s)
{
	jevent e;
	key_event_t k;

	/* Start by dequeuing GUI events. We want to properly propagate all the
	   consequences of a single input event before reading others. */
	while((e = jscene_read_event(s)).type != JSCENE_NONE) {
		/* Handle PAINT events when autopaint is on. */
		if(e.type == JSCENE_PAINT && s->autopaint) {
			jscene_render(s);
			dupdate();
			continue;
		}
		/* Send the rest of the events through the scene. If they're not
		   handled internally, then to the user. */
		if(e.type != JSCENE_NONE && !jscene_process_event(s, e))
			return e;
	}

	/* Then try to dequeue keyboard events, if there are any. */
	while((k = keydev_read(d, false, NULL)).type != KEYEV_NONE) {
		/* Keep track of where the touch cursor was last seen. */
#if J_CONFIG_TOUCH_INSPECTOR
		if(k.type == KEYEV_TOUCH_DOWN || k.type == KEYEV_TOUCH_DRAG) {
			s->touch_last_x = k.x - s->x;
			s->touch_last_y = k.y - s->y;
			s->widget.update = true;
		}
		if(k.type == KEYEV_TOUCH_UP) {
			s->touch_last_x = -1;
			s->touch_last_y = -1;
			s->widget.update = true;
		}
#endif
		/* Auto return-to-menu */
		if(k.type == KEYEV_DOWN && k.key == KEY_MENU && !k.shift && !k.alpha) {
			if(s->mainmenu) {
				gint_osmenu();
				jscene_queue_event(s, (jevent){ .type = JSCENE_PAINT });
				continue;
			}
		}
		/* Handle poweroff */
		if(k.type == KEYEV_DOWN && k.key == KEY_ACON && k.shift && !k.alpha) {
			if(s->poweroff) {
				gint_poweroff(true);
				jscene_queue_event(s, (jevent){ .type = JSCENE_PAINT });
				continue;
			}
		}
		/* Handle backlight */
#if GINT_HW_FX
		if(k.type == KEYEV_DOWN && k.key == KEY_OPTN && k.shift && !k.alpha) {
			t6k11_backlight(-1);
			continue;
		}
#endif
		/* Run through the getkey feature function */
		getkey_feature_t feat = getkey_feature_function();
		if((k.type == KEYEV_DOWN || k.type == KEYEV_HOLD) && feat && feat(k))
			continue;
		/* Finally, send through the scene; if it comes back, send to user. */
		if(k.type != KEYEV_NONE && !jscene_process_key_event(s, k)) {
			e.type = JWIDGET_KEY;
			e.key = k;
			return e;
		}
	}

	/* Not a single event could be found. */
	e.type = JSCENE_NONE;
	return e;
}

jevent jscene_run(jscene *s)
{
	keydev_t *d = keydev_std();
	keydev_transform_t tr0 = keydev_transform(d);
	keydev_set_transform(d, jscene_tr);

	jevent e;
	while(1) {
		e = poll_next_unhandled_event(d, s);
		if(e.type != JSCENE_NONE)
			break;

		/* When out of events to handle, consider whether to relayout and/or
		   repaint the scene. */
		if(jwidget_layout_dirty(s) || jwidget_needs_update(s)) {
			jscene_queue_event(s, (jevent){ .type = JSCENE_PAINT });
			continue;
		}

		/* Now we're fully out, sleep and wait for some more. */
		sleep();
	}

	keydev_set_transform(d, tr0);
	return e;
}

/* jscene type definition */
static jwidget_poly type_jscene = {
	.name    = "jscene",
	.csize   = NULL,
	.layout  = NULL,
	.render  = NULL,
	.event   = NULL,
	.destroy = NULL,
};

__attribute__((constructor))
static void j_register_jscene(void)
{
	jscene_type_id = j_register_widget(&type_jscene);
	JSCENE_NONE = j_register_event();
	JSCENE_PAINT = j_register_event();
}
