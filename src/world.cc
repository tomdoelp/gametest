#include "world.h"

World::World(Renderer *r) : r(r) {
	LOG("World created");
	m = NULL;
	player = NULL;
	sndmgr = new SoundManager(AUDIO_DEPTH_GOOD);
	sndmgr->init();
	mode = MODE_OVERWORLD;
	r->world = this;
	textbox.world = this;
	pmenu.world = this;
}
World::~World(){
 	for (auto &o : objects) {
		delete o;
	}

	if (m) {
		delete m;
		m = NULL;
	}

	if (sndmgr){
		delete sndmgr;
		sndmgr = NULL;
	}

	LOG("World destroyed");
}
Map *World::get_map() { return m; }
Renderer *World::get_renderer() { return r; }

bool World::obj_collision(Obj *a, Obj *b) {
	return a->get_bbox().check_collision(b->get_bbox());
}
bool World::obj_collision(PhysicalObj *a, PhysicalObj *b) {
	return a->get_bbox().check_collision(b->get_bbox());
}

void World::update() {
	if (key_press[ALLEGRO_KEY_ESCAPE]) {
		if (mode == World::MODE_OVERWORLD) {
			mode = World::MODE_PAUSE;
			sndmgr->play_sound(SoundManager::SND_PAUSE);
		}
		else if (mode == World::MODE_PAUSE) {
			mode = World::MODE_OVERWORLD;
		}
	}

	switch(mode) {
		case World::MODE_OVERWORLD:
			for (auto &o : objects) {
				o->update();
			}
			clear_dead_objects();
			break;
		case World::MODE_PAUSE:
			pmenu.update();
			break;
		case World::MODE_TEXT:
			textbox.update();
			break;
		default:
			break;
	}
		
/*		for (auto &o : objects) { 
			if (o->is_active()){
				for (auto &other : objects) {
					if (other->is_active() && *o != *other && obj_collision(o,other)) {
						other->collide(o);
					}
				}
			}
		}
		*/

		/* double dispatch ? ? ? */

}

void World::register_object(Obj *o) {
	objects.push_back(o);
}

void World::set_view_focus(PhysicalObj *o) {
	r->set_view_focus(o);
}
Player *World::get_player(){
	return player;
}

void World::set_mode(WorldMode mode) { this->mode = mode; }
World::WorldMode World::get_mode() { return mode; }

void World::set_player(Player *p) {
	player = p;
}

void World::load_map(const char* fname) {
	for (auto &o : objects) {
		o->map_end();
	}
	for (unsigned int i = 0; i<objects.size(); i++) {
		if (!objects[i]->is_persistent()) {
			r->destroy_visible(objects[i]);

			delete objects[i];
			std::swap(objects[i], objects.back());
			objects.pop_back();
		}
	}
	set_view_focus(NULL);

	if (m) {
		delete m;
	}
	LOG("loading Map " << fname);
	m = new Map(this, fname);

	for (auto &o : objects) {
		o->map_start();
	}
}

void World::show_text(const char *msg) {
	mode = World::MODE_TEXT;
	textbox.set_msg(msg);

}

void World::stop_text() {
	if (mode == World::MODE_TEXT)
		mode = World::MODE_OVERWORLD;
}

void World::render() {
	r->render(*m);
}

void World::clear_dead_objects(){
	for (auto& o : dead_visibles){
		r->destroy_visible(o);
	}

	for (auto& o : dead_objects) {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (*(objects[i]) == *o) {
				delete objects[i];
				std::swap(objects[i], objects.back());
				objects.pop_back();

				i--;
			}
		}
	}

	dead_visibles.clear();
	dead_objects.clear();
	/*
	   remove pointers to id from objects and r->visibles (if any)
	   then delete the object
	   */
}

void World::queue_destroy(Obj *o){
	dead_objects.push_back(o);
}
void World::queue_destroy_visible(VisibleObj *o){
	dead_objects.push_back(o);
	dead_visibles.push_back(o);
}
