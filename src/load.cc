#include "load.h"
#include "sprite.h"


ALLEGRO_BITMAP *load_bitmap(const char* fname) {
	ALLEGRO_BITMAP *image = al_load_bitmap(fname);
	if (!image) 
		LOG("Could not load image " << fname);
	return image;
}

json load_json(const char* fname) {
	json j;
	std::ifstream i(fname);
	if (i)
		i >> j;
	else
		LOG("Could not load json file " << fname);

	return j;
}


ALLEGRO_SAMPLE *load_sound(const char* fname) {
	ALLEGRO_SAMPLE *sound = al_load_sample(fname);
	if (!sound) 
		LOG("Could not load sample " << fname);
	return sound;
}



ALLEGRO_AUDIO_STREAM *load_stream(const char* fname, int buffers, int samples) {
	ALLEGRO_AUDIO_STREAM *stream = al_load_audio_stream(fname, buffers, samples);
	if (!stream) 
		LOG("Could not load audio stream " << fname);
	return stream;
}


std::vector<SpriteSheet *> SheetManager::SheetList;
SheetManager::SheetManager() {}
SheetManager::~SheetManager() {
	for (int i = 0, max = SheetList.size(); i < max; i++) {
		delete SheetList[i];
	}
}

SpriteSheet *SheetManager::get_sheet(SheetName sheet) {
	if (SheetList.empty()) {
		for (int i = 0; i < SheetManager::SH_NUM; i++) { 
			SheetList.push_back(NULL); 
		} 
	}


	if (SheetList[sheet]){
		LOG("dup sprite sheet");
		return SheetList[sheet];
	}

	switch (sheet) {
		case SH_DEATH:
			SheetList[sheet] = new SpriteSheet("./res/sprites/death/death2.png", "./res/sprites/death/death2.json");
			break;
		case SH_SHADOW:
			SheetList[sheet] = new SpriteSheet("./res/sprites/shadow.png", "./res/sprites/shadow.json");
			break;

		case SH_DUMMY:
			SheetList[sheet] = new SpriteSheet("./res/sprites/dummy/dummy.png", "./res/sprites/dummy/dummy.json");
			break;
		default:
			break;
	}
	return SheetList[sheet];
}

void SheetManager::clear_sheets() {
	for (int i = 0, max = SheetList.size(); i < max; i++) {
		delete SheetList[i];
	}
	/*
	for (auto &s : SheetList)
		delete s;
		*/
}







/*SpriteSheet create_spritesheet_from_file(std::vector<std::string> fnames) {
  std::string cmd("./src/make_spritesheet ");
  for (auto s : fnames) {
  cmd += s;
  }

  std::system(cmd.c_str());

  return SpriteSheet(fnames.back().c_str(), "TODO get replace extension with .json");
  }*/ 
