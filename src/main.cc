#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <allegro5/allegro.h>
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
bool key[ALLEGRO_KEY_MAX];


ALLEGRO_SAMPLE *sample = NULL;
std::list<SolidObj> solids;
void register_solid(SolidObj o) {
	solids.push_back(o);
}

std::list<VisibleObj> visibles;
void register_visible(VisibleObj o) {
	visibles.push_back(o);
	visibles.sort();
}

void abort(const char* message) {
	fprintf(stderr,"%s\n", message);
	exit(1);
}

void init() {
	/* fill keyboard array with false */
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
		key[i] = false;
	}
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
	al_set_window_title(display, "o b o m a  does the  m a m b o");

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
/*	al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);  */

	/* create a player object */
	Player p(SCREEN_W/2, SCREEN_H/2, 32.0, 32.0, 0); 
	register_visible(p);

	/* #BuildTheWall */
	Wall w (320, 320, 64.0, 64.0, 1); 
	register_visible(w);
	register_solid(w);

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
			key[event.keyboard.keycode] = true;
			//get_input();
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			key[event.keyboard.keycode] = false;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			al_clear_to_color(al_map_rgb(0,0,0));
			p.draw(); 
			w.draw(); 

/*			printf("Start drawing\n");
			for (std::list<VisibleObj>::iterator iterator = visibles.begin(), end = visibles.end();
					iterator != end;
					++iterator) {
				iterator->draw();
				printf("depth %d\n", iterator->getDepth());
				
			}*/

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
