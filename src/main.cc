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

// maybe a forward list? :w
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
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
		abort("Failed to create display");
	al_set_window_title(display, "o b o m a  does the  m a m b o");

	/* Events */
	event_queue = al_create_event_queue();
	if (!event_queue)
		abort("Failed to create event queue");

	/* Image formats */
	if (!al_init_image_addon())
		abort("Failed to initialize image addon");

	/* Audio (probably won't stick with allegro here) */
	if (!al_install_audio())
		abort("Failed to install audio");
	if (!al_init_acodec_addon())
		abort("Failed to initialize audio codecs");
	if (!al_reserve_samples(1))
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

	if (sample)
		al_destroy_sample(sample);
}

void game_loop() {
	bool redraw = true;
	al_start_timer(timer);

	/* load and a sound */
	sample = al_load_sample("./res/ring.ogg");
	if (!sample)
		fprintf(stderr, "Error loading sound file\n");
/*	al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);  */
	
	/* sheet for animation */
	ALLEGRO_BITMAP *sheet_mrsaturn = load_bitmap("./res/saturn_strip.png");
	Sprite spr_mrsaturn(sheet_mrsaturn, 18, 23, 2);

	/* create a spritesheet */
/*	SpriteSheet s("./res/saturnfishing-sheet.png","./res/saturnfishing.json");  */

	/* create a sprite */
	spr_mrsaturn.sprite_center_origin(false);

	/* create a player object */
	Player p(SCREEN_W/2, SCREEN_H/2, 32.0, 32.0, 0, &spr_mrsaturn);

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
			al_clear_to_color(al_map_rgb(64,64,64));
			p.draw(); 
			w.draw(); 

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
