#include "view.h"
#include "obj.h"

View::View() {}
View::~View() {}

View::View(float w, float h, ALLEGRO_DISPLAY *d) : w(w), h(h), display(d) {
	buffer = al_create_bitmap(w,h);

	scale = MIN(WINDOW_W / w, WINDOW_H / h);

	scale_w = SCREEN_W * scale;
	scale_h = SCREEN_H * scale;
	scale_x = (al_get_display_width(display) - scale_w) / 2;
	scale_y = (WINDOW_H - scale_h) / 2;	

	focus = NULL;

	dispw = al_get_display_width(display);
	disph = al_get_display_height(display);

}

void View::set_focus(PhysicalObj *o) {
	focus = o;
}

float View::get_x() {
	if (!focus) {
		float sc = get_scale(dispw, disph);
		return dispw/2-(w * sc)/2;
	}
	else {
		return focus->get_x() - w/2;
	}
}
float View::get_y() {
	if (!focus) {
		float sc = get_scale(dispw, disph);
		return disph/2-(h * sc)/2;
	}
	else {
		return focus->get_y() - h/2;
	}
}



float View::get_w() const { return w; }
float View::get_h() const { return h; }

ALLEGRO_BITMAP *View::get_buffer() { return buffer; }

float View::get_scale(float dispw, float disph) {
	if (screen_scale < 1) {
		scale_w = (float) dispw / w;
		scale_h = (float) disph / h;
		return scale_w < scale_h ? scale_w : scale_h;
	} else {
		return screen_scale;
	}
}

Box View::get_view_box() { return Box(get_x(),get_y(),w,h); }
