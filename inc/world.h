#ifndef _WORLD_H_
#define _WORLD_H_

#include <allegro5/allegro.h>

#include "global.h"
#include "level.h"
#include "utils.h"

class Obj;
class PhysicalObj;
class World {
	public:
		World(Map *m);

		void register_object(Obj *o);
		void update_logic();
		void checkin(PhysicalObj *o, const Box &bbox);
		void broad_collisions();
		void narrow_collisions();
	private:
		Map *m;
		std::vector<Obj *> objects;
		std::vector<std::vector<PhysicalObj *>> grid;

		std::vector<bool> colpairs;

};

#endif
