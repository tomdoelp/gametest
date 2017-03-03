#include "utils.h"


Box::Box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {
	halfw = w/2;
	halfh = h/2;
	hypotenuse = sqrt(w*w + h*h);

}
Box::~Box() {}

float Box::get_x() const { return x; }
float Box::get_y() const { return y; }
float Box::get_w() const { return w; }
float Box::get_h() const { return h; }
float Box::get_center_x() const { return x + halfw; }
float Box::get_center_y() const { return y + halfh; }
float Box::get_halfw() const { return halfw; }
float Box::get_halfh() const { return halfw; }
float Box::get_hypotenuse() const { return hypotenuse; }

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

float Box::check_collision_horizontal(Box &other) {
	float x2 = other.get_x();
	float w2 = other.get_w();
	if ((x > x2+w2-1) ||
		(x2 > x+w-1)) {
		return 0.0;
	}
	float left = x2 - x;
	float right = x2+w2 - x;
	return (-left < right) ? left : right;
}
float Box::check_collision_horizontal(float ox, float ow) {
	if ((x > ox+ow-1) ||
		(ox > x+w-1)) {
		return 0.0;
	}
	float left = ox - x;
	float right = ox+ow - x;
	return (-left < right) ? left : right;
}
float Box::check_collision_vertical(Box &other) {
	float y2 = other.get_y();
	float h2 = other.get_h();
	if ((y > y2+h2-1) ||
		(y2 > y+h-1)) {
		return 0.0;
	}
	float up = y2 - y;
	float down = y2+h2 - w;

	return (-up < down) ? up : down;
}
float Box::check_collision_vertical(float oy, float oh) {
	if ((y > oy+oh-1) ||
		(oy > y+h-1)) {
		return 0.0;
	}
	float up = oy - y;
	float down = oy+oh - w;

	return (-up < down) ? up : down;
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

