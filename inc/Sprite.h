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

/*
 * Loading from zip file:
 *
 * 	PHYSFS_init(NULL);
 * 	if (!PHYSFS_mount("archive.zip","/", 1) {
 * 		//error checking
 * 	}
 *	al_set_physfs_file_interface();
 *
 *  //Use al_set_standard_file_interface(); to go back to normal.
 *  Or google that example where a dude used ALLEGRO_STATE or w/e
 */

/* SpriteStrip, how to get frames of animation from an image strip */
class SpriteStrip {
	public:
		SpriteStrip(float framew=0, float frameh=0, float gap=0, int row=0);
		~SpriteStrip();
		
		Box frame(int n);
		float getw();
		float geth();
		float getgap();
		int getrow();
		
	private:
		float w, h;
		float gap;
		int row;
};

/* Sprite. Holds bitmap & data, bounding box, and a strip for information */
class Sprite {
	public:
		Sprite(const char *fname, float x=0.0, float y=0.0); /* Single frame, get dimensions from file */
		Sprite(const char *fname, float x, float y, const SpriteStrip &s); /* animated */
		Sprite(const char *fname, float x, float y, const SpriteStrip &s, const Box &b); /* animated w/ bbox*/
		~Sprite();

		float sprite_x();
		float sprite_y();
		float sprite_w();
		float sprite_h();
		int getframes();

		void sprite_center_origin(bool round);

		void sprite_draw(float destx, float desty, int f=0);
	protected:
		ALLEGRO_BITMAP *image;

		float x, y;
		float w, h;

		SpriteStrip strip;
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
