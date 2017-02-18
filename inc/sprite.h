#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_physfs.h>

#include "global.h"
#include "utils.h"
#include "json.h"
#include "load.h"
using json = nlohmann::json;

/* TODO at some point stop passing the size of the arrays since they are vectors now */

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

/* OKAY NEW PLAN
 * SpriteSheet holds the parent bitmap
 * Sprite takes a sheet, h, w, originx=0, originy=0 number of frames=0, row=0, offx=0, offy=0, gap=0
 * Holds an array of sub-bitmaps, one for each frame.
 * sprite_draw() takes n=0, draws the nth sub-bitmap
 *
 * */
 
/* SpriteStrip, how to get frames of animation from an image strip */
/*class SpriteStrip {
	public:
		SpriteStrip(float framew, float frameh, float frames, int row, float gap=0, bool centered=false, float ox=0, float oy=0);
		~SpriteStrip();
		
		float getw();
		float geth();
		float getframes();
		int getrow();
		float getgap();
		float getoriginx();
		float getoriginy();
		bool getcentered();

		Box operator[](int i);
		
	private:
		float w, h, frames;
		int row;
		float gap;
		bool centered;
		float ox, oy;
};*/


/* Sprite. Holds bitmap & data, bounding box, and a strip for information */
class Sprite {
	public:
		Sprite(ALLEGRO_BITMAP *sheet, float w, float h, int frames=1, float gap=0, float offx=0, float offy=0, float ox=0, float oy=0);
		Sprite(const char* name, ALLEGRO_BITMAP *sheet, std::vector<Box> framearray, float w, float h, int n, float ox=0, float oy=0);
		~Sprite();

		float getx();
		float gety();
		float getw();
		float geth();
		int getframes();
		const char *getname();

		void sprite_center_origin(bool round);
		void sprite_set_origin(float ox, float oy);

		void sprite_draw(float destx, float desty, int f=0, int flags=0, float angle=0, float xscale=1, float yscale=1);
	protected:
		const char *name;
		std::vector< ALLEGRO_BITMAP* > subimages;

		float w, h;
		int frames, index;
		float x, y;

		Box bbox;

		/* TODO: 
 		 * bounding box info
 		 */
};


/* This will load and store sprite sheet data that I will parse from JSON files */
class SpriteSheet {
	public:
		SpriteSheet(const char* fname, const char* jname);
		~SpriteSheet();

		float getw();
		float geth();

		ALLEGRO_BITMAP *getbitmap();

		Sprite *getsprite(int i);
		Sprite *getsprite(const char *name);
		Sprite *operator[](int i);
		Sprite *operator[](const char *name);
	private:
		ALLEGRO_BITMAP *sheet;
		float w; /* total sheet width */
		float h; /* total sheet height */
		int sprnum;
		
		std::vector< Sprite > sprites;
		std::vector< std::vector< Box > > strips;
};
/* Plan:
 * singleton ?
 * Objects get a pointer to a sprite
 * If they want a sprite somebody already loaded, they get a pointer to the same one
 * OR just do that at the beginning of a game / scene . . .
 */

#endif
