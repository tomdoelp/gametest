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

/* $$SPRITE_SHEET_ENUM_LIST */
typedef enum SHNAME { 
	SH_DEATH, 
	SH_SHADOW, 
	SH_DUMMY, 
	SH_CASTLE_PROPS, 
	SH_NOSEMAN,
	SH_NUM 
} SheetName;
class SheetManager{
	public:
		SheetManager();
		~SheetManager();

		static SpriteSheet *get_sheet(SheetName sheet);
		static void cleanup();
	protected:
		static std::vector<SpriteSheet *> SheetList;

};

/* $$MUSIC_ENUM_LIST */
typedef enum MUSNAME { 
	MUS_TEST, 
	MUS_BATTLE_END,
	MUS_BATTLE1,
	MUS_NUM 
} MusicName;
/* $$SAMPLE_ENUM_LIST */
typedef enum SNDNAME { 
	SND_ACCEPT, 
	SND_SELECT, 
	SND_REJECT, 
	SND_PAUSE, 
	SND_TICK, 
	SND_COLLECT, 
	SND_IGNITE, 
	SND_HIT,
	SND_BATTLE_START,
	SND_NUM 
} SoundName;

/* Unlike SpriteSheets, we're just going to load all sounds at the beginning. */
/* Hopefully it won't be a problem */
class SoundManager {
	public:
		SoundManager(unsigned int depth);
		~SoundManager();

		ALLEGRO_SAMPLE_ID play_sound(SoundName sound, float gain=1.0f, float speed=1.0f, float pan = ALLEGRO_AUDIO_PAN_NONE);
		ALLEGRO_AUDIO_STREAM *play_music(MusicName music, bool loop=true);
		void stop_music();
		void clear_sounds();
		void init();
	protected:
		unsigned int depth;
		ALLEGRO_VOICE *voice;
		ALLEGRO_MIXER *music_mixer;
		ALLEGRO_MIXER *sound_mixer;
		ALLEGRO_MIXER *master_mixer;
		std::vector<ALLEGRO_SAMPLE *> samples;
		ALLEGRO_AUDIO_STREAM *current_song;
};

#endif
