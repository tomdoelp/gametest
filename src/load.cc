#include "load.h"
#include "sprite.h"


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


ALLEGRO_SAMPLE *load_sound(const char* fname) {
	ALLEGRO_SAMPLE *sound = al_load_sample(fname);
	if (!sound) 
		alert("Could not load sample %s", fname);
	return sound;
}



ALLEGRO_AUDIO_STREAM *load_stream(const char* fname, int buffers, int samples) {
	ALLEGRO_AUDIO_STREAM *stream = al_load_audio_stream(fname, buffers, samples);
	if (!stream) 
		alert("Could not load audio stream %s", fname);
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
		alert("dup sprite sheet");
		return SheetList[sheet];
	}

	switch (sheet) {
		case SH_DEATH:
			SheetList[sheet] = new SpriteSheet("./res/sprites/death/death2.png", "./res/sprites/death/death2.json");
			break;
		default:
			break;
	}
	return SheetList[sheet];
}








/*SpriteSheet create_spritesheet_from_file(std::vector<std::string> fnames) {
  std::string cmd("./src/make_spritesheet ");
  for (auto s : fnames) {
  cmd += s;
  }

  std::system(cmd.c_str());

  return SpriteSheet(fnames.back().c_str(), "TODO get replace extension with .json");
  }*/ 
