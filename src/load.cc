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
		if (SheetList[i]){
			delete SheetList[i];
			SheetList[i] = NULL;
		}
	}
}

SpriteSheet *SheetManager::get_sheet(SheetName sheet) {
	if (SheetList.empty()) {
		for (int i = 0; i < SH_NUM; i++) { 
			SheetList.push_back(NULL); 
		} 
	}


	if (SheetList[sheet]){
		return SheetList[sheet];
	}

	/* ye gods this is going to be huge isn't it */
	/* $$SPRITE_SHEET_SWITCH */
	switch (sheet) {
		case SH_DEATH:
			SheetList[sheet] = new SpriteSheet("sprites/death/death2.png", "sprites/death/death2.json");
			break;
		case SH_SHADOW:
			SheetList[sheet] = new SpriteSheet("sprites/shadow.png", "sprites/shadow.json");
			break;
		case SH_DUMMY:
			SheetList[sheet] = new SpriteSheet("sprites/dummy/dummy2.png", "sprites/dummy/dummy2.json");
			break;
		case SH_CASTLE_PROPS:
			SheetList[sheet] = new SpriteSheet("sprites/props/castleprops.png", "sprites/props/castleprops.json");
			break;
		case SH_SPIRIT:
			SheetList[sheet] = new SpriteSheet("sprites/enemies/spirit.png", "sprites/enemies/spirit.json");
			break;
		default:
			break;
	}
	return SheetList[sheet];
}

void SheetManager::cleanup() {
	for (int i = 0, max = SheetList.size(); i < max; i++) {
		if (SheetList[i]){
			delete SheetList[i];
			SheetList[i] = NULL;
		}
	}
	/*
	   for (auto &s : SheetList)
	   delete s;
	   */
}


SoundManager::SoundManager(unsigned int depth) : depth(depth){}

void SoundManager::init(){
	voice = al_create_voice(depth, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	music_mixer = al_create_mixer(depth, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	sound_mixer = al_create_mixer(depth, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	master_mixer = al_create_mixer(depth, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	al_attach_mixer_to_mixer(music_mixer, master_mixer); 
	al_attach_mixer_to_mixer(sound_mixer, master_mixer); 
	al_attach_mixer_to_voice(master_mixer, voice); 
/*	al_set_default_mixer(sound_mixer); */

	/* $$SAMPLE_FILENAME_LIST */
	const char *fnames[] = {
		"samples/accept.wav", 
		"samples/select.wav",
		"samples/reject.wav",
		"/home/tom/sounds/earthbound/itemget1.wav",
/*		"samples/pause.wav", */
		"samples/tick2.wav",
		"samples/okdesuka.wav",
/*		"samples/collect.wav" */
		"samples/ignite.wav",
		"samples/hit.wav",
		"samples/battle_start.wav"
	};

	for(int i =0; i < SND_NUM; i++){
		samples.push_back(al_load_sample(fnames[i]));
	}

	/*
	   ALLEGRO_AUDIO_STREAM *song = load_stream( "./res/music/offbeat.xm", 8, 4096); 
	   if (song) {
	   al_attach_audio_stream_to_mixer(song, music_mixer);
	   al_set_audio_stream_playmode(song, ALLEGRO_PLAYMODE_LOOP);
	   al_set_audio_stream_gain(song, 0.7f);
	   al_set_audio_stream_playing(song, true); 
	   }
	   */
	current_song = NULL;
}

ALLEGRO_AUDIO_STREAM *SoundManager::play_music(MusicName music, bool loop) {
	ALLEGRO_PLAYMODE mode;
	if (loop)
		mode = ALLEGRO_PLAYMODE_LOOP;
	else
		mode = ALLEGRO_PLAYMODE_ONCE;
	
	const char *fnames[] = {
		"music/offbeat.xm",
		"music/victory.xm",
		"music/punkrockshow.xm"
	};
	if (current_song)
		al_destroy_audio_stream(current_song);

	current_song = load_stream(fnames[music]);
	if (current_song) {
		al_attach_audio_stream_to_mixer(current_song, music_mixer);
		al_set_audio_stream_playmode(current_song, mode);
		al_set_audio_stream_gain(current_song, 0.4f);
		al_set_audio_stream_playing(current_song, true);
	}

	return current_song;
}
ALLEGRO_SAMPLE_ID SoundManager::play_sound(SoundName sound, float gain, float speed, float pan) {
	ALLEGRO_SAMPLE_ID id;
	al_play_sample(samples[sound], gain, pan, speed, ALLEGRO_PLAYMODE_ONCE, &id);
	return id;
	/*
	ALLEGRO_SAMPLE_INSTANCE *instance = al_create_sample_instance(samples[sound]);
	if (instance) {
		al_attach_sample_instance_to_mixer(instance, sound_mixer);
		al_set_sample_instance_pan(instance, pan);
		al_play_sample_instance(instance);
	}

	return instance;
	*/
}

void SoundManager::stop_music(){
	al_set_audio_stream_playing(current_song, false);
	al_destroy_audio_stream(current_song);
}

SoundManager::~SoundManager() {
	if (current_song) {
		al_drain_audio_stream(current_song); 
		al_destroy_audio_stream(current_song);
		current_song = NULL;
	}

	for (auto& s : samples) {
		al_destroy_sample(s);
	}

	al_destroy_mixer(music_mixer);
	al_destroy_mixer(sound_mixer);
	al_destroy_mixer(master_mixer);
	al_destroy_voice(voice);
}








/*SpriteSheet create_spritesheet_from_file(std::vector<std::string> fnames) {
  std::string cmd("./src/make_spritesheet ");
  for (auto s : fnames) {
  cmd += s;
  }

  std::system(cmd.c_str());

  return SpriteSheet(fnames.back().c_str(), "TODO get replace extension with .json");
  }*/ 
