#include "utils.h"


Box::Box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
Box::~Box() {}

float Box::getx() { return x; }
float Box::gety() { return y; }
float Box::getw() { return w; }
float Box::geth() { return h; }

/* from https://wiki.allegro.cc/index.php?title=Bounding_Box */
bool Box::check_collision(Box &other) {
	float x2 = other.getx();
	float y2 = other.gety();
	float w2 = other.getw();
	float h2 = other.geth();
	if ((x > x2+w2-1) ||
		(y > y2+h2-1) ||
		(x2 > x+w-1)  ||
		(y2 > y+h-1)) {
		return false;
	}

	return true;
}

bool Box::check_collision_horizontal(Box &other) {
	float x2 = other.getx();
	float w2 = other.getw();
	if ((x > x2+w2-1) ||
		(x2 > x+w-1)) {
		return false;
	}
	return true;
}
bool Box::check_collision_vertical(Box &other) {
	float y2 = other.gety();
	float h2 = other.geth();
	if ((y > y2+h2-1) ||
		(y2 > y+h-1)) {
		return false;
	}

	return true;
}


void Box::setx(float x) { this->x = x; }
void Box::sety(float y) { this->y = y; }
void Box::setw(float w) { this->w = w; }
void Box::seth(float h) { this->h = h; }


BoxOrigin::BoxOrigin(float x, float y, float w, float h, float ox, float oy) : Box(x,y,w,h), ox(ox), oy(oy) {}
BoxOrigin::~BoxOrigin() {}

float BoxOrigin::getox() { return this->ox; }
float BoxOrigin::getoy() { return this->oy; }

void BoxOrigin::setox(float ox) { this->ox = ox; }
void BoxOrigin::setoy(float oy) { this->oy = oy; }

float round_nearest(float n) {
	return (n > 0.0) ? floor(n + 0.5) : ceil(n - 0.5);
}
