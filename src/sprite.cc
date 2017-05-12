#include "sprite.h"

/* SpriteSheet */
SpriteSheet::SpriteSheet(const char *fname, const char* jname) {
	LOG("loading SpriteSheet " << fname);
	int fx, fy, fw, fh;
	int framenums;
	sheet = load_bitmap(fname);
	if (sheet) {
		w = al_get_bitmap_width(sheet);
		h = al_get_bitmap_height(sheet);
	}

	/* open and parse the metadata file associated with the bitmap */
	/*
	char *fbuf;
	PHYSFS_file *f = PHYSFS_openRead(fname);
	fbuf = new char[PHYSFS_fileLength(f)+1];
	int length_read = PHYSFS_read(f, fbuf, sizeof(char), PHYSFS_fileLength(f));
	fbuf[length_read] = '\0';
	*/

	json data;

/*	data = json::parse(fbuf); */
	data = load_json(jname);
	sprnum = data["meta"]["sprnum"].get<int>();
	sprites.reserve(sprnum);
	strips.reserve(sprnum);

	for (int i = 0; i < sprnum; i++) {
		framenums = data["sprites"][i]["frames"].size();
		strips.push_back(std::vector<Box>());
		strips[i].reserve(framenums);
		for (int j = 0; j < framenums; j++) {
			fx = data["sprites"][i]["frames"][j]["frame"]["x"];
			fy = data["sprites"][i]["frames"][j]["frame"]["y"];
			fw = data["sprites"][i]["frames"][j]["frame"]["w"];
			fh = data["sprites"][i]["frames"][j]["frame"]["h"];
			strips[i].emplace_back(fx,fy,fw,fh); 
		}
		sprites.emplace_back(
				data["sprites"][i]["name"].get<std::string>(),
				sheet, 
				strips[i], 
				strips[i][0].get_w(), 
				strips[i][0].get_h(), 
				framenums); 
	}
	/*
	delete fbuf;
	PHYSFS_close(f);
	*/
}
SpriteSheet::~SpriteSheet(){
	if (sheet)
		al_destroy_bitmap(sheet);
	sheet = NULL;
	LOG("Sheet destroyed");
}
Sprite *SpriteSheet::get_sprite(int i) {
	if (i >= 0 && i < sprnum) {
		return &sprites[i];
	}
	else return NULL;
}
Sprite *SpriteSheet::get_sprite(std::string str) {
	for (auto &spr : sprites) {
		if (str == spr.getname())
			return &spr;
	}
	return NULL;
}
Sprite *SpriteSheet::operator[](int i) {
	return get_sprite(i);
}
Sprite *SpriteSheet::operator[](std::string str) {
	return get_sprite(str);
}


/* Sprite */
Sprite::Sprite(
		ALLEGRO_BITMAP *sheet, 
		float w, 
		float h, 
		int frames, 
		float gap, 
		float offx, 
		float offy, 
		float ox, 
		float oy) : w(w), h(h), frames(frames), x(ox), y(oy) {
	if (sheet) {
		subimages.reserve(frames);

		/* fill array of frames with sub-bitmaps */
		for (int i = 0; i < frames; i++) {
			subimages[i] = al_create_sub_bitmap(sheet, offx + (i*w + i*gap), offy, w, h);
		}
	}
}
Sprite::Sprite(std::string name, ALLEGRO_BITMAP *sheet, std::vector< Box > framearray, float w, float h, int n, float ox, float oy) : name(name), w(w), h(h), frames(n), x(ox), y(oy) {
	LOG("Sprite " << name << " created");
	if (sheet) {
		subimages.reserve(frames);
		/* Create an array of sub-bitmaps based on the array of boxes we've been passed */
		for (int i = 0; i < frames; i++){
			subimages[i] = al_create_sub_bitmap(
					sheet, 
					framearray[i].get_x(),
					framearray[i].get_y(),
					framearray[i].get_w(),
					framearray[i].get_h());
		}
	} 
}

Sprite::~Sprite() {
	for (auto &i : subimages) {
		al_destroy_bitmap(i);
		i = NULL;
	}
/*	LOG("Sprite destroyed"); */
}

float Sprite::get_x() const { return x; }
float Sprite::get_y() const { return y; }
float Sprite::get_w() const { return w; }
float Sprite::get_h() const { return h; }
int Sprite::getframes() const { return frames; }
std::string Sprite::getname() const { return name; }

ALLEGRO_BITMAP *Sprite::get_bitmap(int i) const {
	return subimages[i];
}
ALLEGRO_BITMAP *Sprite::operator [](int i) const {
	return get_bitmap(i);
}



void Sprite::set_name(std::string str) { name = str; }

void Sprite::sprite_center_origin(Origin o, float offsetx, float offsety) {
	switch (o) {
		case ORIGIN_DEFAULT:
			x = 0;
			y = 0;
			break;
		case ORIGIN_CENTER_MIDDLE:
			x = w/2;
			y = h/2;
			break;
		case ORIGIN_CENTER_BOTTOM:
			x = w/2;
			y = h-1;
			break;
		case ORIGIN_CENTER_TOP:
			x = w/2;
			y = 0;
	}
	x += offsetx;
	y += offsety;
}

void Sprite::sprite_draw(float destx, float desty, int f, int flags, ALLEGRO_COLOR blend, float angle, float xscale, float yscale) {
	int n = f % frames;
	if (subimages[n]) {
		al_draw_tinted_scaled_rotated_bitmap(subimages[n], blend, round_nearest(x), round_nearest(y), round_nearest(destx), round_nearest(desty), xscale, yscale, angle, flags);
	}

	else if (w != 0 && h != 0) {
		al_draw_filled_ellipse(destx, desty, w/2, h/2, al_map_rgb(126,0,0));	
	} else {
		al_draw_filled_ellipse(destx, desty, 8, 8, al_map_rgb(126,0,0));	
	}
}
