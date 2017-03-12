#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "global.h"
#include "level.h"
#include "obj.h"
#include "view.h"
#include "render.h"

/*
class Map;
class Obj;
class PhysicalObj;
class View;
class Renderer;
*/

typedef enum mode {MODE_OVERWORLD, MODE_BATTLE, MODE_PAUSE, MODE_SCENE} Mode;

class World {
	public:
		World(Renderer *r);
		virtual ~World();

		Map *get_map();
		Renderer *get_renderer();
		void update();
		void register_object(Obj *o);
		void render();
		void set_view_focus(PhysicalObj *o);

		void load_map(const char* fname);

		/* template function for creating objects */
		/* destroy function takes obj id */
	protected:
		Map *m;
		Renderer *r;
		std::vector<Obj *> objects;
		std::vector<PhysicalObj *> physicals;
		Mode current_mode;
		
};

#endif
