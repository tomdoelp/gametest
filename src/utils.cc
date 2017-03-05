#include "utils.h"

Vec2f::Vec2f(float x, float y) : x(x), y(y) {}
Vec2f::~Vec2f() {}
float Vec2f::get_x() const { return x; }
float Vec2f::get_y() const { return y; }
float Vec2f::get_diagonal() const { return sqrt(x*x+y*y); }

void Vec2f::set_x(float x) { this->x = x; }
void Vec2f::set_y(float y) { this->y = y; }

bool Vec2f::operator < (const Vec2f &rhs) {
	return (get_diagonal() < rhs.get_diagonal());
}
bool Vec2f::operator == (float diag) {
	return (get_diagonal() == diag);
}
bool Vec2f::operator != (float diag) {
	return (get_diagonal() != diag);
}

Box::Box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
Box::~Box() {}

float Box::get_x() const { return x; }
float Box::get_y() const { return y; }
float Box::get_w() const { return w; }
float Box::get_h() const { return h; }

Box Box::operator + (const Vec2f &displace) {
	return Box(x+displace.get_x(), y+displace.get_y(), w, h);
}

void Box::draw(ALLEGRO_COLOR color){
	al_draw_filled_rectangle(x, y, x+w, y+h, color);
}

Vec2f Box::get_collision_vec(const Box &other) {
	float ox = other.get_x();
	float oy = other.get_y();
	float ow = other.get_w();
	float oh = other.get_h();

	float vx, vy;

	if ((x > ox+ow-1) ||
		(y > oy+oh-1) ||
		(ox > x+w-1)  ||
		(oy > y+h-1)) {
		vx = vy = 0.0f;
	}
	else {
		/*
 		float left = ox-x;
		float right = ox+ow - x;
		float up = oy - y;
		float down = oy+oh - h;
		vx = (-left < right) ? left : right;
		vy = (-up < down) ? up : down;
		*/

		vx = (x < ox) ? (ox-(x+w)) : ((ox+ow)-x);
		vy = (y < oy) ? (oy-(y+h)) : ((oy+oh)-y);
	}

	return Vec2f(vx, vy);
}

float Box::get_collision_h(const Box &other) {
	float ox = other.get_x();
	float ow = other.get_w();

	if ((x > ox+ow-1) || (ox > x+w-1)) {
		return 0.0f;
	}

	float left = ox-x;
	float right = ox+ow-x;
	return (-left < right) ? left : right;
}
float Box::get_collision_v(const Box &other) {
	float oy = other.get_y();
	float oh = other.get_h();

	if ((y > oy+oh-1) || (oy > y+h-1)) {
		return 0.0f;
	}

	return (y < oy) ? (oy-(y+h)) : ((oy+oh)-y);
}

/* from https://wiki.allegro.cc/index.php?title=Bounding_Box */
bool Box::check_collision(Box &other) {
	float x2 = other.get_x();
	float y2 = other.get_y();
	float w2 = other.get_w();
	float h2 = other.get_h();
	if ((x > x2+w2-1) ||
		(y > y2+h2-1) ||
		(x2 > x+w-1)  ||
		(y2 > y+h-1)) {
		return false;
	}

	return true;
}

bool Box::check_collision_horizontal(Box &other) {
	float x2 = other.get_x();
	float w2 = other.get_w();
	if ((x > x2+w2-1) ||
		(x2 > x+w-1)) {
		return false;
	}
	return true;
}
bool Box::check_collision_vertical(Box &other) {
	float y2 = other.get_y();
	float h2 = other.get_h();
	if ((y > y2+h2-1) ||
		(y2 > y+h-1)) {
		return false;
	}

	return true;
}


void Box::set_x(float x) { this->x = x; }
void Box::set_y(float y) { this->y = y; }
void Box::set_w(float w) { this->w = w; }
void Box::set_h(float h) { this->h = h; }


BoxOrigin::BoxOrigin(float x, float y, float w, float h, float ox, float oy) : Box(x,y,w,h), ox(ox), oy(oy) {}
BoxOrigin::~BoxOrigin() {}

float BoxOrigin::getox() { return this->ox; }
float BoxOrigin::getoy() { return this->oy; }

void BoxOrigin::setox(float ox) { this->ox = ox; }
void BoxOrigin::setoy(float oy) { this->oy = oy; }

float round_nearest(float n) {
	return (n > 0.0) ? floor(n + 0.5) : ceil(n - 0.5);
}
