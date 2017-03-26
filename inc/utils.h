#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
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
		Vec2f& operator = (const Vec2f& rhs);

	protected:
		float x, y;

};

class Box_Diag;
class Box {
	public:
		Box(float x=0, float y=0, float w=0, float h=0);
		~Box();
		float get_x() const;
		float get_y() const;
		float get_w() const;
		float get_h() const;

		Vec2f get_collision_vec(const Box &other);
		Vec2f get_collision_vec(const Box_Diag &other);
		float get_collision_h(const Box &other);
		float get_collision_v(const Box &other);
		bool check_collision(const Box &other);
		bool check_collision_horizontal(Box &other);
		bool check_collision_vertical(Box &other);

		void set_x(float x);
		void set_y(float y);
		void set_w(float w);
		void set_h(float h);

		void draw(ALLEGRO_COLOR color);
	
		Box operator + (const Vec2f &displace);

		friend std::ostream& operator << (std::ostream& out, const Box &b);
	protected:
		float x, y, w, h;
};

class Box_Diag : public Box {
	public:
		typedef enum diags { DIAG_NONE, DIAG_UPRIGHT, DIAG_UPLEFT } DiagType;
		Box_Diag(float x=0.0f, float y=0.0f, float w=0.0f, float h=0.0f, DiagType d=DIAG_NONE, Vec2f velocity=Vec2f(0.0f,0.0f));

		DiagType get_diag() const;

		/* from jethro */
		/* http://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line */
		bool isUp(const Box &other) const;

		void set_velocity(Vec2f velocity);
		Vec2f get_velocity() const;

	protected:
		DiagType diag;
		Vec2f velocity;
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
