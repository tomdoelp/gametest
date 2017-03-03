#include "obj.h"
#include "world.h"

/* Basic Object */
Obj::Obj() {
	active=true;
	id = objtotal++;
	alert("id %d", id);
	world = NULL;
}
Obj::~Obj() {}
void Obj::update() { }	
void Obj::activate() { active = true; }
void Obj::deactivate() { active = false; }
bool Obj::isactive() { return active; }
int Obj::objtotal = 0;
int Obj::get_id() const { return id; }
void Obj::attach_to_world(World *w) { this->world = w; }


/* Physical Object */
PhysicalObj::PhysicalObj(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
PhysicalObj::~PhysicalObj() {}
void PhysicalObj::collide(PhysicalObj* other) {
	alert("%d collided with %d", id, other->id);
}
Box PhysicalObj::get_bbox() const { return Box(x,y,w,h); }
bool PhysicalObj::is_moving() const { return moving; }
void PhysicalObj::update() {
	if (world) 
		world->checkin(this, this->get_bbox());
}



/* Solid Object */
SolidObj::SolidObj(float x, float y, float w, float h) : PhysicalObj(x, y, w, h) {}
SolidObj::~SolidObj() {}



/* Visible Object */
VisibleObj::VisibleObj(float x, float y, float w, float h, int depth, Sprite *s) : PhysicalObj(x, y, w, h), depth(y), sprite(s) {
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
	al_draw_filled_rectangle(x-1,y-1,x+1,y+1,al_map_rgb(0,0,0));
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
	depth = y; 
}
void MobileObj::collide_horizontal(float movex) {
	x += movex;
	dx = 0;
}
void MobileObj::collide_vertical(float movey) {
	y += movey;
	dy = 0;
}



/* Player Object */
Player::Player(float x, float y, float w, float h, int depth, Sprite *s, Map *m) : PhysicalObj(x, y, w, h), MobileObj(x, y, w, h, depth, s), m(m) {
	score = 0;
}
Player::~Player() {}
void Player::update() {
/*	float xnext, ynext; */
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

	/*xnext = x+dx;
	ynext = y+dy;*/
	/* don't leave the screen */
	/*if (xnext <= (w/2) || xnext >= SCREEN_W - (w/2)) {
		dx = 0;
	}

	if (ynext <= h/2 || ynext >= SCREEN_H - h/2) {
		dy = 0;
	}*/

	/* handle collisions */
	/*std::vector<Box> coltiles = m->get_collision_box(xnext-w/2, ynext-h/4, w, h/2);

	for (auto &b : coltiles) {
		float intersectx = b.check_collision_horizontal(xnext,w);
		float intersecty = b.check_collision_vertical(ynext,h);
		if (intersectx != 0.0 && b.check_collision_vertical(y,h) != 0.0) {
			x += intersectx;
			dx = 0;
		}
		if (intersecty != 0.0 && b.check_collision_horizontal(x,w) != 0.0) {
			y += intersecty;
			dy = 0;
		}
	}*/

	/* update position based on speed */
	super::update();
}
void Player::draw() {
	if (sprite) 
		super::draw();
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}

Box Player::get_bbox() const { return Box(x-w/2, y-w/2, w, h); }

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

void Player::collide(VisibleObj *other) {
	alert("zoom!");
}

