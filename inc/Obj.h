#ifndef __OBJ_H__
#define __OBJ_H__

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "../inc/global.h"

/* Basic object. Holds the total number of instances, an id, and can update (nop) */
class Obj {
	public:
		Obj();
		void update();
	protected:
		int id;
		static int objtotal;
};


/* Visible object. Has a position, size, and depth. Can draw itself. */
/* Compared by depth, so they can be drawn in order. */
class VisibleObj : public Obj {
	public:
		VisibleObj(float x, float y, int w, int h, int depth = 0);
		void draw();
		bool operator<(const VisibleObj &r);
	protected:
		typedef Obj super;
		float x;
		float y;
		int w;
		int h;
		int depth;
		bool visible;
};


/* Mobile object. Has horizontal and vertical speed and moves at those speeds when updated. */
class MobileObj : public VisibleObj {
	public:
		MobileObj(float x, float y, int w, int h, int depth = 0);
		void update();
	protected:
		typedef VisibleObj super;
		float hspeed;
		float vspeed;
};


/* Player object. Has a score. Horizontal and vertical motion controlled with arrow keys. */
class Player : public MobileObj {
	public:
	   Player(float x, float y, int w, int h, int depth = 0);
	   void update();
	   void draw();
	protected:
	   typedef MobileObj super;
	   int score;
};

#endif
