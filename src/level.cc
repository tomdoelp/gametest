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
			}
		}
	} 
	else 
		LOG("XML oopsie");
}
Map::~Map() {
	for (auto& ts : tilesets) {
		al_destroy_bitmap(ts);
	}
	for (auto& t : tiles) {
		al_destroy_bitmap(t);
	}

}

int Map::numlayers() { return layers.size(); }

void Map::draw_layer(float x, float y, int n) {
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


std::vector<Box> Map::get_collision_box(const Box &bbox) {
	int firstcol = (bbox.get_x()) / tilew;
	int lastcol = (bbox.get_x() + bbox.get_w()-1) / tilew;
	int firstrow = (bbox.get_y()) / tileh;
	int lastrow = (bbox.get_y() + bbox.get_h()-1) / tileh;
	std::vector<Box> boxes;
	std::vector<int> tilemap = layers[LAYER_COLLISIONS].get_gids();
	int totaltiles = tilemap.size();

	for (int i = firstcol; i <= lastcol; i++) {
		for (int j = firstrow; j <= lastrow; j++) {
			if (i+j*w > 0 && i+j*w < totaltiles &&
					tiletype[tilemap[i+j*w]] == TILE_SOLID_SQUARE){
				boxes.emplace_back(i*tilew, j*tileh, tilew, tileh);
			}
		}
	}
	return boxes;
}

Vec2f Map::get_collision_vec(const Box &now, const Box &next) {
	/* get all boxes that have a solid type */
	std::vector<Box> coltiles = get_collision_box(next);

	Vec2f intersect;

	float boxw = next.get_w();
	float boxh = next.get_h();

	for (auto &b : coltiles) {
		/*		alert("me  [%.2f - %.2f]\nyou [%.2f - %.2f]", 
				next.get_y(), next.get_y()+next.get_h(),
				b.get_y(), b.get_y()+b.get_h());
				*/
		float intersect_h = Box(next.get_x(), now.get_y(), boxw, boxh).get_collision_vec(b).get_x();
		float intersect_v = Box(now.get_x(), next.get_y(), boxw, boxh).get_collision_vec(b).get_y();
		if (intersect_h != 0.0f) {
			intersect.set_x(intersect_h);
		}
		if (intersect_v != 0.0f) {
			intersect.set_y(intersect_v); 
		}
		if ((intersect_h == intersect_v || -intersect_h == intersect_v ) && intersect_h != 0.0f){
			intersect.set_y(0.0f);
			alert("!");
		}
	}

	return intersect;
}


