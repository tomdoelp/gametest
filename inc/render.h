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
#include "menu.h"

class Renderer {
	friend class World;
	public:
		Renderer();
		Renderer(ALLEGRO_DISPLAY *display, View &v);
		~Renderer();

		void register_visible(VisibleObj *o);
/*		void destroy_visible(VisibleObj *o); */
		void destroy_visible(Obj *o);
		void render(Map &m);

		void set_view_focus(PhysicalObj *o);
	private:
		ALLEGRO_TRANSFORM trans;
		ALLEGRO_DISPLAY *display;
		View v;
		Pause pmenu;

		std::vector<VisibleObj*> visibles;
		ALLEGRO_SHADER *scale_shader;

		void depth_sort();
		ALLEGRO_SHADER *create_scale_shader();
		ALLEGRO_BITMAP *temp;
};

#endif
