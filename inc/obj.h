#ifndef OBJ_H
#define OBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "global.h"
#include "sprite.h"
#include "level.h"
#include "script.h"


typedef enum OBJTYPE { 
	OBJ, 
	OBJ_PHYSICAL, 
	OBJ_VISIBLE, 
	OBJ_MOBILE, 
	OBJ_DUMMY, 
	OBJ_PLAYER, 
	OBJ_PROP, 
	OBJ_ENEMY,
	OBJ_ENEMY_SPIRIT,
	OBJ_NOSEMAN,
	OBJNUM
} ObjType;

class World;
class VisibleObj;
/* Basic object. Holds the total number of instances, an id, and can update */
class Obj {
	public:
		Obj();
/*		Obj(World *world); */
		virtual ~Obj();
		void attach_to_world(World *world);

		virtual void update();
		virtual void map_start();
		virtual void map_end();
		virtual bool destroy();
/*		virtual void collide(Obj *other); */
		virtual void interact();
		
		void set_active(bool active);
		void set_persistent(bool persistent);

		bool is_active() const;
		bool is_persistent() const;
		bool is_solid() const;
		bool is_dead() const;
		int get_id() const;
		virtual Box get_bbox() const;
		float distance_from_point(float x, float y);

		bool operator ==(const Obj &rhs);
		bool operator !=(const Obj &rhs);

		virtual ObjType get_type() const;

	protected:
		World *world;
		bool active;
		int id;
		static int objtotal;
		bool persistent;
		std::queue<GenericTween*> tweens;
		bool solid = false;
		bool dead = false;
};


/* Physical Object. Has a location, a width, and a height */
class PhysicalObj : public Obj {
	public:
		PhysicalObj(float x=0.0f, float y=0.0f, float w=0, float h=0);
/*		PhysicalObj(World *world, float x=0.0f, float y=0.0f, float w=0, float h=0); */
		virtual ~PhysicalObj();
		float get_x() const;
		float get_y() const;
		float get_z() const;
		float get_w() const;
		float get_h() const;
		virtual Box get_bbox() const;

		void set_position(float x=0.0f, float y=0.0f);
		void displace(float dx=0.0f, float dy=0.0f);
		void displace(Vec2f disp);

		virtual ObjType get_type() const;
	protected:
		typedef Obj super;
		float x, y, z;
		float w, h;
};


/* Visible object. Has a position, size, depth, and possibly knows a sprite. Can draw itself. */
/* Compared by depth, so they can be drawn in order. */
class VisibleObj : public PhysicalObj {
	public:
		VisibleObj(float x=0.0f, float y=0.0f, float w=16.0f, float h=16.0f, int depth=0, SpriteSheet *s=NULL);
/*		VisibleObj(World *world, float x=0.0f, float y=0.0f, float w=0.0f, float h=0.0f, int depth=0, SpriteSheet *s=NULL); */
		virtual ~VisibleObj();

		virtual void set_sprite(SpriteSheet *s, int index=0);
		virtual void set_sprite(Sprite *sprite);

		virtual void draw();
		virtual bool destroy();
		bool operator<(const VisibleObj &rhs);
		int depth;

		virtual ObjType get_type() const;
	protected:
		typedef PhysicalObj super;
		bool loop;
		SpriteSheet *sheet;
		Sprite *sprite;
		float aspeed;
		float frame_index;
		bool visible;
		bool hflip, vflip;
		float alpha;
};


/* Mobile object. Has horizontal and vertical speed and moves at those speeds when updated. */
class MobileObj : public VisibleObj {
	public:
		MobileObj();
		MobileObj(float x=0.0f, float y=0.0f, float w=0, float h=0, int depth=0, SpriteSheet *s=NULL);
/*		MobileObj(World *world, float x=0.0f, float y=0.0f, float w=0, float h=0, int depth=0, SpriteSheet *s=NULL); */
		virtual ~MobileObj();
		float get_dx() const;
		float get_dy() const;
		virtual void update();

		void face_point(float ox, float oy);

		virtual ObjType get_type() const;
	protected:
		typedef enum compassdirs {DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW} CompassDir;
		CompassDir direction;
		typedef VisibleObj super;
		float dx, dy;
		void collide_with_tiles();
};


/* Prop. Static object (not C++ static), sprite determined by map file. Possibly solid. */

typedef enum PROPTYPE { PROP_CANDELABRUM, PROP_CANDELABRUM_LIT, PROP_NUM } PropType;
class Prop : public VisibleObj {
	public:
		Prop(float x, float y, PropType t);
		virtual void update();
		virtual Box get_bbox() const;

		virtual ObjType get_type() const;
		virtual void interact();

	protected:
		PropType t;
		typedef VisibleObj super;
		const char *mymsg;
};


/* Dummy, for testing stuff */
class Dummy : public MobileObj {
	public:
		Dummy(float x=0.0f, float y=0.0f);
		virtual ~Dummy();
		virtual void update();
		virtual Box get_bbox() const;
		virtual void draw();

		virtual ObjType get_type() const;

		virtual void interact();
	protected:
		typedef MobileObj super;
		Sprite *spr_shadow;
		const char *mymsg = "If you can read this, then object interaction is working!";
		
};

/* A mysterious figure */
class Noseman : public MobileObj {
	public:
		Noseman(float x=0.0f, float y=0.0f);
		~Noseman();
		void update();
		Box get_bbox() const;
		void draw();

		ObjType get_type() const;

		void interact();
	protected:
		typedef MobileObj super;
		Sprite *spr_shadow;
};


/* An overworld enemy object! */
class Enemy : public MobileObj {
	public:
		Enemy(float x=0.0f, float y=0.0f, float w=0.0f, float h=0.0f, SpriteSheet *s=NULL, std::string cname="Enemy");
		virtual ~Enemy();
		virtual void update();
		virtual Box get_bbox() const;
		virtual void draw();

		virtual ObjType get_type() const;

		bool is_invincible() const;
		bool is_aggro() const;
	protected:
		typedef MobileObj super;
		Sprite *spr_shadow;
		bool aggro = false;
		bool invincible = false;
};


/* Player object. */
class Player : public MobileObj {
	public:
		Player();
		Player(float x=0.0f, float y=0.0);
/*		Player(World *world, float x=0.0f, float y=0.0); */
		virtual ~Player();

		virtual void update();
		virtual void map_start();
		virtual void draw();

		bool is_sneaking() const;

		virtual Box get_bbox() const;

		virtual ObjType get_type() const;
	protected:
		int spritenum;
		typedef enum pose {SPR_STAND, SPR_WALK_DOWN, SPR_WALK_DOWN_RIGHT, SPR_WALK_RIGHT, SPR_WALK_UP, SPR_WALK_UP_RIGHT, SPRNUM} Pose;
		typedef MobileObj super;
		int score;
		Sprite *sprites[SPRNUM];
		Sprite *spr_shadow;
		float maxspeed = 1.0f;

		bool can_sneak = true;
		bool sneaking = false;
		int sneak_time = 0;
		int sneak_time_max = 60*5;
		int sneak_cooldown = 0;

};

#endif
