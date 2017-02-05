#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../inc/global.h"

class Object {
	public:
		Object(float x, float y, int w, int h) : x(x), y(y), w(w), h(h) { }
/*	private: */
		float x;
		float y;
		int w;
		int h;
		float hspeed = 0;
		float vspeed = 0;

		void update() {
			if (x + hspeed > (w/2) && x + hspeed < SCREEN_W - (w/2))
				x += hspeed;
			else 
				hspeed = 0;

			if (y + vspeed > 0 && y + vspeed < SCREEN_H - h)
				y += vspeed;
			else
				vspeed = 0;
		}
		
		void draw() {
			al_draw_text(font, al_map_rgb(255,255,255), x, y, ALLEGRO_ALIGN_CENTRE, "#");
		}
};



struct Player: public Object {
	public:
		Player(float x, float y, int w, int h) : Object(x, y, w, h) { }

		void update() {
			if (key[KEY_UP]) {
				vspeed = -5;
			} else if (key[KEY_DOWN]) {
				vspeed = 5;
			} else
				vspeed = 0;

			if (key[KEY_UP] && key[KEY_DOWN])
				vspeed = 0;

			if (key[KEY_LEFT]) {
				hspeed = -5;
			} else if (key[KEY_RIGHT]) {
				hspeed = 5;
			} else
				hspeed = 0;

			if (key[KEY_LEFT] && key[KEY_RIGHT])
				hspeed = 0;

			Object::update();
		}

		void draw() {
			al_draw_text(font, al_map_rgb(255,255,255), x, y, ALLEGRO_ALIGN_CENTRE, "@");
		}
};


void abort(const char* message) {
	printf("%s\n", message);
	exit(1);
}

void init() {
	if (!al_init()) {
		abort("Failed to initialize allegro");
	}

	if (!al_install_keyboard()) {
		abort("Failed to install keyboard");
	}

	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		abort("Failed to create timer");
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		abort("Failed to create display");
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		abort("Failed to create event queue");
	}

	if (!al_init_font_addon()) {
		abort("Failed to do font thing");
	}
	if (!al_init_ttf_addon()) {
		abort("Failed to do ttf");
	}

	font = al_load_ttf_font("/home/tom/school/cse350/APHont/APHont-Regular_q15c.ttf",72,0);
	if (!font) {
		abort("Failed to load font");
	}

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
}

void game_loop() {
	bool redraw = true;
	al_start_timer(timer);

	Player p (100, 100, 72, 72); 

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
			//update graphics();
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
