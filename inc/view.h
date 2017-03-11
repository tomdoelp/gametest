#ifndef _VIEW_H_
#define _VIEW_H_

#include <algorithm>
#include <stdlib.h>
#include <allegro5/allegro.h>

#include "global.h"
#include "utils.h"

class PhysicalObj;
class View {
	public:
		View();
		View(float w, float h, ALLEGRO_DISPLAY *d);
		~View();

		ALLEGRO_BITMAP *get_buffer();
		float get_x();
		float get_y();
		float get_w() const;
		float get_h() const;
		float get_scale(float dispw, float disph);
		Box get_view_box();


		void set_focus(PhysicalObj *o);
	private:
		float w, h;
		ALLEGRO_BITMAP *buffer;
		ALLEGRO_DISPLAY *display;
		float x, y;
		float scale;
		float scale_w, scale_h, scale_x, scale_y;
		PhysicalObj *focus;

		float dispw, disph;

};

#endif
