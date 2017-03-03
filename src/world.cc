#include "world.h"
#include "obj.h"

World::World(Map *m) : m(m) {}

void World::register_object(Obj *o) {
	objects[o->get_id()] = o;
}

void World::update_logic() {
   for (auto &o : objects) {
	   o->update();
   }
}

void World::checkin(PhysicalObj *o, const Box &bbox) {
	std::vector<int> cels = m->get_containing_cells(bbox);
	for (auto &cel : cels) {
		grid[cel].push_back(o);
	}
}

void World::broad_collisions() {
	for (auto &celllist : grid) {
		/* celllist is all the objects in a cell */
		for (int i = 0, listsize = celllist.size(); i < listsize-1; i++) { 
			/* celllist[i] is an object in this cell */
			for (int j = i+1; j < listsize; j++) {
				/* somehow check celllist[i] and celllist[j] for a collision then alert them both */
				/* but not if they've already been checked (if both are in multiple cells) */
				if (celllist[i]->get_bbox().check_collision(celllist[j]->get_bbox())) {
					celllist[i]->collide(celllist[j]);
					celllist[j]->collide(celllist[i]);
					/* TODO mark this pair as checked ! ! ! ! ! */
				}
			}

			if (celllist[i]->is_moving()) {
				/* check against tiles for collisions 
				 * moving flag saves time so we can skip objs that arent moving 
				 * since they couldn't possibly collide with a tile
				 */
			}
		}
	}
}





/*
 * Okay so everyone registers as an object when created (somewhere . . . )
 * And every object gets updated in World::update_logic().
 * If it wants to move, it will register that in its update(). 
 * Then, World uses a spacial grid for broad phase collisions
 * Then... World (?) does narrow phase and alerts all the colliders.
 */
