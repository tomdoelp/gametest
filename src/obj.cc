#include "obj.h"
#include "world.h"

/* Basic Object */
Obj::Obj(){
	active=true;
	id = objtotal++;
	alert("id %d", id);
	world = NULL;
}
Obj::~Obj() {}
void Obj::update() {}	
void Obj::map_start() {}
void Obj::map_end() {}
void Obj::set_active(bool active) { this->active = active; }
bool Obj::is_active() const { return active; }
void Obj::set_persistent(bool persistent) { this->persistent = persistent; }
bool Obj::is_persistent() const { return persistent; }

int Obj::objtotal = 0;

int Obj::get_id() const { return id; }
void Obj::attach_to_world(World *world) { 
	this->world = world; 
	world->register_object(this);
}
bool Obj::operator ==(const Obj &rhs) {
	return this->get_id() == rhs.get_id();
}



/* Physical Object */
PhysicalObj::PhysicalObj(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
PhysicalObj::~PhysicalObj() {}
float PhysicalObj::get_x() const { return x; }
float PhysicalObj::get_y() const { return y; }
float PhysicalObj::get_w() const { return w; }
float PhysicalObj::get_h() const { return h; }
Box PhysicalObj::get_bbox() const { return Box(x,y,w,h); }



/* Solid Object */
/*
SolidObj::SolidObj(float x, float y, float w, float h) : PhysicalObj(x, y, w, h) {}
SolidObj::~SolidObj() {}
*/


/* Visible Object */
VisibleObj::VisibleObj(float x, float y, float w, float h, int depth, SpriteSheet *s) : PhysicalObj(x, y, w, h), depth(y), sheet(s) {
	if (sheet) {
		sprite = (*sheet)[0];
		sprite->sprite_center_origin(Sprite::ORIGIN_CENTER_BOTTOM); 
	}

	aspeed = 2.0 / 60.0;
	loop = true;
	frame_index = 0;
	visible = true;
	hflip = false;
	vflip = false;
}

VisibleObj::~VisibleObj() {}
void VisibleObj::draw() {
#if DEBUG_DRAW
	get_bbox().draw(al_map_rgb(0,255,0));
#endif

	if (sprite){
		int flags = 0;
		if (hflip)
			flags = flags | ALLEGRO_FLIP_HORIZONTAL;
		if (vflip)
			flags = flags | ALLEGRO_FLIP_VERTICAL;

		sprite->sprite_draw(x,y,frame_index, flags);
		if (!paused)
			frame_index += aspeed;
		if (frame_index >= sprite->getframes() && !loop) {
			aspeed = 0;
			frame_index = sprite->getframes();
		}
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,0,125));	

#if DEBUG_DRAW
	al_draw_filled_rectangle(x-1,y-1,x+1,y+1,al_map_rgb(0,0,0));
#endif
}
bool VisibleObj::operator<(const VisibleObj &rhs) {
	return depth > rhs.depth; //reverse order, since we draw depth high to low
}



/* Mobile Object */
MobileObj::MobileObj(float x, float y, float w, float h, int depth, SpriteSheet *s) : VisibleObj(x, y, w, h, depth, s) {
	dy = 0;
	dx = 0;
	direction = DIR_S;
}
MobileObj::~MobileObj() {}
void MobileObj::update() {
	x += dx;
	y += dy;
	depth = y; 
}



/* Player Object */
Player::Player(float x, float y) : MobileObj(x, y, 16, 8, 0, SheetManager::get_sheet(SheetManager::SH_DEATH)) {
	score = 0;
	aspeed = 0;
	spritenum = 6;

	for (int i = 0; i < spritenum; i++) {
		sprites[i] = (*sheet)[i];
		sprites[i]->sprite_center_origin(Sprite::ORIGIN_CENTER_BOTTOM);
	}
	sprite = sprites[0];

}
Player::Player(World *world, float x, float y) : MobileObj(x, y, 16, 8, 0, SheetManager::get_sheet(SheetManager::SH_DEATH)) {
	attach_to_world(world);

	world->set_view_focus(this);
	score = 0;
	aspeed = 0;
	spritenum = 6;

	for (int i = 0; i < spritenum; i++) {
		sprites[i] = (*sheet)[i];
		sprites[i]->sprite_center_origin(Sprite::ORIGIN_CENTER_BOTTOM);
	}
	sprite = sprites[0];
}
Player::~Player() {}
void Player::update() {
	/* vertical control */
	if (key[ALLEGRO_KEY_UP]) {
		dy = -1;
	} else if (key[ALLEGRO_KEY_DOWN]) {
		dy = 1;
	} else
		dy = 0;

	if (key[ALLEGRO_KEY_UP] && key[ALLEGRO_KEY_DOWN])
		dy = 0;

	/* horizontal control */
	if (key[ALLEGRO_KEY_LEFT]) {
		dx = -1;
	} else if (key[ALLEGRO_KEY_RIGHT]) {
		dx = 1;
	} else
		dx = 0;

	/* Not moving */
	if (key[ALLEGRO_KEY_LEFT] && key[ALLEGRO_KEY_RIGHT])
		dx = 0;

	/* which way ya facing */
	if (dy > 0 && dx == 0)
		direction = DIR_S;
	if (dy > 0 && dx > 0)
		direction = DIR_SE;
	if (dy > 0 && dx < 0)
		direction = DIR_SW;
	if (dy < 0 && dx == 0)
		direction = DIR_N;
	if (dy < 0 && dx < 0)
		direction = DIR_NW;
	if (dy < 0 && dx > 0)
		direction = DIR_NE;
	if (dy == 0 && dx > 0)
		direction = DIR_E;
	if (dy == 0 && dx < 0)
		direction = DIR_W;

	Sprite *temp = sprite;
	switch (direction) {
		case DIR_S:
			sprite = sprites[SPR_WALK_DOWN];
			hflip = false;
			break;
		case DIR_SE:
			sprite = sprites[SPR_WALK_DOWN_RIGHT];
			hflip = false;
			break;
		case DIR_E:
			sprite = sprites[SPR_WALK_RIGHT];
			hflip = false;
			break;
		case DIR_NE:
			sprite = sprites[SPR_WALK_UP_RIGHT];
			hflip = false;
			break;
		case DIR_N:
			sprite = sprites[SPR_WALK_UP];
			hflip = false;
			break;
		case DIR_SW:
			sprite = sprites[SPR_WALK_DOWN_RIGHT];
			hflip = true;
			break;
		case DIR_W:
			sprite = sprites[SPR_WALK_RIGHT];
			hflip = true;
			break;
		case DIR_NW:
			sprite = sprites[SPR_WALK_UP_RIGHT];
			hflip = true;
			break;
	}
	/* always start a walkcycle from the same place */
	if (sprite != temp){
		frame_index = 2;
	}

	/* stand tall and proud if not moving */
	if (dy ==0 && dx == 0) {
		sprite = sprites[SPR_STAND];
		aspeed = 0;
		hflip = false;
	}
	else
		aspeed = 6.0f/60.0f;

	/* Tile collision handling */
	if (world && (dx != 0 || dy != 0)) {
		Vec2f intersection = world->get_map()->get_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy));
		dx += intersection.get_x();
		dy += intersection.get_y();
	}

	/* update position based on speed */
	super::update();
}
void Player::draw() {
#if DEBUG_DRAW
	for (auto &b : world->get_map()->get_collision_box(get_bbox()+Vec2f(dx,dy)))
		b.draw(al_map_rgb(255,0,0));
#endif

	if (sprite) {
		if (aspeed == 0)
			frame_index = direction;
		super::draw();
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}

Box Player::get_bbox() const { return Box(x-w/2, y-h+2, w, h); }
