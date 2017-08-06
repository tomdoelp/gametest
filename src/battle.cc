#include "battle.h"
#include "sprite.h"
#include "world.h"
#include "obj.h"

/* useless */
Action::Action(Combatant *source, Combatant *target, std::string action_text, std::string action_name) : 
	source(source),
	target(target),
	action_text(action_text),
	action_name(action_name) {}
Action::~Action() {}
void Action::execute() {
	LOG(action_text);
}
ActionType Action::get_type() const { return ACT_ACTION; }
std::string Action::get_action_name() const { return action_name; }
std::string Action::get_action_text() const { return action_text; }
bool Action::operator < (const Action &rhs) {
	return this->get_type() < rhs.get_type();
}

/* TODO: remove battles entirely! no longer an RPG */




///////////////////////////////////////////////////////////////////////////////////
// ACT ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

Act_Run::Act_Run(Combatant *source, Combatant *target, std::string action_text, std::string action_name) : Action(source, target, action_text, action_name) {}
Act_Run::~Act_Run() {}
ActionType Act_Run::get_type() const { return ACT_RUN; }

Act_Attack::Act_Attack(Combatant *source, Combatant *target, std::string action_text, std::string action_name) : Action(source, target, action_text, action_name) {}
Act_Attack::~Act_Attack() {}
ActionType Act_Attack::get_type() const { return ACT_ATT; }

Act::Act(Combatant *source, std::vector<Combatant *> targets, ActionType type, Battle *battle, int speed) : 
	source(source), 
	targets(targets), 
	type(type),
	battle(battle),
	speed(speed) {
	
		switch (type) {
			default:
			case ACT_ACTION:
			case ACT_RUN:
				target_num = 0;
				break;
			case ACT_ATT:
				target_num = 1;
				break;
		}
}
Act::Act(Combatant *source, Combatant *target, ActionType type, Battle *battle, int speed) : 
	source(source), 
	type(type),
	battle(battle),
	speed(speed) {
		targets.push_back(target);
	
		switch (type) {
			default:
			case ACT_ACTION:
			case ACT_RUN:
				target_num = 0;
				break;
			case ACT_ATT:
				target_num = 1;
				break;
		}
}
Act::~Act() {}
void Act::execute() {
	std::string msg;
	switch (type) {
		default:
		case ACT_ACTION:
			msg = source->get_combat_name() + " did nothing.";
			break;
		case ACT_RUN:
			msg = source->get_combat_name() + " ran away...";
			break;
		case ACT_ATT:
			/* TODO highlight source! */
			msg = source->get_combat_name() + " attacked!";
			if (battle) {
				battle->get_world()->sndmgr->play_sound(SND_HIT);
				battle->set_battle_msg(msg);
			}
			for (auto &c : targets) {
				int damage = source->get_offense();
				if (battle)
					battle->show_damage(c, damage, true);

				c->set_hp(c->get_hp() - source->get_offense());
				battle->start_tween(new LoopTween<float>(&c->y, c->get_y(false), c->get_y(false)-8.0f, 3, 2, 0.5));
/*				LOG(source->get_offense() << " damage to " << c->get_combat_name()); */
			}
			break;
	}
	LOG(msg);
	if (battle) {
		battle->set_battle_msg(msg);
	}
}
void Act::set_source(Combatant *source) { this->source = source; }
void Act::set_targets(std::vector<Combatant *> targets) { this->targets = targets; }
void Act::set_target(Combatant *target) { 
	targets.clear();
	targets.push_back(target);
}
void Act::add_target(Combatant *target) { 
	targets.push_back(target);
}
void Act::set_speed(int speed) { this->speed = speed; }
int Act::get_speed() const { return speed; }

int Act::get_target_num() const { return target_num; }

bool Act::operator < (const Act &rhs) const { return speed < rhs.get_speed(); }
bool Act::operator > (const Act &rhs) const { return speed > rhs.get_speed(); }

Combatant::Combatant(std::string combat_name, ALLEGRO_BITMAP *image, int hp, int pp, int hp_max, int pp_max, int level, int xp, int offense, int defense, int magic, int speed, int luck) : 
	combat_name(combat_name), 
	image(image),
	hp(hp), 
	pp(pp), 
	hp_max(hp_max), 
	pp_max(pp_max), 
	lv(level), 
	xp(xp), 
	offense(offense), 
	defense(defense), 
	magic(magic), 
	spd(speed), 
	luck(luck)
{
	next=NULL;
	add_action(ACT_ACTION);
	dead = false;
	x = 0.0f;
	y = 0.0f;
	spr_shadow = (*SheetManager::get_sheet(SH_SHADOW))[0];
	spr_shadow->sprite_center_origin(ORIGIN_CENTER_MIDDLE);
}
Combatant::~Combatant() {
	for(auto &a : available_actions) {
		delete a;
		a=NULL;
	}
}
int Combatant::get_hp() const { return hp; }
int Combatant::get_pp() const { return pp; }
int Combatant::get_hp_max() const { return hp_max; }
int Combatant::get_pp_max() const { return pp_max; }
int Combatant::get_level() const { return lv; }
int Combatant::get_xp() const { return xp; }
int Combatant::get_offense() const { return offense; }
int Combatant::get_defense() const { return defense; }
int Combatant::get_magic() const { return magic; }
int Combatant::get_speed() const { return spd; }
int Combatant::get_luck() const { return luck; }
std::string Combatant::get_combat_name() const { return combat_name; }
const std::vector<Action *> &Combatant::get_available_actions() const { return available_actions; }
const std::vector<ActionType> &Combatant::get_actions() const { return actions; }
bool Combatant::is_dead() const { return dead; }
Combatant *Combatant::get_next() const { return next; }
void Combatant::set_hp(int hp) {
	this->hp = hp; 
	if (hp <= 0)
		dead = true;
	else
		dead = false;
}
float Combatant::get_x(bool centered) const { return x; }
float Combatant::get_y(bool centered) const { 
	if (centered && image) 
		return y - al_get_bitmap_height(image)/2;
	else 
		return y; 
}
void Combatant::set_pp(int pp) { this->pp = pp; }
void Combatant::set_hp_max(int hp_max) { this->hp_max = hp_max; }
void Combatant::set_pp_max(int pp_max) { this->pp_max = pp_max; }
void Combatant::set_level(int lv) { this->lv = lv; }
void Combatant::set_xp(int xp) { this->xp = xp; }
void Combatant::set_offense(int offense) { this->offense = offense; }
void Combatant::set_defense(int defense) { this->defense = defense; }
void Combatant::set_magic(int magic) { this->magic = magic; }
void Combatant::set_speed(int speed) { this->spd = speed; }
void Combatant::set_luck(int luck) { this->luck = luck; }
void Combatant::set_combat_name(std::string name) { this->combat_name = name; }
void Combatant::set_parent(Obj *o) { this->parent = o; }
void Combatant::set_image(ALLEGRO_BITMAP *image) { this->image = image; }
void Combatant::set_position(float x, float y) { this->x = x; this->y = y; }

void Combatant::add_action(ActionType action) {
	actions.push_back(action);
	std::sort(actions.begin(), actions.end());
}
std::ostream& operator << (std::ostream& out, const Combatant &c) {
	return out << "\t" << c.get_combat_name() << "\n\tHP: " << c.get_hp();
}

void Combatant::end() {
	if (dead && parent) {
		parent->destroy();
	}
}

void Combatant::draw(bool shadow) {
	if (!dead) {
		if (shadow && spr_shadow) {
			spr_shadow->sprite_draw(x,y,0.0f);
		}


		if (image) {
			float w = al_get_bitmap_width(image);
			float h = al_get_bitmap_height(image);
			/*		sprite->sprite_draw(x+this->x, y+this->y); */
			al_draw_tinted_scaled_rotated_bitmap(
					image, 
					al_map_rgb(255,255,255), 
					round_nearest(w/2), 
					round_nearest(h), 
					round_nearest(x), 
					round_nearest(y), 
					1.0f,//xscale, 
					1.0f,//yscale, 
					0.0f,//angle, 
					0//flags
					);
		}
		else {
			al_draw_filled_ellipse(x, y, 16.0f, 16.0f, al_map_rgb(0,0,125));
		}
	}
}

void Combatant::draw_stats(float x, float y) {
	char statbuf[20];
	sprintf(statbuf, "HP:%d/%d\nMP:%d/%d", hp, hp_max, pp, pp_max);
	al_draw_multiline_textf(
			font_tamsynb, 
			al_map_rgb(255,255,255), 
			x, 
			y, 
			SCREEN_W, 
			12.0f, 
			ALLEGRO_ALIGN_CENTER | ALLEGRO_ALIGN_INTEGER, 
			statbuf
			);



}


///////////////////////////////////////////////////////////////////////////////
// BATTLE /////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


Battle::Battle(float x, float y) : x(x), y(y) {
	LOG("\n\n****************\nBattle starts!");
	world = NULL;
	active_act = NULL;
	phase = PHASE_START;
}
Battle::Battle(float x, float y,std::vector<Combatant *> enemies) : x(x), y(y), enemies(enemies) {
	LOG("\n\n****************\nBattle starts!");
	world = NULL;
	active_act = NULL;
	phase = PHASE_START;
}
Battle::Battle(float x, float y, Combatant *enemy) : x(x), y(y) {
	LOG("\n\n****************\nBattle starts!");
	world = NULL;
	active_act = NULL;
	phase = PHASE_START;
	enemies.push_back(enemy);
}

Battle::~Battle() {
	while (!tweens.empty()) {
		delete tweens.front();
		tweens.pop();
		LOG("Battle destroyed active tween");
	}
	LOG("Battle ends.\n****************\n\n");
}

void Battle::battle_continue() { cont = true; }

void Battle::show_damage(Combatant *target, int amount, bool dmg) {
	if (dmg) {
		effects.emplace_back(target->get_x(), target->get_y(true), std::to_string(amount), 40);
	}
}

void Battle::update_tweens() {
	if (tweens.size() > 0) {
		std::queue<GenericTween*> tempqueue;
		while (!tweens.empty()) {
			if (tweens.front()->step()) {
				tempqueue.push(tweens.front());
				tweens.pop();
			}
			else {
				delete tweens.front();
				tweens.pop();
				LOG("tween finished and destroyed");
			}
		}
		swap(tweens, tempqueue);
	}
}

void Battle::update() {

	/* timer business */
	if (key_press[ALLEGRO_KEY_X]) {
		timer = 0;
		cont = true;
		clear_battle_msg();
	}
	if (timer > 0)
		timer--;
	else if (!cont) {
		clear_battle_msg();
		cont = true;
	}




	update_tweens();


	/* update effect logic */
	for (unsigned int i = 0; i < effects.size(); i++) {
		effects[i].update();

		if (effects[i].is_dead()) {
			std::swap(effects[i], effects.back());
			effects.pop_back();
			i--;
		}
	}


	if (kmap(ALLEGRO_KEY_ESCAPE)) 
		world->end_battle();
	switch (phase) {
		case PHASE_CHOICE:
			{
				int choice = -1;
				if (key_press[ALLEGRO_KEY_PAD_0]) {
					choice = 0;
				}
				if (key_press[ALLEGRO_KEY_PAD_1]) {
					choice = 1;
				}
				if (key_press[ALLEGRO_KEY_PAD_2]) {
					choice = 2;
				}
				if (key_press[ALLEGRO_KEY_PAD_3]) {
					choice = 3;
				}

				if (choice >= 0 && unsigned(choice) < active_choice->get_actions().size()){
					round_actions.emplace_back(
							active_choice, 
							std::vector<Combatant *>(), 
							active_choice->get_actions()[choice], 
							this);
					active_act = &round_actions.back();
					active_act->set_speed(active_choice->get_speed());
					if (active_act->get_target_num() > 0) {
						change_phase(PHASE_TARGET);
					}
					else {
						change_active(active_choice->get_next()); 
					}
				}
			}
			break;
		case PHASE_TARGET:
			{
				int choice = -1;
				if (key_press[ALLEGRO_KEY_PAD_0]) {
					choice = 0;
				}
				if (key_press[ALLEGRO_KEY_PAD_1]) {
					choice = 1;
				}
				if (key_press[ALLEGRO_KEY_PAD_2]) {
					choice = 2;
				}
				if (key_press[ALLEGRO_KEY_PAD_3]) {
					choice = 3;
				}

				if (choice >= 0 && unsigned(choice) < enemies.size()) {
					if (active_act) {
						active_act->set_target(enemies[choice]);
						/* ... */
						active_act = NULL;

						change_active(active_choice->get_next());
					}
				}


			}
			break;

		case PHASE_ACTION:
			/* check timer flag before continuing*/
			/* Idea: obj that is just a sprite + tweens, destroys self when finished animation*/
			if (cont) {
				if (check_for_end()) {
					change_phase(PHASE_END);
				}
				else if (round_actions.size() == 0) {
					change_phase(PHASE_CHOICE);
				} else {
					cont = false; 
					timer = 40;
					round_actions.back().execute();
					round_actions.pop_back();
				}
			}
			/* TODO check for end after each action! */
			break;
		case PHASE_START:
			change_phase(PHASE_CHOICE);
			break;
		case PHASE_END:
			if (kmap(ALLEGRO_KEY_X)) {
				battle_end();
			}
		default:
			break;
	}
}

void Battle::change_phase(BattlePhase phase) {
	this->phase = phase;
	clear_battle_msg();

	switch (phase) {
		case PHASE_CHOICE:
			{
				if (check_for_end()) {
					change_phase(PHASE_END);
					/*					battle_end(); */
				}
				else {

					if (world) {
						/* Set up combatant data */
						if (!party_leader) {
							party_leader = world->get_player()->get_combatant();

							/* calculate party size */
							party_size = 0;
							Combatant *temp = party_leader;
							while (temp) {
								party_size++;
								temp = temp->get_next();
							}

							/* calculate screen positions for all combatants */
							float screen_width_slice=0.0f;
							if (party_size > 0) 
								screen_width_slice = SCREEN_W / party_size;

							temp = party_leader;
							int counter = 0;
							while (temp) {
								temp->set_position(
										x+counter*screen_width_slice + screen_width_slice/2,
										y + SCREEN_H - 32.0f);
								temp = temp->get_next();
								counter++;
							}

							/* TODO enemy combatant screen positions */
							for (auto &e : enemies) {
								e->set_position(x+SCREEN_W/2, y+SCREEN_H/2 + 16.0f);
							}

						}
						change_active(party_leader);
					}
				}
			}
			break;
		case PHASE_TARGET:
			{
				LOG("Select target:");

				int num = 0;
				for (auto &e : enemies) {
					LOG(num++ << ". " << e->get_combat_name());
				}

			}
			break;
		case PHASE_END:
			if (world) {
				world->sndmgr->play_music(MUS_BATTLE_END, false);
			}
			LOG("*** IT'S OVER ***");
			cont = false;
			break;
		case PHASE_ACTION:
		default:
			break;
	}
}

void Battle::change_active(Combatant *c) {
	if (c) {
		active_choice = c;
		/*		LOG(*active_choice); */

		int num = 0;
		for(auto &s : active_choice->get_actions()) {
			LOG(num++ << ". " << get_action_name(s));
		}
	}
	else {
		/* go through enemy list and get their actions */
		for (auto &e : enemies) {
			/* randomly select from available actions */
			/* TODO add some weight value */
			int index = rand() % int(e->get_actions().size());
			Combatant *target = world->get_player()->get_combatant();

			round_actions.emplace_back(
					e, 
					target, 
					e->get_actions()[index], 
					this);
			round_actions.back().set_speed(e->get_speed());
		}

		/* Sort actions by ascending speed */
		/* (because we're gonna execute back to front) */
		std::sort(round_actions.begin(), round_actions.end());
		change_phase(PHASE_ACTION);
	}
}

/* Get the string names of moves based on their type enum */
std::string Battle::get_action_name(ActionType t) {
	switch(t) {
		default:
		case ACT_ACTION:
			return "(nothing)";
			break;
		case ACT_RUN:
			return "Run";
			break;
		case ACT_ATT:
			return "Attack";
			break;
	}
}

void Battle::battle_end() {
	for (auto &e : enemies) {
		e->end();
	}

	if (world)
		world->end_battle();

}

World *Battle::get_world() const { return world; }

void Battle::draw() { 
	/* draw enemies */
	for (auto &e : enemies) {
		e->draw();
	}

	/* draw party */
	Combatant *temp = party_leader;
	float screen_width_slice = 0.0f;
	if (party_size > 0) {
		screen_width_slice = SCREEN_W / party_size;
	}
	int counter = 0;

	while (temp) {
		temp->draw(true);
		temp->draw_stats(x+counter*screen_width_slice + screen_width_slice/2, y+SCREEN_H-32.0f);
		temp = temp->get_next();
		counter++;
	}

	/* draw effects last */
	for (auto &e : effects) {
		e.draw();
	}

	/* draw text box */
	if (msg_visible) {
		al_draw_filled_rectangle(x+8.0f, y+8.0f, x+8.0f+SCREEN_W-16.0f, y+8.0f+59.0f, al_map_rgb(0,0,0));
		al_draw_multiline_textf(
				font_tamsyn, 
				al_map_rgb(255,255,255), 
				x+16.0f, 
				y+16.0f, 
				SCREEN_W-16.0f, 
				12.0f, 
				ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER, 
				battle_msg.c_str()
				);


	}



}

bool Battle::check_for_end() {
	bool over = true;
	for (auto &e : enemies) {
		if (!e->is_dead()) {
			over = false;
		}
	}
	return over;
}

void Battle::start_tween(GenericTween *tween) {
	tweens.push(tween);
}
void Battle::set_battle_msg(std::string msg) {
	if (msg != std::string()) {
		msg_visible = true;
		battle_msg = msg;
	}
}
void Battle::clear_battle_msg() {
	msg_visible = false;
	battle_msg = "";
}

/////////////////////////////////////////////////////////////////////
// EFFECT ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

Effect::Effect(float x, float y, Sprite *sprite) : x(x), y(y), sprite(sprite) {
	msg = "";
	text = false;
}
Effect::Effect(float x, float y, std::string msg, int lifetime) : x(x), y(y), msg(msg) {
	sprite = NULL;
	text = true;
	timer = lifetime;
}
void Effect::destroy() { dead = true; }
bool Effect::is_dead() { return dead; } 
void Effect::update() {
	if (timer > 0) {
		timer--;

		if (timer <= 0) 
			destroy();
	}
}
void Effect::draw() {
	if (text) {
		al_draw_multiline_textf(
				font_tamsynb, 
				al_map_rgb(255,255,255), 
				x, 
				y, 
				SCREEN_W, 
				12.0f, 
				ALLEGRO_ALIGN_CENTER | ALLEGRO_ALIGN_INTEGER, 
				msg.c_str()
				);
	}
}
