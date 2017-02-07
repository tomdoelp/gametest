#include "../inc/Sprite.h"

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

void Sprite::draw(float destx, float desty) { al_draw_bitmap(image, destx, desty, 0); }


