#ifndef OBJ_H
#define OBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "global.h"
#include "sprite.h"
#include "level.h"

class World;
class VisibleObj;
/* Basic object. Holds the total number of instances, an id, and can update (nop) */
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
		

		void set_active(bool active);
		void set_persistent(bool persistent);

		bool is_active() const;
		bool is_persistent() const;
		int get_id() const;
		virtual Box get_bbox() const;

		bool operator ==(const Obj &rhs);
		bool operator !=(const Obj &rhs);
	protected:
		World *world;
		bool active;
		int id;
		static int objtotal;
		bool persistent;
};


/* Physical Object. Has a location, a width, and a height */
class PhysicalObj : public Obj {
	public:
		PhysicalObj(float x=0.0f, float y=0.0f, float w=0, float h=0);
/*		PhysicalObj(World *world, float x=0.0f, float y=0.0f, float w=0, float h=0); */
		virtual ~PhysicalObj();
		float get_x() const;
		float get_y() const;
		float get_w() const;
		float get_h() const;
		virtual Box get_bbox() const;

		void set_position(float x=0.0f, float y=0.0f);
		void displace(float dx=0.0f, float dy=0.0f);
		void displace(Vec2f disp);
	protected:
		typedef Obj super;
		float x, y;
		float w, h;
};


/* Visible object. Has a position, size, depth, and possibly knows a sprite. Can draw itself. */
/* Compared by depth, so they can be drawn in order. */
class VisibleObj : public PhysicalObj {
	public:
		VisibleObj(float x=0.0f, float y=0.0f, float w=16.0f, float h=16.0f, int depth=0, SpriteSheet *s=NULL);
/*		VisibleObj(World *world, float x=0.0f, float y=0.0f, float w=0.0f, float h=0.0f, int depth=0, SpriteSheet *s=NULL); */
		virtual ~VisibleObj();

		virtual void draw();
		virtual bool destroy();
		bool operator<(const VisibleObj &rhs);
		int depth;
	protected:
		typedef PhysicalObj super;
		bool loop;
		SpriteSheet *sheet;
		Sprite *sprite;
		float aspeed;
		float frame_index;
		bool visible;
		bool hflip, vflip;
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
	protected:
		typedef enum compassdirs {DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW} CompassDir;
		CompassDir direction;
		typedef VisibleObj super;
		float dx, dy;
};

class Dummy : public MobileObj {
	public:
		Dummy(float x=0.0f, float y=0.0f);
		virtual ~Dummy();
		virtual void update();
		virtual Box get_bbox() const;
		virtual void draw();
	protected:
		typedef MobileObj super;
		Sprite *spr_shadow;
		
};


/* Player object. Has a score. Horizontal and vertical motion controlled with arrow keys. */
/* Just a scratchpad for ideas, really */
class Player : public MobileObj {
	public:
		Player();
		Player(float x=0.0f, float y=0.0);
/*		Player(World *world, float x=0.0f, float y=0.0); */
		virtual ~Player();

		virtual void update();
		virtual void map_start();
		virtual void draw();

		virtual Box get_bbox() const;

	protected:
		int spritenum;
		typedef enum pose {SPR_STAND, SPR_WALK_DOWN, SPR_WALK_DOWN_RIGHT, SPR_WALK_RIGHT, SPR_WALK_UP, SPR_WALK_UP_RIGHT} Pose;
		typedef MobileObj super;
		int score;
		Sprite *sprites[6];
		Sprite *spr_shadow;
};

#endif
