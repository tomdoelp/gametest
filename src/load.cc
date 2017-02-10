#include "load.h"


ALLEGRO_BITMAP *load_bitmap(const char* fname) {
	ALLEGRO_BITMAP *image = al_load_bitmap(fname);
	if (!image) 
		alert("Could not load image %s", fname);
	return image;
}

json load_json(const char* fname) {
	json j;
	std::ifstream i(fname);
	if (i)
		i >> j;
	else
		alert("Could not load json file %s", fname);

	return j;
}

SpriteSheet create_spritesheet_from_file(std::vector<std::string> fnames) {
	std::string cmd("./src/make_spritesheet ");
	for (auto s : fnames) {
		cmd += s;
	}
	
	std::system(cmd.c_str());

	return SpriteSheet(fnames.back().c_str(), "TODO get replace extension with .json");
}
