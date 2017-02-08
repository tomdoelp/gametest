#include "../inc/Sprite.h"

Sprite::Sprite(const char *fname, float x, float y) : x(x), y(y) {
	image = al_load_bitmap(fname);

	if (image) {
		w = al_get_bitmap_width(image);
		h = al_get_bitmap_height(image);
	}
	else {
		alert("Could not load image %s", fname);
	}

	frames = 1;
	Box bbox;
}
Sprite::Sprite(const char *fname, float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {
	image = al_load_bitmap(fname);
	frames = 1;
	Box bbox;
}
Sprite::Sprite(const char *fname, float x, float y, float w, float h, Box const & b) : x(x), y(y), w(w), h(h) {
	image = al_load_bitmap(fname);
	frames = 1;
	Box bbox(b);
}

Sprite::~Sprite() {
	if (image)
		al_destroy_bitmap(image);
}

float Sprite::sprite_x() { return x; }
float Sprite::sprite_y() { return y; }
float Sprite::sprite_w() { return w; }
float Sprite::sprite_h() { return h; }

void Sprite::sprite_center_origin(bool round) {
	x = w/2;
	y = h/2;

	if (round) {
		x = round_nearest(x);
		y = round_nearest(y);
	}
}


void Sprite::sprite_draw(float destx, float desty) { 
	if (image) {
		al_draw_bitmap(image, destx-x, desty-y, 0); 
	} else if (w != 0 && h != 0) {
		al_draw_filled_ellipse(destx, desty, w/2, h/2, al_map_rgb(126,0,0));	
	} else {
		al_draw_filled_ellipse(destx, desty, 8, 8, al_map_rgb(126,0,0));	
	}
	
}

