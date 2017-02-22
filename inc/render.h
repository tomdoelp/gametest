#ifndef _RENDER_H_
#define _RENDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "global.h"
#include "obj.h"
#include "sprite.h"
#include "load.h"
#include "level.h"
#include "view.h"

class Renderer {
	public:
		Renderer(ALLEGRO_DISPLAY *display, View &v);

		void register_visible(VisibleObj *o);
		void render(Map &m);
	private:
		ALLEGRO_DISPLAY *display;
		View v;

		std::vector<VisibleObj*> visibles;
		ALLEGRO_SHADER *scale_shader;

		void depth_sort();
		ALLEGRO_SHADER *create_scale_shader();
};

#endif
