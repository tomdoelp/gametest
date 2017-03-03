#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <stdlib.h>
#include <allegro5/allegro_image.h>
#include <pugixml.hpp>

#include "global.h"
#include "load.h"

class Layer {
	public: 
		Layer(const char *name, int w, int h, std::vector<int> gids);
		Layer(pugi::xml_node &l);
		std::vector<int> get_gids();
	private:
		const char* name;
		int w, h;
		std::vector<int> gids;
};
class Tileset {
	public:
		Tileset(int firstgid, const char *name, int tilew, int tileh, int tilecount, int columns);
		Tileset(pugi::xml_node &t);
	private:
		int firstgid;
		const char* name;
		int tilew, tileh;
		int tilecount;
		int columns;
		int spacing;

		ALLEGRO_BITMAP *image_parent;
		std::vector<ALLEGRO_BITMAP*> tiles; 

		std::vector<bool> properties;
};
class Map {
	public:
		Map(const char *fname);
		~Map();

		int tilew, tileh;

		void draw_layer(int n);
		void draw_row(int r, int l);
		void draw_layer_from_row(int r, int l);
		int numlayers();
		std::vector<Box> get_collision_box(float ox, float oy, float ow, float oh);
		std::vector<Box> get_collision_box(Box &bbox);
		std::vector<int> get_containing_cells(const Box &bbox);
	private:
		int w, h;
		int nextobj;

		std::vector<Layer> layers;
		std::vector<ALLEGRO_BITMAP*> tilesets;
		std::vector<ALLEGRO_BITMAP*> tiles;
		std::vector<int> solid;
};

#endif
