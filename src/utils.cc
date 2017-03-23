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
Vec2f& Vec2f::operator = (const Vec2f& rhs) {
	x = rhs.get_x();
	y = rhs.get_y();
	return *this;
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
 
std::ostream& operator << (std::ostream& out, const Box &b) {
	return out << b.get_x() << ", " << b.get_y() << "; " << b.get_w() << "x" << b.get_h();
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
		vx = (x < ox) ? (ox-(x+w)) : ((ox+ow)-x);
		vy = (y < oy) ? (oy-(y+h)) : ((oy+oh)-y);
	}

	return Vec2f(vx, vy);
}

Vec2f Box::get_collision_vec(const Box_Diag &other) {
	Vec2f result(0.0f, 0.0f);
	Vec2f v = other.get_velocity();


	switch (other.get_diag()) {
		case Box_Diag::DIAG_NONE:
			result =  get_collision_vec((Box) other); /* hopefully? */
			break; 
		case Box_Diag::DIAG_UPRIGHT:
			if (v.get_x() > 0)
				result.set_y((other.get_y()+other.get_h() - y+h) - ((x+w)-other.get_x()));
			if (v.get_x() < 0)
				result.set_y(0.0f);
			if (v.get_y() > 0)
				result.set_x(0.0f);
			if (v.get_y() < 0)
				result.set_x(0.0f);
			break;

		case Box_Diag::DIAG_UPLEFT:

			break;

		default:
			break;
	}

	return result;
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
bool Box::check_collision(const Box &other) {
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


Box_Diag::Box_Diag(float x, float y, float w, float h, DiagType d, Vec2f velocity) : Box(x,y,w,h), diag(d), velocity(velocity) {}
Box_Diag::DiagType Box_Diag::get_diag() const { return this->diag; }
Vec2f Box_Diag::get_velocity() const { return velocity; }
void Box_Diag::set_velocity(Vec2f velocity) { this->velocity = velocity; }

bool Box_Diag::isUp(const Box &other) const {
	float ax=0.0f, ay=0.0f, bx=0.0f, by=0.0f, cx=0.0f, cy=0.0f;
	if (diag == Box_Diag::DIAG_UPRIGHT) {
		ax = x;
		ay = y+h;
		bx = x+w;
		by = y;
		cx = other.get_x() + other.get_w();
		cy = other.get_y();

	}
	if (diag == Box_Diag::DIAG_UPLEFT) {
		ax = x;
		ay = y;
		bx = x+w;
		by = y+h;
		cx = other.get_x();
		cy = other.get_y();
	}

	return ((bx - ax)*(cy - ay) - (by - ay)*(cx - ax)) > 0;
}




BoxOrigin::BoxOrigin(float x, float y, float w, float h, float ox, float oy) : Box(x,y,w,h), ox(ox), oy(oy) {}
BoxOrigin::~BoxOrigin() {}

float BoxOrigin::getox() { return this->ox; }
float BoxOrigin::getoy() { return this->oy; }

void BoxOrigin::setox(float ox) { this->ox = ox; }
void BoxOrigin::setoy(float oy) { this->oy = oy; }

float round_nearest(float n) {
	return (n > 0.0) ? floor(n + 0.5) : ceil(n - 0.5);
}
