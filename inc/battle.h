#ifndef BATTLE_H
#define BATTLE_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "global.h"
#include "script.h"

class Sprite;
class Combatant;
class World;
class Battle;
class Obj;

typedef enum ACTTYPE { ACT_ACTION, ACT_RUN, ACT_ATT, ACTNUM } ActionType;
class Act /* jokes */ {
	public:
		Act(Combatant *source, std::vector<Combatant *> targets, ActionType type, Battle *battle=NULL, int speed=0);
		Act(Combatant *source, Combatant *target, ActionType type, Battle *battle=NULL, int speed=0);
		~Act();
		void execute();

		void set_source(Combatant *source);
		void set_targets(std::vector<Combatant *> targets);
		void set_target(Combatant *target);
		void add_target(Combatant *target);
		void set_speed(int speed);

		int get_speed() const;
		int get_target_num() const;

		bool operator < (const Act &rhs) const;
		bool operator > (const Act &rhs) const;
	protected:
		Combatant *source;
		std::vector<Combatant *> targets;
		ActionType type;
		Battle *battle;
		int speed;
		int target_num;
};

/* Probably won't be used */
class Action {
	public:
		Action(Combatant *source, Combatant *target, std::string action_text="did something!", std::string action_name="Act");
		virtual ~Action();
		virtual void execute();
		std::string get_action_text() const;
		std::string get_action_name() const;
		virtual ActionType get_type() const;

		void set_source(Combatant *s);
		void set_target(Combatant *t);
		void set_text(std::string text);
		void set_name(std::string name);

		bool operator < (const Action &rhs);
	protected:
		Combatant *source;
		Combatant *target;
		std::string action_text;
		std::string action_name;
};

class Act_Run : public Action {
	public:
		Act_Run(Combatant *source, Combatant *target=NULL, std::string action_text="ran away.", std::string action_name="Run away");
		~Act_Run();
/*		void execute();  */
		ActionType get_type() const;
};

class Act_Attack : public Action {
	public:
		Act_Attack(Combatant *source, Combatant *target=NULL, std::string action_text="attacked!", std::string action_name="Attack");
		~Act_Attack();
/*		void execute(); */
		ActionType get_type() const;
};



/* A combatant has all the stats necessary for participating in a battle.
 * They will typically also appear in the overworld, so that will probably
 * be a multiple inheritance.
 *
 * JUST KIDDING! Overworld objs will own a combatant. 
 *
 */
class Combatant {
	public:
		friend class Act;
		Combatant(
				std::string combat_name="Combatant", 
				ALLEGRO_BITMAP *image=NULL,
				int hp=10, 
				int pp=10, 
				int hp_max=10,
				int pp_max=10,
				int level=1,
				int xp=0,
				int offense=1,
				int defense=1,
				int magic=1,
				int speed=1,
				int luck=1
				);
		virtual ~Combatant();

		int get_hp() const;
		int get_pp() const;
		int get_hp_max() const;
		int get_pp_max() const;
		int get_level() const;
		int get_xp() const;
		int get_offense() const;
		int get_defense() const;
		int get_magic() const;
		int get_speed() const;
		int get_luck() const;
		std::string get_combat_name() const;
		const std::vector<Action*> &get_available_actions() const;
		const std::vector<ActionType> &get_actions() const;
		bool is_dead() const;
		Combatant *get_next() const;
		float get_x(bool centered=true) const;
		float get_y(bool centered=true) const;
		float get_x_text() const;
		float get_y_text() const;

		void set_hp(int hp);
		void set_pp(int pp);
		void set_hp_max(int hp_max);
		void set_pp_max(int pp_max);
		void set_level(int lv);
		void set_xp(int xp);
		void set_offense(int offense);
		void set_defense(int defense);
		void set_magic(int magic);
		void set_speed(int speed);
		void set_luck(int luck);
		void set_combat_name(std::string name);
		void set_parent(Obj *o);
		void set_image(ALLEGRO_BITMAP *image);

		void set_position(float x, float y);

		void add_action(ActionType action);
		template <class T> void add_action(Combatant *source) {
			T *act = new T(source);
			available_actions.push_back(act);
			std::sort(available_actions.begin(), available_actions.end());
		}


		friend std::ostream& operator << (std::ostream& out, const Combatant &c);

		void end();

		void draw(bool shadow=false);
		void draw_stats(float x, float y);

	protected:
		std::string combat_name;
		ALLEGRO_BITMAP *image;
		int hp, pp;
		int hp_max, pp_max;
		int lv, xp;
		int offense, defense, magic;
		int spd, luck;

		std::vector<Action *> available_actions;
		std::vector<ActionType> actions;

		Combatant *next;

		Obj *parent;
		bool dead;

		float x, y;
		Sprite *spr_shadow;
};

/*
typedef enum ENEMYTYPE { EN_TEST, ENNUM } EnemyType;
class BattleEnemy : public Combatant {
	public:
		BattleEnemy();
		virtual ~BattleEnemy();

		EnemyType get_enemy_type();

	protected:
		Sprite* spr;
		EnemyType en_type;
		std::queue<GenericTween*> tweens;
};
*/

class Effect {
	public:
		friend class Battle;
		Effect(float x, float y, Sprite *sprite);
		Effect(float x, float y, std::string msg, int lifetime=0); /*TODO color */

		bool is_dead();

		void update();
		void draw();
		void destroy();
	protected:
		float x, y;
		Sprite *sprite;
		std::string msg;
		bool text;
		bool dead = false;
		int timer;
};
		

class Battle {
	public:
		friend class World;
		Battle(float x, float y);
		Battle(float x, float y, std::vector<Combatant *> enemies);
		Battle(float x, float y, Combatant *enemy);
		~Battle();

		void update();
		void battle_continue();
		void show_damage(Combatant *target, int amount, bool dmg=true);
		void start_tween(GenericTween *tween);

		std::vector<Combatant *> get_party();
		std::vector<Combatant *> get_enemies();

		void battle_end();

		World *get_world() const;

		void draw();

		void set_battle_msg(std::string msg);
		void clear_battle_msg();
	protected:
		float x, y;
		World *world;
		typedef enum BATPHASE { PHASE_START, PHASE_CHOICE, PHASE_TARGET, PHASE_ACTION, PHASE_END, PHASENUM } BattlePhase;
		BattlePhase phase;
		void change_phase(BattlePhase phase);
		void change_active(Combatant *c);
		bool check_for_end();

		std::vector<Combatant*> enemies;
		std::vector<Act> round_actions;
		std::vector<Combatant*> targets;
		std::vector<Effect> effects;

		Combatant *party_leader=NULL;
		int party_size=0;
		Combatant *active_choice;
		Act *active_act;

		float lb_thickness = 0.0f; // to be tweened to...
		const float lb_thickness_final = 16.0f;

		int timer = 0;

		void execute_action(Combatant *souce, std::vector<Combatant *> targets, ActionType t);

		std::string get_action_name(ActionType t);

		bool done = false;
		bool cont = true;

		std::queue<GenericTween*> tweens;
		void update_tweens(); 

		bool msg_visible = false;
		std::string battle_msg;
};


/* Combatant class with all battle data (stats, hp, pp, you name it)
 * Overworld objects inherit from this, they might as well own the info 
 * (Will enemies remember battles if they survive? Yes, I think so)
 *
 * Also, lets make a class for battle action.
 * It will have a source, a (list of) target(s), and a function that will 
 * be pointed at the target(s). The combatant class must have every
 * value that a move can alter, so that the target can be anybody (including self,
 * teammates...). 
 *
 * A combatant owns a (list of) action(s). Only a Battle object can cause an 
 * action to be carried out. 
 *
 * A combatant also owns a list of items, and an action can be using an item. 
 *
 *
 * A Battle rotates between two phases: choice and action.
 * In the choice phase, we start at the Player, who owns a list of team members.
 * The player chooses from a menu of available actions for the current member, then
 * we move to the next one. If there aren't any more team members, the enemies all
 * choose their actions and then the action phase begins.
 *
 * At the start of the action phase, combatants are added to a queue
 * in descending order of speed. Then, each action is simply carried out in order, with 
 * some timers and stuff for drawing. But let's not worry about that just yet. Once
 * the list of combatants is over, we determine if the battle is over. If not, the
 * next choice phase begins. 
 *
 * Combatants can act if their HP is positive. If a target has nonpositive HP, we 
 * should have a method of changing targets automagically. 
 *
 */



#endif
