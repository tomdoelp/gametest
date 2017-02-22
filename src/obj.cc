#include "obj.h"

/* Basic Object */
Obj::Obj() {
	active=true;
	id = objtotal++;
	alert("id %d", id);
}
void Obj::update() { }	
void Obj::activate() { active = true; }
void Obj::deactivate() { active = false; }
bool Obj::isactive() { return active; }
int Obj::objtotal = 0;


/* Physical Object */
PhysicalObj::PhysicalObj(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
PhysicalObj::~PhysicalObj() {}



/* Solid Object */
SolidObj::SolidObj(float x, float y, float w, float h) : PhysicalObj(x, y, w, h) {}
SolidObj::~SolidObj() {}



/* Visible Object */
VisibleObj::VisibleObj(float x, float y, float w, float h, int depth, Sprite *s) : PhysicalObj(x, y, w, h), depth(y+h), sprite(s) {
	aspeed = 2.0 / 60.0;
	loop = true;
	frame_index = 0;
	visible = true;
}

VisibleObj::~VisibleObj() {}
void VisibleObj::draw() {
	if (sprite){
		sprite->sprite_draw(x,y,frame_index);
		frame_index += aspeed;
		if (frame_index >= sprite->getframes() && !loop) {
			aspeed = 0;
			frame_index = sprite->getframes();
		}
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,0,125));	
}
bool VisibleObj::operator<(const VisibleObj &rhs) {
	return depth > rhs.depth; //reverse order, since we draw depth high to low
}



/* Wall */
Wall::Wall(float x, float y, float w, float h, int depth) : 
	PhysicalObj(x, y, w, h), 
	SolidObj(x, y, w, h),
	VisibleObj(x, y, w, h, depth) {}
	Wall::~Wall() {}
	void Wall::draw() {
		al_draw_filled_rectangle(x-w/2, y-h/2, x+w/2, y+h/2, al_map_rgb(125,125,125));	
	}



/* Mobile Object */
MobileObj::MobileObj(float x, float y, float w, float h, int depth, Sprite *s) : PhysicalObj(x, y, w, h), VisibleObj(x, y, w, h, depth, s) {
	dy = 0;
	dx = 0;
}
MobileObj::~MobileObj() {}
void MobileObj::update() {
	x += dx;
	y += dy;
	depth = y+h; 
}



/* Player Object */
Player::Player(float x, float y, float w, float h, int depth, Sprite *s) : PhysicalObj(x, y, w, h), MobileObj(x, y, w, h, depth, s) {
	score = 0;
}
Player::~Player() {}
void Player::update() {
	float xnext, ynext;
	/* vertical control */
	if (key[ALLEGRO_KEY_UP]) {
		dy = -2;
	} else if (key[ALLEGRO_KEY_DOWN]) {
		dy = 2;
	} else
		dy = 0;

	if (key[ALLEGRO_KEY_UP] && key[ALLEGRO_KEY_DOWN])
		dy = 0;

	/* horizontal control */
	if (key[ALLEGRO_KEY_LEFT]) {
		dx = -2;
	} else if (key[ALLEGRO_KEY_RIGHT]) {
		dx = 2;
	} else
		dx = 0;

	if (key[ALLEGRO_KEY_LEFT] && key[ALLEGRO_KEY_RIGHT])
		dx = 0;

	xnext = x+dx;
	ynext = y+dy;
	/* don't leave the screen */
	if (xnext <= (w/2) || xnext >= SCREEN_W - (w/2)) {
		dx = 0;
		/* TODO place object right at the edge */
	}

	if (ynext <= h/2 || ynext >= SCREEN_H - h/2) {
		dy = 0;
		/* TODO place object right at the edge */
	}

	/* handle collisions */


	/* update position based on speed */
	super::update();
}
void Player::draw() {
	if (sprite) 
		super::draw();
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}

bool Player::bb_collision(float x, float y, float w, float h){
	float _x = this->x - this->w/2;
	float _y = this->y - this->h/2;
	float _w = this->w;
	float _h = this->h;

	if ((_x > x+w - 1) ||
			(_y > y+h - 1) ||
			(x > _x+_w - 1) ||
			(y > _y+_h - 1)) 
	{
		return false;
	}

	return true;
}
