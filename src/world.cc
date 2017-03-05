#include "world.h"
#include "level.h"
#include "obj.h"

World::World(Map *m) : m(m) {}
World::~World(){}
Map *World::get_map() const { return m; }

void World::update() {
	for (auto &o : objects) {
		o->update();
	}
}

void World::register_object(Obj *o) {
	objects.push_back(o);
}
