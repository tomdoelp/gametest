#include "world.h"

World::World(Renderer *r) : r(r) {
	LOG("World created");
	m = NULL;
}
World::~World(){
 	for (auto &o : objects) {
		delete o;
	}

	if (m) {
		delete m;
		m = NULL;
	}

	LOG("World destroyed");
}
Map *World::get_map() { return m; }
Renderer *World::get_renderer() { return r; }

void World::update() {
	if (!paused) {
		for (auto &o : objects) {
			o->update();
		}
	}
	else
		r->pmenu.update();
}

void World::register_object(Obj *o) {
	objects.insert(objects.begin() + o->get_id(), o);
}

void World::set_view_focus(PhysicalObj *o) {
	r->set_view_focus(o);
}

void World::load_map(const char* fname) {
	if (m) {
		delete m;
	}
	m = new Map(this, fname);
	LOG("loading Map " << fname);
}

void World::render() {
	r->render(*m);
}

/*
template <class T> void World::create(float x, float y) {
	T *obj = new T(this,x,y);
	register_object(obj);

	if (std::is_base_of<T,VisibleObj>::value)
		r->register_visible(obj);
}*/

void World::destroy(Obj *o){
	for (int i = 0, max=r->visibles.size(); i<max; i++) {
		if (*(r->visibles[i]) == *o) {
			r->visibles.erase(r->visibles.begin()+i);
		}
	}


	
	/*
 	remove pointers to id from objects and r->visibles (if any)
	then delete the object
	*/
}

