#include "sprite.h"

/* SpriteSheet */
SpriteSheet::SpriteSheet(ALLEGRO_BITMAP *sheet, const char* jname) : sheet(sheet) {
	int fx, fy, fw, fh;
	int framenums;
	int sprnum;
/*	std::vector< std::vector< Box > > strips; */
	w = al_get_bitmap_width(sheet);
	h = al_get_bitmap_height(sheet);

	/* open and parse the metadata file associated with the bitmap */
	json data;

	data = load_json(jname);
	sprnum = data["sprites"].size();
	sprites.reserve(spritenum);
	strips.reserve(spritenum);

	for (int i = 0; i < sprnum; i++) {
		framenums = data["sprites"][i].size();
		strips[i].reserve(framenums);
		for (int j = 0; j < framenums; j++) {
			fx = data["sprites"][i]["frames"][j]["frame"]["x"];
			fy = data["sprites"][i]["frames"][j]["frame"]["y"];
			fw = data["sprites"][i]["frames"][j]["frame"]["w"];
			fh = data["sprites"][i]["frames"][j]["frame"]["h"];
			strips[i][j] = Box(fx,fy,fw,fh); 
		}
		sprites[i] = Sprite(
				data["sprites"][i]["name"].get<std::string>().c_str(),
				sheet, 
				strips[i], 
				strips[i][0].getw(), 
				strips[i][0].geth(), 
				framenums); 
	}
}
SpriteSheet::~SpriteSheet(){
	if (sheet)
		al_destroy_bitmap(sheet);
}
Sprite *SpriteSheet::getsprite(int i) {
	if (i >= 0 && i < spritenum) {
		return &sprites[i];
	}
	else return NULL;
}
Sprite *SpriteSheet::operator[](int i) {
	return getsprite(i);
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
Sprite::Sprite(const char *name, ALLEGRO_BITMAP *sheet, std::vector< Box > framearray, float w, float h, int n, float ox, float oy) : name(name), w(w), h(h), frames(n), x(ox), y(oy) {
	if (sheet) {
		subimages.reserve(frames);
		/* Create an array of sub-bitmaps based on the array of boxes we've been passed */
		for (int i = 0; i < frames; i++){
			subimages[i] = al_create_sub_bitmap(
					sheet, 
					framearray[i].getx(),
					framearray[i].gety(),
					framearray[i].getw(),
					framearray[i].geth());
		}
	} 
}

Sprite::~Sprite() {
	for (auto i : subimages) {
		al_destroy_bitmap(i);
		i = NULL;
	}
}

float Sprite::getx() { return x; }
float Sprite::gety() { return y; }
float Sprite::getw() { return w; }
float Sprite::geth() { return h; }
int Sprite::getframes() { return frames; }

void Sprite::sprite_center_origin(bool round) {
	x = w/2;
	y = h/2;

	if (round) {
		x = round_nearest(x);
		y = round_nearest(y);
	}
}

void Sprite::sprite_draw(float destx, float desty, int f, int flags, float angle, float xscale, float yscale) {
	int n = f % frames;
	if (subimages[n]) {
		al_draw_scaled_rotated_bitmap(subimages[n], x, y, destx, desty, xscale, yscale, angle, flags);
	}

	else if (w != 0 && h != 0) {
		al_draw_filled_ellipse(destx, desty, w/2, h/2, al_map_rgb(126,0,0));	
	} else {
		al_draw_filled_ellipse(destx, desty, 8, 8, al_map_rgb(126,0,0));	
	}
}
