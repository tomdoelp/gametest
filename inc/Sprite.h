#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_physfs.h>
#include "../inc/global.h"
#include "../inc/Utils.h"

class Sprite {
	public:
		Sprite(const char *fname, float x=0.0, float y=0.0);
		Sprite(const char *fname, float x, float y, float w, float h);
		Sprite(const char *fname, float x, float y, float w, float h, const Box &b);
		~Sprite();

		float sprite_x();
		float sprite_y();
		float sprite_w();
		float sprite_h();

		void sprite_center_origin(bool round);

		void sprite_draw(float destx, float desty);
	protected:
		ALLEGRO_BITMAP *image;

		float x, y;
		float w, h;

		int frames;

		Box bbox;
		/* Box bbox(bx, by, bw, bh);
 		 * or just Box bbox(b); (copy constructor) */

		/* TODO: 
 		 * bounding box info
		 * a general box class ? probably a good idea
		 * bitmap
		 * frames from an animation strip
 		 */
};

/* Plan:
 * singleton ?
 * Objects get a pointer to a sprite
 * If they want a sprite somebody already loaded, they get a pointer to the same one
 * OR just do that at the beginning of a game / scene . . .
 */

#endif
