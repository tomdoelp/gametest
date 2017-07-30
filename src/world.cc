#include "world.h"
#include "battle.h"

World::World(Renderer *r) : r(r) {
	LOG("World created");
	m = NULL;
	player = NULL;
	battle = NULL;
	sndmgr = new SoundManager(AUDIO_DEPTH);
	sndmgr->init();
	mode = MODE_OVERWORLD;
	r->world = this;
	textbox.world = this;
	pmenu.world = this;

	for (int i = 0; i < TIMERNUM; i++) 
		timer[i] = 0;
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

	if (battle) {
		delete battle;
		battle = NULL;
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

void World::interact_with_object(Box b) {
	for (auto& o : objects) {
		if (o->is_active() && b.get_collision_vec(o->get_bbox()) != 0) {
			o->interact();
			return;
		}
	}
}

Vec2f World::get_object_collision_vec(Box now, Box next, ObjType t) {
	for (auto& o : objects) {
		if (o->is_active() && o->get_type() == t) {
			Vec2f intersect;

			float boxw = next.get_w();
			float boxh = next.get_h();
			float intersect_h = Box(next.get_x(), now.get_y(), boxw, boxh).get_collision_vec(o->get_bbox()).get_x();
			float intersect_v = Box(now.get_x(), next.get_y(), boxw, boxh).get_collision_vec(o->get_bbox()).get_y();
			if (intersect_h != 0.0f) {
				intersect.set_x(intersect_h);
			}
			if (intersect_v != 0.0f) {
				intersect.set_y(intersect_v); 
			}
			if ((intersect_h == intersect_v || -intersect_h == intersect_v ) && intersect_h != 0.0f){
				intersect.set_x(0.0f);
			}

			if (intersect != 0.0f)
				return intersect;
		}
	}

	return Vec2f(0.0f, 0.0f);
}

Vec2f World::get_object_collision_solid(Box now, Box next) {
	Vec2f intersect;
	for (auto& o : objects) {
		if (o->is_active() && o->is_solid()) {

			float boxw = next.get_w();
			float boxh = next.get_h();
			float intersect_h = Box(next.get_x(), now.get_y(), boxw, boxh).get_collision_vec(o->get_bbox()).get_x();
			float intersect_v = Box(now.get_x(), next.get_y(), boxw, boxh).get_collision_vec(o->get_bbox()).get_y();
			if (intersect_h != 0.0f) {
				intersect.set_x(intersect_h);
			}
			if (intersect_v != 0.0f) {
				intersect.set_y(intersect_v); 
			}
			if ((intersect_h == intersect_v || -intersect_h == intersect_v ) && intersect_h != 0.0f){
				intersect.set_x(0.0f);
			}

			if (intersect.get_x() != 0.0f && intersect.get_y() != 0.0f)
				return intersect;
		}
	}

	return intersect;
}


void World::update() {
	if (key_press[ALLEGRO_KEY_ESCAPE]) {
		if (mode == World::MODE_OVERWORLD) {
			mode = World::MODE_PAUSE;
			sndmgr->play_sound(SND_PAUSE);
		}
		else if (mode == World::MODE_PAUSE) {
			mode = World::MODE_OVERWORLD;
		}
	}

	switch(mode) {
		case MODE_OVERWORLD:
			for (auto &o : objects) {
				o->update();
			}
/*			clear_dead_objects(); */
			break;
		case MODE_PAUSE:
			pmenu.update();
			break;
		case MODE_TEXT:
			textbox.update();
			break;
		case MODE_BATTLE:
			if (battle)
				battle->update();
		default:
			break;
	}

	clear_dead_objects();

	/* timers */
	for (int i = 0; i < TIMERNUM; i++) {
		if (timer[i] > 0) {
			if (--timer[i] == 0) {
				switch (i) {
					case TIMER_TEST:
						LOG("Test timer ding ding!");
						break;
					default:
						break;
				}
			}
		}
	}

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

void World::start_battle() {
	sndmgr->play_sound(SND_BATTLE_START);
	sndmgr->play_music(MUS_BATTLE1);
	mode = MODE_BATTLE;

	if (!battle) {
		battle = new Battle(r->v.get_x(), r->v.get_y());
		battle->world = this;
	}
}
void World::start_battle(Combatant *enemy) {
	sndmgr->play_sound(SND_BATTLE_START);
	sndmgr->play_music(MUS_BATTLE1);
	mode = MODE_BATTLE;

	if (!battle) {
		battle = new Battle(r->v.get_x(), r->v.get_y(), enemy);
		battle->world = this;
	}
}
void World::start_battle(std::vector<Combatant *> enemies) {
	/*
	sndmgr->play_sound(SND_BATTLE_START);
	sndmgr->play_music(MUS_BATTLE1);
	mode = MODE_BATTLE;

	if (!battle) {
		battle = new Battle(r->v.get_x(), r->v.get_y(), enemies);
		battle->world = this;
	}
	*/
}
void World::end_battle() {
	/*
	sndmgr->play_music(MUS_TEST);
	mode = MODE_OVERWORLD;

	if (battle) {
		delete battle;
		battle = NULL;
	}
	*/
}

void World::set_mode(WorldMode mode) { this->mode = mode; }
World::WorldMode World::get_mode() { return mode; }

void World::set_player(Player *p) {
	player = p;
}

void World::load_map(const char* fname) {
	for (auto &o : objects) {
		o->map_end();
		if (!o->is_persistent())
			o->destroy();
	}
	clear_dead_objects();

	/*
	for (unsigned int i = 0; i<objects.size(); i++) {
		if (!objects[i]->is_persistent()) {
			r->destroy_visible(objects[i]);

			delete objects[i];
			std::swap(objects[i], objects.back());
			objects.pop_back();
		}
	}
	*/
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

Textbox *World::show_text(const char *msg) {
	mode = World::MODE_TEXT;
	textbox.set_msg(msg);
	return &textbox;
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
