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

		void draw_layer(int n);
	private:
		int w, h;
		int tilew, tileh;
		int nextobj;

		std::vector<Layer> layers;
		std::vector<ALLEGRO_BITMAP*> tilesets;
		std::vector<ALLEGRO_BITMAP*> tiles;
		std::vector<bool> solid;
};

/*namespace pt = boost::property_tree; */

/*class Layer {
	public: 
		Layer(std::vector<int> tile_list, int w, int h);

		std::vector< std::vector< int > > get_tile_grid();
	private:
		std::vector< std::vector< int > > tile_grid;
		int w, h;
};
		


class Tileset {
	public:
		Tileset(ALLEGRO_BITMAP *source, int firstgid, int tilew, int tileh, int spacing, int tilecount, int cols); 

		void draw_tile(int gid, float x, float y);
	private:
		ALLEGRO_BITMAP *source; 
		const char* source;  
		int firstgid;
		int w, h;
		int tilew, tileh;
		int spacing;
		int tilecount, cols;

		std::vector< ALLEGRO_BITMAP* > tiles;
};

class Level {
	public:
		Level(const char* fname);

		void draw_background();
		void draw_objects();  
		void draw_foreground();
		void draw_layer(int n);
		std::vector< Box > get_collision_tiles(Box bbox);
	private:
		const char *image_source;

		std::vector< ALLEGRO_BITMAP* > tileset_images;

		std::vector< bool > solidtiles;
		std::vector< Tileset > tilesets;
		std::vector< Layer > layers;
		std::vector< ALLEGRO_BITMAP* > tiles;

		int total_w, total_h;
		int map_w, map_h;
		int tile_w, tile_h;
};

*/
#endif
