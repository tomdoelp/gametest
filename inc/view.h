#ifndef _VIEW_H_
#define _VIEW_H_

#include <algorithm>
#include <stdlib.h>
#include <allegro5/allegro.h>

#include "global.h"
#include "utils.h"

class View {
	public:
		View(float w, float h, ALLEGRO_DISPLAY *d);
		ALLEGRO_BITMAP *get_buffer();
		float get_x();
		float get_y();
		float get_w();
		float get_h();
		float get_scale(float dispw, float disph);

	private:
		float w, h;
		ALLEGRO_BITMAP *buffer;
		ALLEGRO_DISPLAY *display;
		float x, y;
		float scale;
		float scale_w, scale_h, scale_x, scale_y;
};

#endif
