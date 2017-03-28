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

	const char *fnames[] = {
		"./res/samples/okdesuka.wav",
/*		"./res/samples/accept.wav", */
		"./res/samples/select.wav",
		"./res/samples/reject.wav",
		"/home/tom/sounds/earthbound/itemget1.wav",
/*		"./res/samples/pause.wav", */
		"./res/samples/tick.wav",
		"./res/samples/collect.wav"
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

ALLEGRO_AUDIO_STREAM *SoundManager::play_music(MusicName music) {
	const char *fnames[] = {
		"./res/music/offbeat.xm"
	};
	if (current_song)
		al_destroy_audio_stream(current_song);

	current_song = load_stream(fnames[music]);
	if (current_song) {
		al_attach_audio_stream_to_mixer(current_song, music_mixer);
		al_set_audio_stream_playmode(current_song, ALLEGRO_PLAYMODE_LOOP);
		al_set_audio_stream_gain(current_song, 0.7f);
		al_set_audio_stream_playing(current_song, true);
	}

	return current_song;
}
ALLEGRO_SAMPLE_INSTANCE *SoundManager::play_sound(SoundName sound, float pan) {
	al_play_sample(samples[sound], 1.0f, pan, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
	return NULL;
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
	al_drain_audio_stream(current_song); 
	al_destroy_audio_stream(current_song);

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
