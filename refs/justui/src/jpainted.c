#include <justui/jpainted.h>
#include <justui/jwidget-api.h>
#include <stdlib.h>

J_DEFINE_WIDGET(jpainted, csize, render)

jpainted *jpainted_create(void *function, j_arg_t arg, int natural_w,
	int natural_h, void *parent)
{
	if(jpainted_type_id < 0) return NULL;

	jpainted *p = malloc(sizeof *p);
	jwidget_init(&p->widget, jpainted_type_id, parent);

	p->paint = function;
	p->arg = arg;
	p->natural_w = natural_w;
	p->natural_h = natural_h;

	return p;
}

void jpainted_poly_csize(void *p0)
{
	jpainted *p = p0;
	p->widget.w = p->natural_w;
	p->widget.h = p->natural_h;
}

void jpainted_poly_render(void *p0, int x, int y)
{
	jpainted *p = p0;
	p->paint(x, y, p->arg);
}
