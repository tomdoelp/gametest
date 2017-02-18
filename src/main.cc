#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "global.h"
#include "obj.h"
#include "sprite.h"
#include "load.h"
#include "level.h"
#include "view.h"

//#include <list>
// std::list<int> L;
// L.push_back(1)
// L.insert(++L.begin(), 2);
// L.sort

/* externs from global.h */
bool done;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_TIMER* timer;
ALLEGRO_DISPLAY* display;
ALLEGRO_FONT *font;
bool key[ALLEGRO_KEY_MAX];


// maybe a forward list?
std::list<VisibleObj> visibles;
void register_visible(VisibleObj o) {
	visibles.push_back(o);
	visibles.sort();
}




void init() {
	/* fill keyboard array with false */
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
		key[i] = false;
	}

	/* Allegro 5 */
	if (!al_init())
		abort("Failed to initialize allegro");

	/* Keyboard */
	if (!al_install_keyboard())
		abort("Failed to install keyboard");

	/* Timer */
	timer = al_create_timer(1.0 / FPS);
	if (!timer)
		abort("Failed to create timer");

	/* Display */
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	display = al_create_display(WINDOW_W, WINDOW_H);
	if (!display)
		abort("Failed to create display");
	al_set_window_title(display, "B O I N G !");

	ALLEGRO_BITMAP *screen_buffer = al_create_bitmap(SCREEN_W, SCREEN_H);

	/* Events */
	event_queue = al_create_event_queue();
	if (!event_queue)
		abort("Failed to create event queue");

	/* Image formats */
	if (!al_init_image_addon())
		abort("Failed to initialize image addon");

	/* Audio */
	if (!al_install_audio())
		abort("Failed to install audio");
	if (!al_init_acodec_addon())
		abort("Failed to initialize audio codecs");
	if (!al_reserve_samples(4)) 
		abort("Failed to reserve samples"); 

	/* Fonts, ttf */
	if (!al_init_font_addon())
		abort("Failed to initialize font addon");
	if (!al_init_ttf_addon())
		abort("Failed to initialize ttf addon");
	font = al_load_ttf_font("/home/tom/school/cse350/APHont/APHont-Regular_q15c.ttf",72,0); 
	if (!font)
		abort("Failed to load font");

	/* Event sources */
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	done = false;
}

void shutdown() {
	if (timer)
		al_destroy_timer(timer);

	if (display) 
		al_destroy_display(display);

	if (event_queue)
		al_destroy_event_queue(event_queue);

	/* destoy mixers and such */
}

void game_loop() {
	View v(SCREEN_W, SCREEN_H);

	bool redraw = true;
	al_start_timer(timer);

	ALLEGRO_SAMPLE *sample = load_sound("./res/okdesuka.wav");
	al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	

	/* load and play an xm music file */
	ALLEGRO_VOICE *voice = al_create_voice( 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	ALLEGRO_MIXER *music_mixer = al_create_mixer( 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	/* and one for sound */
	ALLEGRO_MIXER *master_mixer = al_create_mixer( 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2); 
	al_attach_mixer_to_mixer(music_mixer, master_mixer); 
	al_attach_mixer_to_voice(master_mixer, voice); 

	/* buffer count and samples? ? ??? ? ?? ? ? ?  */
	ALLEGRO_AUDIO_STREAM *worry = load_stream( "/home/tom/songs/milkytracker/Worry.xm", 4, 2048); 

	if (worry) {
		al_attach_audio_stream_to_mixer(worry, music_mixer);
		al_set_audio_stream_playmode(worry, ALLEGRO_PLAYMODE_LOOP);
		al_set_audio_stream_playing(worry, true); 
	}

	/* sheet for animation */
	/*	ALLEGRO_BITMAP *sheet_mrsaturn = load_bitmap("./res/saturn_strip.png"); */
	/*	Sprite spr_mrsaturn(sheet_mrsaturn, 18, 23, 2); */

	/* create a spritesheet */
	SpriteSheet sh_saturn1("./res/saturn-sheet.png","./res/saturn-sheet.json");
	SpriteSheet sh_saturn("./res/saturnfishing-sheet.png","./res/saturnfishing-sheet.json");

	/* create a sprite */
	Sprite *spr_saturn1 = sh_saturn1[0];
	Sprite *spr_saturn2 = sh_saturn[0];
	spr_saturn1->sprite_center_origin(false);
	spr_saturn2->sprite_center_origin(false);

	/* create a player object */
	Player p(SCREEN_W/2, SCREEN_H/2, 32.0, 32.0, 0, spr_saturn1);
	Player p2(SCREEN_W/2+100, SCREEN_H/2, 32.0, 32.0, 0, spr_saturn2);

	/* create a wall object */
	Wall w (320, 320, 64.0, 64.0, 1); 

	while (!done) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			p.update();
			//update_logic();
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			/* update key array (probably slightly overkill tbh) */
			key[event.keyboard.keycode] = true;

			if (key[ALLEGRO_KEY_ESCAPE]) 
				done = true;
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			key[event.keyboard.keycode] = false;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			
			al_set_target_bitmap(v.get_buffer());
			al_clear_to_color(al_map_rgb(64,64,64));
			p.draw(); 
			p2.draw(); 
			w.draw(); 
			al_set_target_backbuffer(display);
			al_clear_to_color(al_map_rgb(0,0,0));
			Box b = v.get_scales();
			al_draw_scaled_bitmap(v.get_buffer(), 0, 0, SCREEN_W, SCREEN_H, b.getx(), b.gety(), b.getw(), b.geth(), 0);

			//update_graphics();
			al_flip_display();
		}
	}
}

int main(int argc, char* argv[]) {
	init();
	game_loop();
	shutdown();

	return 0;
}
