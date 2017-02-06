#ifndef __OBJ_H__
#define __OBJ_H__

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../inc/global.h"

class Obj {
	public:
		Obj();
		void update();
	protected:
};

class VisibleObj : public Obj {
	public:
		VisibleObj(float x, float y, int w, int h);
		void draw();
	protected:
		typedef Obj super;
		float x;
		float y;
		int w;
		int h;
};

class MobileObj : public VisibleObj {
	public:
		MobileObj(float x, float y, int w, int h);
		void update();
	protected:
		typedef VisibleObj super;
		float hspeed;
		float vspeed;
};

class Player : public MobileObj {
	public:
	   Player(float x, float y, int w, int h);
	   void update();
	   void draw();
	protected:
	   typedef MobileObj super;
	   int score;
};

#endif
