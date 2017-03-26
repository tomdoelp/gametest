#ifndef WORLD_H
#define WORLD_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <type_traits>

#include "global.h"
#include "level.h"
#include "obj.h"
#include "render.h"

/*
class Map;
class Obj;
class PhysicalObj;
class View;
class Renderer;
*/

class World {
	public:
		typedef enum mode {MODE_OVERWORLD, MODE_BATTLE, MODE_PAUSE, MODE_SCENE} Mode;
		World(Renderer *r);
		virtual ~World();

		Map *get_map();
		Renderer *get_renderer();
		void update();
		void register_object(Obj *o);
		void render();
		void set_view_focus(PhysicalObj *o);

		Player *get_player();
		void set_player(Player *p);

		bool obj_collision(Obj *a, Obj *b);
		bool obj_collision(PhysicalObj *a, PhysicalObj *b);

		void load_map(const char* fname);

		/*
		   template <class T> std::weak_ptr<T> create_visible(float x=0.0f, float y=0.0f) {
		   std::shared_ptr<T> obj(make_shared<T>(this,x,y));

		   r->register_visible(std::weak_ptr<T>(obj));

		   return std::weak_ptr(obj);
		   }
		   */
		template <class T> T* create_visible(float x=0.0f, float y=0.0f) {
			T *obj = new T(x,y);
			obj->attach_to_world(this);

			r->register_visible(obj);

			return obj;
		}

		void clear_dead_objects();
		void queue_destroy(Obj *o);
		void queue_destroy_visible(VisibleObj *o);
	protected:
		Map *m;
		Renderer *r;

		Player *player;

		//std::vector< std::shared_ptr<Obj> > objects;
		//std::vector< std::shared_ptr<PhysicalObj> > obj;
		std::vector<Obj *> objects;
		std::vector<PhysicalObj *> physicals;
		std::vector<Obj *> dead_objects;
		std::vector<VisibleObj *> dead_visibles;
		Mode current_mode;
};

#endif
