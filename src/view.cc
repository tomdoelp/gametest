#include "view.h"

View::View(float w, float h, ALLEGRO_DISPLAY *d) : w(w), h(h), display(d) {
	buffer = al_create_bitmap(w,h);

	scale = std::min(WINDOW_W / w, WINDOW_H / h);

	scale_w = SCREEN_W * scale;
	scale_h = SCREEN_H * scale;
	scale_x = (al_get_display_width(display) - scale_w) / 2;
	scale_y = (WINDOW_H - scale_h) / 2;	
}

float View::get_w() { return SCREEN_W; }
float View::get_h() { return SCREEN_H; }

ALLEGRO_BITMAP *View::get_buffer() { return buffer; }
float View::get_scale(float dispw, float disph) {
	if (screen_scale == 0) {
		scale_w = (float) dispw / w;
		scale_h = (float) disph / h;
		return scale_w < scale_h ? scale_w : scale_h;
	} else {
		return screen_scale;
	}
}
