#ifndef _LOAD_H_
#define _LOAD_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <fstream>

#include "global.h"
#include "json.h"

using json = nlohmann::json;

class SpriteSheet;

ALLEGRO_BITMAP *load_bitmap(const char* fname);
json load_json(const char* fname);
ALLEGRO_SAMPLE *load_sound(const char* fname);
ALLEGRO_AUDIO_STREAM *load_stream(const char* fname, int buffers=4, int samples=2048);

class SheetManager{
	public:
		typedef enum SHNAME { SH_DEATH, SH_NUM } SheetName;
		SheetManager();
		~SheetManager();

		static SpriteSheet *get_sheet(SheetName sheet);
		static void clear_sheets();
	protected:
		static std::vector<SpriteSheet *> SheetList;

};

#endif
