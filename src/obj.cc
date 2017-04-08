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
	while (!tweens.empty()) {
		delete tweens.front();
		tweens.pop();
		LOG("Object " << id << " destroyed active tween");
	}

	LOG("Object " << id << " destroyed");
}
void Obj::update() {
	if (tweens.size() > 0) {
		std::queue<GenericTween*> tempqueue;
		while (!tweens.empty()) {
			if (tweens.front()->step()) {
				tempqueue.push(tweens.front());
				tweens.pop();
			}
			else {
				delete tweens.front();
				tweens.pop();
				LOG("tween finished and destroyed");
			}
		}
		swap(tweens, tempqueue);
	}
}	
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
bool Obj::is_solid() const { return solid; }
void Obj::interact() {}

ObjType Obj::get_type() const { return OBJ; }

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
PhysicalObj::PhysicalObj(float x, float y, float w, float h) : x(x), y(y), z(0.0f), w(w), h(h) {
	LOG(get_bbox());
}
PhysicalObj::~PhysicalObj() {}
float PhysicalObj::get_x() const { return x; }
float PhysicalObj::get_y() const { return y; }
float PhysicalObj::get_z() const { return z; }
float PhysicalObj::get_w() const { return w; }
float PhysicalObj::get_h() const { return h; }
Box PhysicalObj::get_bbox() const { return Box(x,y,w,h); }
ObjType PhysicalObj::get_type() const { return OBJ_PHYSICAL; }

void PhysicalObj::set_position(float x, float y) {
	this->x = x;
	this->y = y;
}
void PhysicalObj::displace(float dx, float dy) {
	this->x += dx;
	this->y += dy;
}
void PhysicalObj::displace(Vec2f disp) {
	this->x += disp.get_x();
	this->y += disp.get_y();
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
	alpha = 1.0f;
}

VisibleObj::~VisibleObj() {}
ObjType VisibleObj::get_type() const { return OBJ_VISIBLE; }

void VisibleObj::set_sprite(SpriteSheet *s, int index) {
	sheet = s;
	if (sheet) {
		sprite = (*sheet)[index];
		sprite->sprite_center_origin(Sprite::ORIGIN_CENTER_BOTTOM);
	}
	else
		sprite = NULL;
}

void VisibleObj::set_sprite(Sprite *sprite) { this->sprite = sprite; }

void VisibleObj::draw() {

	if (sprite){
		int flags = 0;
		if (hflip)
			flags = flags | ALLEGRO_FLIP_HORIZONTAL;
		if (vflip)
			flags = flags | ALLEGRO_FLIP_VERTICAL;

		sprite->sprite_draw(x,y-z,frame_index, flags, al_map_rgba_f(1.0f * alpha, 1.0f * alpha, 1.0f * alpha, alpha));
		if (world && world->get_mode() == World::MODE_OVERWORLD)
			frame_index += aspeed;
		if (frame_index >= sprite->getframes() && !loop) {
			aspeed = 0;
			frame_index = sprite->getframes();
		}
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,0,125));	

#if DEBUG_DRAW
	get_bbox().draw(al_map_rgb(0,255,0));
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

	super::update();
}
float MobileObj::get_dx() const { return dx; }
float MobileObj::get_dy() const { return dy; }
ObjType MobileObj::get_type() const { return OBJ_MOBILE; }
void MobileObj::face_point(float ox, float oy) {
	float slope = ((oy-y)/(ox-x));
	bool right = ox > x;

	if ((slope < 0.57f && slope >= 0) ||
		(slope > -0.57f && slope < 0)) {
		if (right)
			direction = DIR_E;
		else
			direction = DIR_W;
	}

	if (slope > 0.57f && slope < 1.73f) {
			if (right)
				direction = DIR_SE;
			else
				direction = DIR_NW;
	}

	if (slope > 1.73f) {
		if (right)
			direction = DIR_S;
		else
			direction = DIR_N;
	}

	if (slope < -1.73f) {
		if (right)
			direction = DIR_N;
		else
			direction = DIR_S;
	}
	if (slope == (1.0f / 0.0f)) {
		if (y < oy)
			direction = DIR_S;
		else
			direction = DIR_N;
	}

	if (slope < -0.57f && slope > -1.73f) {
		if (right)
			direction = DIR_NE;
		else
			direction = DIR_SW;
	}
}


/* Prop object */
Prop::Prop(float x, float y, PropType t) : VisibleObj(x,y,0.0f,0.0f) {
	solid = true;
	switch (t) {
		case PROP_CANDELABRUM:
			set_sprite(SheetManager::get_sheet(SheetManager::SH_CASTLE_PROPS), 0);
			w = 12;
			h = 8;
			aspeed = 0.1;
			break;
		case PROP_CANDELABRUM_LIT:
			set_sprite(SheetManager::get_sheet(SheetManager::SH_CASTLE_PROPS), 1);
			w = 12;
			h = 8;
			aspeed = 0.1;
			break;

		default:
			sheet=NULL;
			sprite=NULL;
			break;
	}
}
void Prop::update(){
	super::update();
}
ObjType Prop::get_type() const { return OBJ_PROP; }
void Prop::interact() {
	if (world && mymsg) {
		Player *p = world->get_player();
		p->face_point(x,y);
		world->show_text(mymsg);
	}
}
Box Prop::get_bbox() const { return Box(x-w/2, y-h, w, h); }



/* Dummy Object */
Dummy::Dummy(float x, float y) : MobileObj(x, y, 12, 8, 0, SheetManager::get_sheet(SheetManager::SH_DUMMY)) {
	spr_shadow = (*SheetManager::get_sheet(SheetManager::SH_SHADOW))[0];
	spr_shadow->sprite_center_origin(Sprite::ORIGIN_CENTER_MIDDLE);
	aspeed = 0;
	alpha = 1.0f;
	solid = true;

	tweens.push(new Tween<float>(&z, 16.0f, 120, 0.01f));
/*	tweens.push(new Tween<float>(&y, y+10.0f, 20, 0.01f)); */
	// TODO simultaneous tweens
}
Dummy::~Dummy() {}

void Dummy::update() {
	/*
	Player *p = world->get_player();
	if (get_bbox().check_collision(p->get_bbox())) {
		world->sndmgr->play_sound(SoundManager::SND_ACCEPT); 
		destroy(); 
		
	}
	*/

	super::update();
}

Box Dummy::get_bbox() const { return Box(x-w/2, y-h, w, h); }
void Dummy::draw() {
	if (sprite) {
		if (aspeed == 0)
			frame_index = direction;

		spr_shadow->sprite_draw(x,y+1,0.0f);

		super::draw();
	}
	else
		al_draw_filled_ellipse(x, y, w/2, h/2, al_map_rgb(0,255,0));	
}
ObjType Dummy::get_type() const { return OBJ_DUMMY; }
void Dummy::interact() {
	if (world) {
		Player *p = world->get_player();
		face_point(p->get_x(), p->get_y());
		p->face_point(x,y);
		world->show_text(mymsg);
	}
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
ObjType Player::get_type() const { return OBJ_PLAYER; }
Player::~Player() {
	if (world) {
		world->set_player(NULL);
		world->set_view_focus(NULL);
	}
}

void Player::update() {
	/* interact with x */
	if (key_press[ALLEGRO_KEY_X] && world) {
		Box bbox = get_bbox();
		switch(direction) {
			case DIR_S:
			case DIR_SE:
			case DIR_SW:
				world->interact_with_object(
						bbox+Vec2f(0.0f,bbox.get_h()));
				key_press[ALLEGRO_KEY_X] = false;
				break;
			case DIR_E:
				world->interact_with_object(
						bbox+Vec2f(bbox.get_w(),0.0f));
				key_press[ALLEGRO_KEY_X] = false;
				break;
			case DIR_NW:
			case DIR_NE:
			case DIR_N:
				world->interact_with_object(
						bbox+Vec2f(0.0f,-bbox.get_h()));
				key_press[ALLEGRO_KEY_X] = false;
				break;
			case DIR_W:
				world->interact_with_object(
						bbox+Vec2f(-bbox.get_w(),0.0f));
				key_press[ALLEGRO_KEY_X] = false;
				break;
			default:
				break;
		}
	}

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

		if (intersection.get_x() == 0)
			intersection.set_x(world->get_object_collision_solid(get_bbox(), get_bbox()+Vec2f(dx,dy)).get_x());
		if (intersection.get_y() == 0)
			intersection.set_y(world->get_object_collision_solid(get_bbox(), get_bbox()+Vec2f(dx,dy)).get_y());
		
		/*
		if (intersection.get_x() == 0)
			intersection.set_x(world->get_object_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy), OBJ_PROP).get_x());
		if (intersection.get_y() == 0)
			intersection.set_y(world->get_object_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy), OBJ_PROP).get_y());
		if (intersection.get_x() == 0)
			intersection.set_x(world->get_object_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy), OBJ_DUMMY).get_x());
		if (intersection.get_y() == 0)
			intersection.set_y(world->get_object_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy), OBJ_DUMMY).get_y());
			*/



		/*
		   if (intersection.get_x() == 0 && intersection.get_y() == 0) {
		   intersection = world->get_object_collision_vec(get_bbox(), get_bbox()+Vec2f(dx,dy), OBJ_PROP);
		   }
		   */
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
	/*	for (auto &b : world->get_map()->get_collision_box(get_bbox()+Vec2f(dx,dy))) */
	/*		b.draw(al_map_rgb(128,0,0)); */
#endif

	if (sprite) {
		if (aspeed == 0)
			frame_index = direction;

		spr_shadow->sprite_draw(x,y+1,0.0f);

		super::draw(); 

		/*		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE); */
		/*		float a = 0.1f; */
		/*		sprite->sprite_draw(x,y,frame_index, flags, al_map_rgba_f(a,a,a,a)); */
		/*		sprite->sprite_draw(x+1,y,frame_index, flags, al_map_rgba_f(a,a,a,a)); */
		/*		sprite->sprite_draw(x-1,y,frame_index, flags, al_map_rgba_f(a,a,a,a)); */
		/*		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); */
	}
}

Box Player::get_bbox() const { return Box(x-w/2, y-h+2, w, h); }
