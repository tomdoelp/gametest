#include "../inc/Obj.h"
#include "../inc/global.h"

/* Basic Object */
Obj::Obj() {
	id = objtotal++;
}
void Obj::update() { }	
int Obj::objtotal = 0;


/* Visible Object */
VisibleObj::VisibleObj(float x, float y, int w, int h, int depth) : x(x), y(y), w(w), h(h), depth(depth) { }
void VisibleObj::draw() {
	al_draw_filled_rectangle(x-w/2, y-h/2, x+w/2, y+h/2, al_map_rgb(125,125,125));	
}
bool VisibleObj::operator<(const VisibleObj &r) {
	return this->depth < r.depth;
}

/* Mobile Object */
MobileObj::MobileObj(float x, float y, int w, int h, int depth) : VisibleObj(x, y, w, h, depth) {
	vspeed = 0;
	hspeed = 0;
}
void MobileObj::update() {
		x += hspeed;
		y += vspeed;
		//super::update(); /* does nothing */
}


/* Player Object */
Player::Player(float x, float y, int w, int h, int depth) : MobileObj(x, y, w, h, depth) {
	score = 0;
}
void Player::update() {
	/* vertical control */
	if (key[KEY_UP]) {
		vspeed = -5;
	} else if (key[KEY_DOWN]) {
		vspeed = 5;
	} else
		vspeed = 0;

	if (key[KEY_UP] && key[KEY_DOWN])
		vspeed = 0;

	/* horizontal control */
	if (key[KEY_LEFT]) {
		hspeed = -5;
	} else if (key[KEY_RIGHT]) {
		hspeed = 5;
	} else
		hspeed = 0;

	if (key[KEY_LEFT] && key[KEY_RIGHT])
		hspeed = 0;

	/* don't leave the screen */
	if (x+hspeed <= (w/2) || x+hspeed >= SCREEN_W - (w/2))
		hspeed = 0;

	if (y+vspeed <= h/2 || y+vspeed >= SCREEN_H - h/2)
		vspeed = 0;

	/* handle collisions */

	/* update position based on speed */
	super::update();
}
void Player::draw() {
	al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}
