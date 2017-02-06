#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../inc/global.h"
#include "../inc/Obj.h"

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
bool key[4] = {false, false, false, false};



ALLEGRO_SAMPLE *sample = NULL;

void abort(const char* message) {
	fprintf(stderr,"%s\n", message);
	exit(1);
}

void init() {
	if (!al_init())
		abort("Failed to initialize allegro");

	if (!al_install_keyboard())
		abort("Failed to install keyboard");

	timer = al_create_timer(1.0 / FPS);
	if (!timer)
		abort("Failed to create timer");

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
		abort("Failed to create display");

	event_queue = al_create_event_queue();
	if (!event_queue)
		abort("Failed to create event queue");

	if (!al_install_audio())
		abort("Failed to install audio");
	if (!al_init_acodec_addon())
		abort("Failed to initialize audio codecs");
	if (!al_reserve_samples(1))
		abort("Failed to reserve samples");

	if (!al_init_font_addon())
		abort("Failed to initialize font addon");
	if (!al_init_ttf_addon())
		abort("Failed to initialize ttf addon");

	font = al_load_ttf_font("/home/tom/school/cse350/APHont/APHont-Regular_q15c.ttf",72,0);
	if (!font)
		abort("Failed to load font");

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

	if (sample)
		al_destroy_sample(sample);
}

void game_loop() {
	bool redraw = true;
	al_start_timer(timer);

	/* load and play a sound */
	sample = al_load_sample("ring.ogg");
	if (!sample)
		fprintf(stderr, "Error loading sound file\n");
	al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); 

	/* create a player object */
	Player p (SCREEN_W/2, SCREEN_H/2, 36, 72); 

	while (!done) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			p.update();
			//update_logic();
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				done = true;
			}
			switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_UP: 
					key[KEY_UP] = true;
					break;
				case ALLEGRO_KEY_DOWN: 
					key[KEY_DOWN] = true;
					break;
				case ALLEGRO_KEY_LEFT: 
					key[KEY_LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT: 
					key[KEY_RIGHT] = true;
					break;
			}
			//get_input();
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_UP: 
					key[KEY_UP] = false;
					break;
				case ALLEGRO_KEY_DOWN: 
					key[KEY_DOWN] = false;
					break;
				case ALLEGRO_KEY_LEFT: 
					key[KEY_LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT: 
					key[KEY_RIGHT] = false;
					break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0,0,0));
			p.draw();
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
