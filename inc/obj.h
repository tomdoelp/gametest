#ifndef __OBJ_H__
#define __OBJ_H__

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "global.h"
#include "sprite.h"

/* Basic object. Holds the total number of instances, an id, and can update (nop) */
class Obj {
	public:
		Obj();
		virtual void update();
		void activate();
		void deactivate();
		bool isactive();
	protected:
		bool active;
		int id;
		static int objtotal;
};


/* Physical Object. Has a location, a width, and a height */
class PhysicalObj : public Obj {
	public:
		PhysicalObj(float x=0.0, float y=0.0, float w=0, float h=0);
		~PhysicalObj();
	protected:
		typedef Obj super;
		float x, y;
		float w, h;
};

/* Solid object. Player can't move through these? */
class SolidObj : public virtual PhysicalObj {
	public:
		SolidObj(float x=0.0, float y=0.0, float w=0, float h=0);
		~SolidObj();
	protected:
		typedef PhysicalObj super;
};


/* Visible object. Has a position, size, depth, and possibly knows a sprite. Can draw itself. */
/* Compared by depth, so they can be drawn in order. */
class VisibleObj : public virtual PhysicalObj {
	public:
		VisibleObj(float x=0.0, float y=0.0, float w=0.0, float h=0.0, int depth=0, Sprite *s=NULL);
		~VisibleObj();
		virtual void draw();
		bool operator<(const VisibleObj &rhs);
		int getDepth() const;
	protected:
		typedef PhysicalObj super;
		bool loop;
		int depth;
		Sprite *sprite;
		float aspeed;
		float frame_index;
		bool visible;
};


/* Wall. A solid, visible object. Player can't move through these. */
class Wall : public SolidObj, public VisibleObj {
	public:
		Wall(float x=0.0, float y=0.0, float w=0, float h=0, int depth=0);
		~Wall();
		virtual void draw();
	protected:
		typedef PhysicalObj super;
};


/* Mobile object. Has horizontal and vertical speed and moves at those speeds when updated. */
class MobileObj : public VisibleObj {
	public:
		MobileObj();
		MobileObj(float x=0.0, float y=0.0, float w=0, float h=0, int depth=0, Sprite *s=NULL);
		~MobileObj();
		virtual void update();
	protected:
		typedef VisibleObj super;
		float dx, dy;
};


/* Player object. Has a score. Horizontal and vertical motion controlled with arrow keys. */
class Player : public MobileObj {
	public:
		Player();
		Player(float x=0.0, float y=0.0, float w=0, float h=0, int depth=0, Sprite *s=NULL);
		~Player();
		virtual void update();
		virtual void draw();
	protected:
		typedef MobileObj super;
		int score;

		bool bb_collision(float x, float y, float w, float h);
};

#endif
