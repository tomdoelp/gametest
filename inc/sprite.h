#ifndef SPRITE_H
#define SPRITE_H

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


/* Sprite. Holds bitmap & data, bounding box, and a strip for information */
class Sprite {
	public:
typedef enum SPRITE_ORIGIN { ORIGIN_DEFAULT, ORIGIN_CENTER_MIDDLE, ORIGIN_CENTER_BOTTOM, ORIGIN_CENTER_TOP } Origin;
		Sprite(ALLEGRO_BITMAP *sheet, float w, float h, int frames=1, float gap=0, float offx=0, float offy=0, float ox=0, float oy=0);
		Sprite(const char* name, ALLEGRO_BITMAP *sheet, std::vector<Box> framearray, float w, float h, int n, float ox=0, float oy=0);
		~Sprite();

		float get_x();
		float get_y();
		float get_w();
		float get_h();
		int getframes();
		const char *getname();

		void sprite_center_origin(Origin o, float offsetx=0, float offsety=0);
		void sprite_set_origin(float ox, float oy);

		void sprite_draw(float destx, float desty, int f=0, int flags=0, float angle=0, float xscale=1, float yscale=1);
	protected:
		const char *name;
		std::vector< ALLEGRO_BITMAP* > subimages;

		float w, h;
		int frames, index;
		float x, y;
};



/* This will load and store sprite sheet data that I will parse from JSON files */
class SpriteSheet {
	public:
		SpriteSheet(const char* fname, const char* jname);
		~SpriteSheet();

		float get_w();
		float get_h();

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



#endif
