#include "level.h"
#include "world.h"

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

Map::Map(){}

Map::Map(World *world, const char* fname) : world(world) {
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
			if (pugi::xml_attribute source = t.attribute("source")) {
				pugi::xml_document sourcedoc;
				std::string tilesetfile = std::string("./res/maps/") + source.value();
/*				LOG("HERE " << tilesetfile); */
				pugi::xml_parse_result sourceres = sourcedoc.load_file(tilesetfile.c_str());
				if (sourceres) {
					pugi::xml_node tileset = sourcedoc.child("tileset");
					int tw = tileset.attribute("tilewidth").as_int();
					int th = tileset.attribute("tileheight").as_int();
					int tilecount = tileset.attribute("tilecount").as_int();
					int columns = tileset.attribute("columns").as_int();
					ALLEGRO_BITMAP *ts_bitmap = load_bitmap((std::string("./res/maps/") + tileset.child("image").attribute("source").value()).c_str());
					int spacing = 0;

					tilesets.push_back(ts_bitmap);
					for (int i = 0; i < tilecount; i++) {
						tiles.emplace_back(al_create_sub_bitmap(
									ts_bitmap,
									(i % columns)*(tw + spacing),
									(i / columns)*(th + spacing),
									tw,
									th));
						tiletype.push_back(TILE_FREE);
					}
					for (pugi::xml_node tile = tileset.child("tile"); tile; tile=tile.next_sibling("tile")) {
						int id = tile.attribute("id").as_int();
						if (tile.child("properties").child("property").attribute("name").value() == std::string("type")) {
							tiletype[id + firstgid] = static_cast<SolidType>(tile.child("properties").child("property").attribute("value").as_int());
						}
					}
				}
				else
					LOG("XML tileset oops");
			}
			else {
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
					tiletype.push_back(TILE_FREE);
				}


				for (pugi::xml_node tile = t.child("tile"); tile; tile=tile.next_sibling("tile")) {
					int id = tile.attribute("id").as_int();
					if (tile.child("properties").child("property").attribute("name").value() == std::string("type")) {
						tiletype[id + firstgid] = static_cast<SolidType>(tile.child("properties").child("property").attribute("value").as_int());
					}
				}
			}
		}

		/* for each layer */
		for (pugi::xml_node l = map.child("layer"); l; l=l.next_sibling("layer")) {
			layers.emplace_back(l);
		}

		/* for each object group */
		for (pugi::xml_node og = map.child("objectgroup"); og; og=og.next_sibling("objectgroup")){
			for (pugi::xml_node o = og.child("object"); o; o=o.next_sibling("object")) {
				std::string otype = o.attribute("type").value();

				if (otype == std::string("Player")) {
					world->create_visible<Player>(
							o.attribute("x").as_float(),	
							o.attribute("y").as_float());
				}
				if (otype == std::string("Dummy")) {
					world->create_visible<Dummy>(
							o.attribute("x").as_float(),	
							o.attribute("y").as_float());
				}
			}
		}
	} 
	else 
		LOG("XML oopsie");
}
Map::~Map() {
	LOG("destroying Tilesets...");
	for (auto& ts : tilesets) {
		al_destroy_bitmap(ts);
	}
	LOG("destroying Tiles...");
	for (auto& t : tiles) {
		al_destroy_bitmap(t);
	}
	LOG("Map destroyed");
}

int Map::numlayers() { return layers.size(); }

void Map::draw_layer(float x, float y, int n) {
	/* Okay let me explain this debug stuff
 	 * If debug is on, then the game can still run when something cant be loaded
	 * But we don't waste time checking that if we're in production build. 
	 * I could check once and cache it so it wouldn't take time
	 * but w/e this is easier
	 * I mean it's constant time anyway because I'm guaranteeing only four layers
	 * But I'm paranoid about being too slow even though there's no reason to optimize yet.
	 * Also, constant time doesn't mean fast. 
	 */
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	int temp = 0;
	std::vector<int> gids = layers[n].get_gids();

	for (std::size_t i = 0, max = gids.size(); i != max; i++) {
		temp = gids[i];
		if (temp > 0) {
			al_draw_bitmap(tiles[temp-1],x+ (i % w)*tilew,y+ (i/w)*tileh, 0);
		}
	}
}

void Map::draw_layer_region(float x, float y, int n, Box b) {
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	int firstcol, lastcol, firstrow, lastrow;
	int firstcol_b = b.get_x() / tilew;
	int lastcol_b = (b.get_x() + b.get_w()-1) / tilew +1;
	int firstrow_b = b.get_y() / tileh;
	int lastrow_b = (b.get_y() + b.get_h()-1) / tileh +1;

	/* make sure we only try to draw within the bounds of the map */
	firstcol = ((firstcol_b > 0) ? firstcol_b : 0);
	lastcol = ((lastcol_b < w) ? lastcol_b : w);
	firstrow = ((firstrow_b > 0) ? firstrow_b : 0);
	lastrow = ((lastrow_b < h) ? lastrow_b : h);

	int temp = 0;
	std::vector<int> gids = layers[n].get_gids();

	for (int i = firstcol; i < lastcol; i++) {
		for (int j = firstrow; j < lastrow; j++) {
			temp = gids[i+j*w];
			if (temp > 0)
				al_draw_bitmap(tiles[temp-1], x+i*tilew, y+j*tileh, 0);
		}
	}
}

void Map::draw_row(float x, float y, int r, int l) {
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	if (r < 0 || r >= h) 
		return;
	int temp = 0;
	int cols = w;
	std::vector <int> gids = layers[l].get_gids();

	for (int i = cols*r, max = i+cols; i < max; i++) {
		temp = gids[i];
		if (temp > 0) {
			al_draw_bitmap(tiles[temp-1], x+(i % w)*tilew, y+(i/w)*tileh, 0);
		}
	}
}

void Map::draw_row_region(float x, float y, int r, int l, Box b) {
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	if (r < 0 || r >= h)
		return;

	int firstcol_b = b.get_x() / tilew;
	int lastcol_b = (b.get_x() + b.get_w()) / tilew +1;
	int firstcol = ((firstcol_b > 0) ? firstcol_b : 0);
	int lastcol = ((lastcol_b < w) ? lastcol_b : w);

	int temp = 0;
	std::vector<int> gids = layers[l].get_gids();

	for (int i = w*r+firstcol, max = w*r+lastcol; i < max; i++) {
		temp = gids[i];
		if (temp > 0) {
			al_draw_bitmap(tiles[temp-1], x+(i % w)*tilew, y+(i/w)*tileh, 0);
		}
	}
}

void Map::draw_layer_from_row(float x, float y, int r, int l) {
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	if (r < 0 || r >= h) 
		return;

	int temp = 0;
	int cols = w;
	std::vector<int> gids = layers[l].get_gids();

	for (std::size_t i = cols*r, max = gids.size(); i != max; i++) {
		temp = gids[i];
		if (temp > 0) {
			al_draw_bitmap(tiles[temp-1], x+(i % w)*tilew, y+(i/w)*tileh, 0);
		}
	}
}

void Map::draw_layer_region_from_row(float x, float y, int r, int l, Box b) {
#if DEBUG
	if (layers.size() == 0)
		return;
#endif
	if (r < 0 || r >= h)
		return;

	int firstcol, lastcol, firstrow, lastrow;
	int firstcol_b = b.get_x() / tilew;
	int lastcol_b = (b.get_x() + b.get_w()-1) / tilew +1;
	int lastrow_b = (b.get_y() + b.get_h()-1) / tileh +1;

	/* make sure we only try to draw within the bounds of the map */
	/* also I would use elvis op but I don't think that's totally portable */
	firstcol = ((firstcol_b > 0) ? firstcol_b : 0);
	lastcol = ((lastcol_b < w) ? lastcol_b : w);
	firstrow = r;
	lastrow = ((lastrow_b < h) ? lastrow_b : h);

	int temp = 0;
	std::vector<int> gids = layers[l].get_gids();

	for (int i = firstcol; i < lastcol; i++) {
		for (int j = firstrow; j < lastrow; j++) {
			temp = gids[i+j*w];
			if (temp > 0)
				al_draw_bitmap(tiles[temp-1], x+i*tilew, y+j*tileh, 0);
		}
	}
}

/*
   TILE_FREE, 
   TILE_SOLID_SQUARE, 
   TILE_SOLID_HALF_TOP, 
   TILE_SOLID_HALF_BOTTOM,
   TILE_SOLID_HALF_RIGHT,
   TILE_SOLID_HALF_LEFT,
   TILE_SOLID_DIAG_UPRIGHT,
   TILE_SOLID_DIAG_UPLEFT
   */

std::vector<Box_Diag> Map::get_collision_box(const Box &bbox) {
	int firstcol = (bbox.get_x()) / tilew;
	int lastcol = (bbox.get_x() + bbox.get_w()-1) / tilew;
	int firstrow = (bbox.get_y()) / tileh;
	int lastrow = (bbox.get_y() + bbox.get_h()-1) / tileh;
	std::vector<Box_Diag> boxes;
	std::vector<int> tilemap = layers[LAYER_COLLISIONS].get_gids();
	int totaltiles = tilemap.size();

	for (int i = firstcol; i <= lastcol; i++) {
		for (int j = firstrow; j <= lastrow; j++) {
			if (i+j*w > 0 && i+j*w < totaltiles){
				switch (tiletype[tilemap[i+j*w]]) {
					case TILE_FREE:
						break;
					case TILE_SOLID_HALF_TOP:
						boxes.emplace_back(i*tilew, j*tileh, tilew, tileh/2);
						break;
					case TILE_SOLID_HALF_BOTTOM:
						boxes.emplace_back(i*tilew, j*tileh+(tileh/2), tilew, tileh/2);
						break;
					case TILE_SOLID_HALF_RIGHT:
						boxes.emplace_back(i*tilew+(tilew/2), j*tileh, tilew/2, tileh);
						break;
					case TILE_SOLID_HALF_LEFT:
						boxes.emplace_back(i*tilew, j*tileh, tilew/2, tileh);
						break;
					case TILE_SOLID_DIAG_UPRIGHT:
						boxes.push_back(Box_Diag(i*tilew, j*tileh, tilew, tileh, Box_Diag::DIAG_UPRIGHT));
						break;
					case TILE_SOLID_DIAG_UPLEFT:
						boxes.push_back(Box_Diag(i*tilew, j*tileh, tilew, tileh, Box_Diag::DIAG_UPLEFT));
						break;
					case TILE_SOLID_SQUARE:
					default:
						boxes.emplace_back(i*tilew, j*tileh, tilew, tileh);
						break;
				}
			}
		}
	}
	return boxes;
}

Vec2f Map::get_collision_vec(const Box &now, const Box &next) {
	/* get all boxes that have a solid type */
	std::vector<Box_Diag> coltiles = get_collision_box(next);

	Vec2f intersect;

	float boxw = next.get_w();
	float boxh = next.get_h();

	for (auto &b : coltiles) {
		b.set_velocity(Vec2f(next.get_x() - now.get_x(), 0.0f));
		float intersect_h = Box(next.get_x(), now.get_y(), boxw, boxh).get_collision_vec(b).get_x();
		b.set_velocity(Vec2f(0.0f, next.get_y() - now.get_y()));
		float intersect_v = Box(now.get_x(), next.get_y(), boxw, boxh).get_collision_vec(b).get_y();
		if (intersect_h != 0.0f) {
			intersect.set_x(intersect_h);
		}
		if (intersect_v != 0.0f) {
			intersect.set_y(intersect_v); 
		}
		if ((intersect_h == intersect_v || -intersect_h == intersect_v ) && intersect_h != 0.0f){
			intersect.set_x(0.0f);
		}
	}

	return intersect;
}


