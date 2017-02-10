#include "utils.h"

Box::Box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
Box::~Box() {}

float Box::getx() { return x; }
float Box::gety() { return y; }
float Box::getw() { return w; }
float Box::geth() { return h; }

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
