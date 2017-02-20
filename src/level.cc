#include "level.h"

Layer::Layer(const char *name, int w, int h, std::vector<int> gids) : name(name), w(w), h(h), gids(gids) {}

Layer::Layer(pugi::xml_node &l) {
	name = l.attribute("name").value();
	w = l.attribute("width").as_int();
	h = l.attribute("height").as_int();

	pugi::xml_node d = l.child("data");

	/* for each tile in this layer */
	for(pugi::xml_node tile = d.child("tile"); tile; tile=tile.next_sibling("tile")) {
		gids.push_back(tile.attribute("gid").as_int());
	}
}
std::vector<int> Layer::get_gids() { return gids; }


Tileset::Tileset(int firstgid, const char *name, int tilew, int tileh, int tilecount, int columns) : firstgid(firstgid), name(name), tilew(tilew), tileh(tileh), tilecount(tilecount), columns(columns) {}

Tileset::Tileset(pugi::xml_node &t) {
	firstgid = t.attribute("firstgid").as_int();
	name = t.attribute("name").value();
	tilew = t.attribute("tilewidth").as_int();
	tileh = t.attribute("tileheight").as_int();
	tilecount = t.attribute("tilecount").as_int();
	columns = t.attribute("columns").as_int();
	spacing = 0;

	image_parent = load_bitmap((std::string("./res/maps/") + t.child("image").attribute("source").value()).c_str());

	/* for each tile in the set */
	for(int i = firstgid; i < tilecount+firstgid; i++) {
		/* make a sub-bitmap */
		tiles.emplace_back(al_create_sub_bitmap(
					image_parent, 
					(i % columns)*(tilew + spacing),
					(i / columns)*(tileh + spacing),
					tilew,
					tileh));
	}

	/* for each tile with properties */
	for(pugi::xml_node tile = t.child("tile"); tile; tile=tile.next_sibling("tile")) {
		int id = tile.attribute("id").as_int();
		if (tile.child("properties").child("property").attribute("name").value() == std::string("solid") &&
			tile.child("properties").child("property").attribute("value").as_bool()) {
			properties[id] = true;
		}
	}
}

Map::Map(const char* fname) {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fname);

	if (result){
		pugi::xml_node map = doc.child("map");
		w = map.attribute("width").as_int();
		h = map.attribute("height").as_int();
		tilew = map.attribute("tilewidth").as_int();
		tileh = map.attribute("tileheight").as_int();
		nextobj = map.attribute("nextobjectid").as_int();

		/* for each tileset */
		for (pugi::xml_node t = map.child("tileset"); t; t=t.next_sibling("tileset")) {
			int firstgid = t.attribute("firstgid").as_int();
			int tw = t.attribute("tilewidth").as_int();
			int th = t.attribute("tileheight").as_int();
			int tilecount = t.attribute("tilecount").as_int();
			int columns = t.attribute("columns").as_int();
			ALLEGRO_BITMAP *ts = load_bitmap((std::string("./res/maps/") + t.child("image").attribute("source").value()).c_str());
			int spacing = 0;

			tilesets.push_back(ts);
			for(int i = 0; i < tilecount; i++) {
				/* make a sub-bitmap */
				tiles.emplace_back(al_create_sub_bitmap(
							ts, 
							(i % columns)*(tw + spacing),
							(i / columns)*(th + spacing),
							tw,
							th));
				solid.push_back(false);
			}


			for (pugi::xml_node tile = t.child("tile"); tile; tile=tile.next_sibling("tile")) {
				int id = tile.attribute("id").as_int();
				if (tile.child("properties").child("property").attribute("name").value() == std::string("solid") &&
						tile.child("properties").child("property").attribute("value").as_bool()) {
					solid[id + firstgid] = true;
				}
			}
		}

		/* for each layer */
		for (pugi::xml_node l = map.child("layer"); l; l=l.next_sibling("layer")) {
			layers.emplace_back(l);
		}
	} else 
		alert("XML oopsie");
}
Map::~Map() {
	for (auto& ts : tilesets) {
		al_destroy_bitmap(ts);
	}
	for (auto& t : tiles) {
		al_destroy_bitmap(t);
	}

}

void Map::draw_layer(int n) {
	int temp = 0;
	std::vector<int> gids = layers[n].get_gids();

	for (std::size_t i = 0, max = gids.size(); i != max; i++) {
		temp = gids[i];
		if (temp > 0) {
			al_draw_bitmap(tiles[temp-1], (i % w)*tilew, (i/w)*tileh, 0);
		}
	}
		
	/*
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			temp = gids[i + j*h];
			if (temp > 0) {
				al_draw_bitmap(tiles[temp-1], i*tilew, j*tileh, 0);
			}
			alert("drawing gid %d at %d, %d", temp, i*tilew, j*tileh);
		}
	}*/
}
/*
   Layer::Layer(std::vector<int> tile_list, int w, int h) : w(w), h(h) {
   for (int i = 0; i < w; i++) {
   for (int j = 0; j < h; j++) {
   tile_grid[i][j] = tile_list[i + j*w];
   }
   }

   }
   std::vector<std::vector<int>> Layer::get_tile_grid() { return tile_grid; }

   Tileset::Tileset(ALLEGRO_BITMAP *source, int firstgid, int tilew, int tileh, int spacing, int tilecount, int cols) : source(source), firstgid(firstgid), tilew(tilew), tileh(tileh), spacing(spacing), tilecount(tilecount), cols(cols) {
   for (int i = firstgid; i < tilecount+firstgid; i++) {
   tiles[i] = al_create_sub_bitmap(
   source, 
   (i % cols)*(tilew + spacing), 
   (i / cols)*(tileh + spacing), 
   tilew,
   tileh);
   }
   }
   void Tileset::draw_tile(int gid, float x, float y) {
   al_draw_bitmap(tiles[gid], x, y, 0);
   }

   Level::Level(const char* fname) {
   pugi::xml_document doc;
   pugi::xml_parse_result result = doc.load_file(fname);

   if (!result) {
   alert("XML panic");
   }

   pugi::xml_node map = doc.child("map");
   map_w = map.attribute("width").as_int();
   map_h = map.attribute("height").as_int();
   tile_w = map.attribute("tilewidth").as_int();
   tile_h = map.attribute("tileheight").as_int();
   total_w = map_w * tile_w;
   total_h = map_h * tile_h;


   for (pugi::xml_node t = map.child("tileset"); t; t = t.next_sibling("tileset")) {

   image_source = (std::string("./res/maps/") + t.attribute("source").value()).c_str();

   tileset_images.emplace_back(image_source, t.attribute("


   tilesets.emplace_back(
   (std::string("./res/maps/") + t.attribute("source").value()).c_str(),
   t.attribute("firstgid").as_int(),
   t.attribute("tilewidth").as_int(),
   t.attribute("tileheight").as_int(),
   t.attribute("spacing").as_int(),
   t.attribute("tilecount").as_int(),
   t.attribute("columns").as_int());



   for (pugi::xml_node i = t.child("tile"); t; t = t.next_sibling("tile")) {
   pugi::xml_node p = i.child("properties");

   if (p.attribute("name").value() == std::string("solid") && p.attribute("value").as_bool())
   solidtiles[i.attribute("id").as_int()] = true;

   }
   }

   for (pugi::xml_node l = map.child("layer"); l; l = l.next_sibling("layer")) {
   std::vector< int > temp;

for (pugi::xml_node t = l.child("data").child("tile"); t; t = t.next_sibling("tile")) {
	temp.push_back(t.attribute("gid").as_int());
}

layers.emplace_back(temp, l.attribute("width").as_int(), l.attribute("height").as_int());
}
}
void Level::draw_layer(int n) {
	std::vector< std::vector<int> > tile_grid = layers[n].get_tile_grid();

	for (int j = 0; j < map_h; j++) {
		for (int i = 0; i < map_w; i++) {
		}
	}


}
*/



