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
	objects.push_back(o);
	//objects.insert(objects.begin() + o->get_id(), o);
}

void World::set_view_focus(PhysicalObj *o) {
	r->set_view_focus(o);
}

void World::load_map(const char* fname) {
	for (unsigned int i = 0; i<objects.size(); i++) {
		if (!objects[i]->is_persistent()) {
			r->destroy_visible(objects[i]);

			delete objects[i];
			std::swap(objects[i], objects.back());
			objects.pop_back();
		}
	}



	/*
	std::vector<Obj*>::iterator i = objects.begin();
	while(i != objects.end()) {
		if (!(*i)->is_persistent()) {
			r->destroy_visible(*i);
			i = objects.erase(i);
		}
		else {
			++i;
		}
	}
	*/


	
	/*for (int i = 0, max=objects.size(); i<max; i++){
		if (!objects[i]->is_persistent()) {
			r->destroy_visible(objects[i]);
			objects.erase(objects.begin()+i);
			i--;
		}
	}*/
	


	if (m) {
		delete m;
	}
	LOG("loading Map " << fname);
	m = new Map(this, fname);
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

void World::destroy(VisibleObj *o){
	r->destroy_visible(o);
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (*(objects[i]) == *o) {
			delete objects[i];
			std::swap(objects[i], objects.back());
			objects.pop_back();

			i--;
		}
	}
	/*
 	remove pointers to id from objects and r->visibles (if any)
	then delete the object
	*/
}

void World::destroy(Obj *o){
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (*(objects[i]) == *o) {
			delete objects[i];
			std::swap(objects[i], objects.back());
			objects.pop_back();
			i--;
		}
	}
	/*
 	remove pointers to id from objects and r->visibles (if any)
	then delete the object
	*/
}
