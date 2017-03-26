#include "obj.h"
#include "world.h"

/* Basic Object */
Obj::Obj(){
	active=true;
	id = objtotal++;
	LOG("Object " << id << " created");
	world = NULL;
	persistent = false;
}
Obj::~Obj() { 
	LOG("Object " << id << " destroyed");
}
void Obj::update() {}	
void Obj::map_start() {}
void Obj::map_end() {}
bool Obj::destroy() {
	if (!world)
		return false;

	world->queue_destroy(this);
	return true;
}
/*void Obj::collide(Obj *other) {} */
void Obj::set_active(bool active) { this->active = active; }
bool Obj::is_active() const { return active; }
void Obj::set_persistent(bool persistent) { this->persistent = persistent; }
bool Obj::is_persistent() const { return persistent; }

int Obj::objtotal = 0;

int Obj::get_id() const { return id; }
Box Obj::get_bbox() const { return Box(0.0f, 0.0f, 0.0f, 0.0f); }
void Obj::attach_to_world(World *world) { 
	this->world = world; 
	world->register_object(this);
}
bool Obj::operator ==(const Obj &rhs) {
	return this->get_id() == rhs.get_id();
}
bool Obj::operator !=(const Obj &rhs) {
	return this->get_id() != rhs.get_id();
}



/* Physical Object */
PhysicalObj::PhysicalObj(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {
	LOG(get_bbox());
}
PhysicalObj::~PhysicalObj() {}
float PhysicalObj::get_x() const { return x; }
float PhysicalObj::get_y() const { return y; }
float PhysicalObj::get_w() const { return w; }
float PhysicalObj::get_h() const { return h; }
Box PhysicalObj::get_bbox() const { return Box(x,y,w,h); }

void PhysicalObj::set_position(float x, float y) {
	this->x = x;
	this->y = y;
}
void PhysicalObj::displace(float dx, float dy) {
	this->x += dx;
	this->y += dy;
}



/* Visible Object */
VisibleObj::VisibleObj(float x, float y, float w, float h, int depth, SpriteSheet *s) : PhysicalObj(x, y, w, h), depth(y), sheet(s) {
	if (sheet) {
		sprite = (*sheet)[0];
		sprite->sprite_center_origin(Sprite::ORIGIN_CENTER_BOTTOM); 
	}
	else 
		sprite = NULL;

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
bool VisibleObj::destroy() {
	if (!world)
		return false;

	world->queue_destroy_visible(this);
	return true;
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

/* Dummy Object */
Dummy::Dummy(float x, float y) : MobileObj(x, y, 12, 8, 0, SheetManager::get_sheet(SheetManager::SH_DUMMY)) {
	spr_shadow = (*SheetManager::get_sheet(SheetManager::SH_SHADOW))[0];
	spr_shadow->sprite_center_origin(Sprite::ORIGIN_CENTER_MIDDLE);
}
Dummy::~Dummy() {}

void Dummy::update() {
	if (get_bbox().check_collision(world->get_player()->get_bbox())) {
		destroy();
	}
}
Box Dummy::get_bbox() const { return Box(x-w/2, y-h, w, h); }
void Dummy::draw() {
		spr_shadow->sprite_draw(x,y+1,0.0f);
		super::draw();
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
	persistent = true;

	spr_shadow = (*SheetManager::get_sheet(SheetManager::SH_SHADOW))[0];
	spr_shadow->sprite_center_origin(Sprite::ORIGIN_CENTER_MIDDLE);

}
Player::~Player() {
	if (world) {
		world->set_player(NULL);
		world->set_view_focus(NULL);
	}
}

void Player::update() {
	/* vertical control */
	if (kmap(ALLEGRO_KEY_UP)) {
		dy = -1;
	} else if (kmap(ALLEGRO_KEY_DOWN)) {
		dy = 1;
	} else
		dy = 0;

	if (kmap(ALLEGRO_KEY_UP) && kmap(ALLEGRO_KEY_DOWN))
		dy = 0;

	/* horizontal control */
	if (kmap(ALLEGRO_KEY_LEFT)) {
		dx = -1;
	} else if (kmap(ALLEGRO_KEY_RIGHT)) {
		dx = 1;
	} else
		dx = 0;

	/* Not moving */
	if (kmap(ALLEGRO_KEY_LEFT) && kmap(ALLEGRO_KEY_RIGHT))
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
	/* (trust me it looks nicer) */
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

void Player::map_start(){
	world->set_view_focus(this);
	world->set_player(this);
	super::map_start();
}



void Player::draw() {
#if DEBUG_DRAW
	for (auto &b : world->get_map()->get_collision_box(get_bbox()+Vec2f(dx,dy)))
		b.draw(al_map_rgb(128,0,0));
#endif

	if (sprite) {
		if (aspeed == 0)
			frame_index = direction;

		spr_shadow->sprite_draw(x,y+1,0.0f);

		super::draw();
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}

Box Player::get_bbox() const { return Box(x-w/2, y-h+2, w, h); }
