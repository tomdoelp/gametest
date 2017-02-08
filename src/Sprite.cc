#include "../inc/Sprite.h"

/* SpriteStrip */
SpriteStrip::SpriteStrip(float framew, float frameh, float gap, int row) : w(framew), h(frameh), gap(gap), row(row) {}
SpriteStrip::~SpriteStrip() {}

Box SpriteStrip::frame(int n) {
	float left, top;
	left = (n*w + n*gap);
	top = row*h;

	Box b(left,top,w,h);
	return b;
}
float SpriteStrip::getw() { return w; }
float SpriteStrip::geth() { return h; }
float SpriteStrip::getgap() { return gap; }
int SpriteStrip::getrow() { return row; }




/* Sprite */
Sprite::Sprite(const char *fname, float x, float y) : x(x), y(y) {
	image = al_load_bitmap(fname);
	if (!image) 
		alert("Could not load image %s", fname);
	else {
		w = al_get_bitmap_width(image);
		h = al_get_bitmap_height(image);
	}

	SpriteStrip strip;
	frames = 1;
	Box bbox;
	
}
Sprite::Sprite(const char *fname, float x, float y, const SpriteStrip &s) : x(x), y(y) {
	int widthspan;

	image = al_load_bitmap(fname);
	strip = s;
	w = strip.getw();
	h = strip.geth();

	if (!image) 
		alert("Could not load image %s", fname);
	else {
		widthspan = al_get_bitmap_width(image) / w;
		frames = ((widthspan > 1) ? widthspan : 1);
	}

	Box bbox;
}
Sprite::Sprite(const char *fname, float x, float y, const SpriteStrip &s, const Box &b) : x(x), y(y) {
	int widthspan;

	image = al_load_bitmap(fname);
	strip = s;
	w = strip.getw();
	h = strip.geth();

	if (!image) 
		alert("Could not load image %s", fname);
	else {
		widthspan = al_get_bitmap_width(image) / w;
		frames = ((widthspan > 1) ? widthspan : 1);
	}

	bbox = b;
}
Sprite::~Sprite() {
	if (image)
		al_destroy_bitmap(image);
}

float Sprite::sprite_x() { return x; }
float Sprite::sprite_y() { return y; }
float Sprite::sprite_w() { return w; }
float Sprite::sprite_h() { return h; }
int Sprite::getframes() { return frames; }

void Sprite::sprite_center_origin(bool round) {
	x = w/2;
	y = h/2;

	if (round) {
		x = round_nearest(x);
		y = round_nearest(y);
	}
}


void Sprite::sprite_draw(float destx, float desty, int f) { 
	Box reg;
	if (image) {
		if (frames == 1)
			al_draw_bitmap(image, destx-x, desty-y, 0); 
		else {
			reg = strip.frame(f % frames);
			al_draw_bitmap_region(image, reg.getx(), reg.gety(), reg.getw(), reg.geth(), destx-x, desty-y, 0);
		}
	} 
	else if (w != 0 && h != 0) {
		al_draw_filled_ellipse(destx, desty, w/2, h/2, al_map_rgb(126,0,0));	
	} else {
		al_draw_filled_ellipse(destx, desty, 8, 8, al_map_rgb(126,0,0));	
	}
}




