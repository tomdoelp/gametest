#include "world.h"

World::World(Renderer *r) : r(r) {
	m = NULL;
}
World::~World(){
	if (m) {
		delete m;
		m = NULL;
	}
}
Map *World::get_map() { return m; }
Renderer *World::get_renderer() { return r; }

void World::update() {
	for (auto &o : objects) {
		o->update();
	}
}

void World::register_object(Obj *o) {
	objects.push_back(o);
}

void World::set_view_focus(PhysicalObj *o) {
	r->set_view_focus(o);
}

void World::load_map(const char* fname) {
	if (m) {
		delete m;
	}
	m = new Map(fname);
}

void World::render() {
		r->render(*m);
}
