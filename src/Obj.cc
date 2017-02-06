#include "../inc/Obj.h"
#include "../inc/global.h"

/* Basic Object */
Obj::Obj() { }
void Obj::update() { }	


/* Visible Object */
VisibleObj::VisibleObj(float x, float y, int w, int h) : x(x), y(y), w(w), h(h) { }
void VisibleObj::draw() {
	al_draw_text(font, al_map_rgb(255,255,255), x, y, ALLEGRO_ALIGN_CENTRE, "O");
}


/* Mobile Object */
MobileObj::MobileObj(float x, float y, int w, int h) : VisibleObj(x, y, w, h) {
	vspeed = 0;
	hspeed = 0;
}
void MobileObj::update() {
		x += hspeed;
		y += vspeed;
		//super::update(); /* does nothing */
}


/* Player Object */
Player::Player(float x, float y, int w, int h) : MobileObj(x, y, w, h) {
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
	if (x + hspeed <= (w/2) || x + hspeed >= SCREEN_W - (w/2))
		hspeed = 0;

	if (y + vspeed <= 0 || y + vspeed >= SCREEN_H - h)
		vspeed = 0;

	/* handle collisions */

	super::update();
}
void Player::draw() {
	al_draw_text(font, al_map_rgb(255,255,255), x, y, ALLEGRO_ALIGN_CENTRE, "@");
}
