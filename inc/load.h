#ifndef LOAD_H
#define LOAD_H

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
typedef enum SHNAME { SH_DEATH, SH_SHADOW, SH_DUMMY, SH_NUM } SheetName;
		SheetManager();
		~SheetManager();

		static SpriteSheet *get_sheet(SheetName sheet);
		static void clear_sheets();
	protected:
		static std::vector<SpriteSheet *> SheetList;

};

/* Unlike SpriteSheets, we're just going to load all sounds at the beginning. */
/* Streams could go either way */
class SoundManager {
	public:
typedef enum MUSNAME { MUS_TEST, MUS_NUM } MusicName;
typedef enum SNDNAME { SND_ACCEPT, SND_SELECT, SND_REJECT, SND_PAUSE, SND_NUM } SoundName;
		SoundManager();
		~SoundManager();

		void play_music(MusicName music);
		void stop_music();
		void clear_sounds();
	protected:
		std::vector<ALLEGRO_AUDIO_STREAM *> MusicStreams;
};

#endif
