#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "global.h"

class Map;
class Obj;
class PhysicalObj;

class World {
	public:
		World(Map *m);
		virtual ~World();

		Map *get_map() const;
		void update();
		void register_object(Obj *o);
	protected:
		Map *m;
		std::vector<Obj *> objects;
		std::vector<PhysicalObj *> physicals;
};

#endif
