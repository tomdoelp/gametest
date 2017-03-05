#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h> // link with -lm ?

	#include <allegro5/allegro.h>
	#include <allegro5/allegro_primitives.h>

#include "global.h"

class Vec2f {
	public:
		Vec2f(float x=0.0f, float y=0.0f);
		~Vec2f();

		float get_x() const;
		float get_y() const;
		float get_diagonal() const;

		void set_x(float x);
		void set_y(float y);

		char *c_str();


		bool operator < (const Vec2f &rhs);
		bool operator == (float diag);
		bool operator != (float diag);

	protected:
		float x, y;

};

class Box {
	public:
		Box(float x=0, float y=0, float w=0, float h=0);
		~Box();
		float get_x() const;
		float get_y() const;
		float get_w() const;
		float get_h() const;

		Vec2f get_collision_vec(const Box &other);
		float get_collision_h(const Box &other);
		float get_collision_v(const Box &other);
		bool check_collision(Box &other);
		bool check_collision_horizontal(Box &other);
		bool check_collision_vertical(Box &other);

		void set_x(float x);
		void set_y(float y);
		void set_w(float w);
		void set_h(float h);

		void draw(ALLEGRO_COLOR color);
	
		Box operator + (const Vec2f &displace);
	protected:
		float x, y, w, h;
};

/* probably won't be used */
class BoxOrigin : public Box {
	public: 
		BoxOrigin(float x=0, float y=0, float w=0, float h=0, float ox=0, float oy=0);
		~BoxOrigin();
		float getox();
		float getoy();

		void setox(float ox);
		void setoy(float oy);
	
	private:
		float ox, oy;
};

float round_nearest(float n);

#endif
