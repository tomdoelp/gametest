#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <algorithm>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "global.h"
/*#include "obj.h" */
#include "sprite.h"
#include "load.h"
#include "level.h"


class Obj;
class PhysicalObj;
class VisibleObj;
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

		Vec2f window2world(float x, float y);
	private:
		ALLEGRO_TRANSFORM trans;
		ALLEGRO_DISPLAY *display;
		View v;
		World *world;

		std::vector<VisibleObj*> visibles;
		ALLEGRO_SHADER *scale_shader;
		ALLEGRO_SHADER *gray_shader;

		void depth_sort();
		ALLEGRO_SHADER *create_scale_shader();
		ALLEGRO_SHADER *create_gray_shader();
		ALLEGRO_BITMAP *temp_buffer;
		ALLEGRO_BITMAP *battle_buffer;

		void render_scene(Map &m, bool draw_objects=true);
};

#endif
