#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h> // link with -lm ?

#include "global.h"

class Box {
	public:
		Box(float x=0, float y=0, float w=0, float h=0);
		~Box();
		float getx();
		float gety();
		float getw();
		float geth();

		void setx(float x);
		void sety(float y);
		void setw(float w);
		void seth(float h);
	
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
