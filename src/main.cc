#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>

#include "global.h"
#include "obj.h"
#include "sprite.h"
#include "load.h"
#include "level.h"
#include "render.h"
#include "world.h"

/* externs from global.h */
bool done;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_TIMER* timer;
ALLEGRO_DISPLAY* display;
ALLEGRO_FONT *font_tamsyn;
ALLEGRO_FONT *font_tamsynb;
D(ALLEGRO_FONT *debug_font;)
bool key[ALLEGRO_KEY_MAX];
bool key_press[ALLEGRO_KEY_MAX];
int key_map[ALLEGRO_KEY_MAX];
int screen_scale = 0;
bool paused = false;

void init(int argc, char* argv[]) {
	/* ranomd seed */
	srand(time(NULL));


	/* fill keyboard array with false */
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
		key[i] = false;
		key_press[i] = false;
		key_map[i] = i;
	}

	/* Allegro 5 */
	if (!al_init())
		abort("Failed to initialize allegro");

	/* Keyboard */
	if (!al_install_keyboard())
		abort("Failed to install keyboard");

	/* Mouse */
	if (!al_install_mouse())
		abort("Failed to install mouse");

	/* Timer */
	timer = al_create_timer(1.0 / FPS);
	if (!timer)
		abort("Failed to create timer");

	/* Display */
	al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_GENERATE_EXPOSE_EVENTS | ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_OPENGL);
/*	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR); */
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	display = al_create_display(WINDOW_W, WINDOW_H);
	if (!display)
		abort("Failed to create display");

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
	if (!al_reserve_samples(8)) 
		abort("Failed to reserve samples"); 

	/* Set working directory */
	ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_append_path_component(path, "res");
	al_change_directory(al_path_cstr(path, '/')); 

	/* PhysFS archive */
	/*
	PHYSFS_init(argv[0]);
	if (!PHYSFS_addToSearchPath(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP), 1)) {
		abort("PhysFS failed");
	}
	al_set_physfs_file_interface();
	*/
	al_destroy_path(path);

	/* Fonts */
	if (!al_init_font_addon())
		abort("Failed to initialize font addon");
/*	int ranges[] = {32, 126}; */
/*	ALLEGRO_BITMAP *temp = al_load_bitmap("./res/fonts/Tamsyn12b.tga"); */
/*	font_tamsyn = al_grab_font_from_bitmap(temp, 1, ranges); */
	font_tamsyn = al_load_font("fonts/Tamsyn15.tga", 0, 0);
	font_tamsynb = al_load_font("fonts/Tamsyn15b.tga", 0, 0);
	D(debug_font = al_create_builtin_font();)
/*	al_destroy_bitmap(temp); */
	if (!font_tamsyn){
		LOG("Failed to load font");
		font_tamsyn = al_create_builtin_font();  
	}

	/* Event sources */
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
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

	if (font_tamsyn) 
		al_destroy_font(font_tamsyn);

	D(if (debug_font) al_destroy_font(debug_font);)

/*	snd.cleanup(); */
	SheetManager::cleanup();
}

void game_loop() {
	/* DEBUG  */

#if DEBUG
	double old_time = al_get_time(), fps = 0;
	int frames_done = 0;
#endif

	/* SCREEN STUFF */

	bool redraw = true;
	al_start_timer(timer);

	/* MAP AND WORLD */	
	View v(SCREEN_W, SCREEN_H, display);
	Renderer r(display, v);
	World world(&r);

	/* Load a map from a file */
	world.load_map("maps/dungeon1.tmx");
	world.sndmgr->play_music(MUS_TEST);  

	/* Events */
	while (!done) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		Vec2f mouse_coord = world.get_renderer()->window2world(event.mouse.x, event.mouse.y);

		switch(event.type){
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				done = true;
				break;

			case ALLEGRO_EVENT_DISPLAY_EXPOSE:
				redraw = true;
				break;

			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				/* destroy and recreate shaders? */
				al_acknowledge_resize(event.display.source);
				redraw = true;
				break;

			case ALLEGRO_EVENT_KEY_DOWN:
				/* update key array (probably slightly overkill tbh) */
				key[event.keyboard.keycode] = true;
				key_press[event.keyboard.keycode] = true;


#if DEBUG
				if (key[ALLEGRO_KEY_0]) {
					screen_scale = 0;
					redraw = true;
				}
				if (key[ALLEGRO_KEY_1]) {
					screen_scale = 1;
					redraw = true;
				}
				if (key[ALLEGRO_KEY_2]) {
					screen_scale = 2;
					redraw = true;
				}
				if (key[ALLEGRO_KEY_3]) {
					screen_scale = 3;
					redraw = true;
				}
				if (key[ALLEGRO_KEY_4]) {
					screen_scale = 4;
					redraw = true;
				}

				if (key[ALLEGRO_KEY_B]) {
					if (world.get_mode() == World::MODE_OVERWORLD)
						world.set_mode(World::MODE_BATTLE);
					else if (world.get_mode() == World::MODE_BATTLE)
						world.set_mode(World::MODE_OVERWORLD);
				}

				/*
				   if (key[ALLEGRO_KEY_PAD_PLUS]) {
				   al_set_audio_stream_speed(song, al_get_audio_stream_speed(song) + 0.1f);
				   }
				   if (key[ALLEGRO_KEY_PAD_MINUS]) {
				   al_set_audio_stream_speed(song, al_get_audio_stream_speed(song) - 0.1f);
				   }
				   */
				if (key[ALLEGRO_KEY_M]) {
					world.load_map("maps/bigtest2.tmx");
				}

				if (key_press[ALLEGRO_KEY_N]) {
					world.show_text("Lorem ipsum, yo. This is a test of the emergency broadcast system. Hopefully, this will work. I have to write a lot of text to make sure, so this is just some FILLER to make sure that this text is long enough to require multiple parts to display all of it!!!"); 
				}
#endif
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				LOG("click");
				world.create_visible<Dummy>(mouse_coord.get_x(), mouse_coord.get_y());

				break;

			case ALLEGRO_EVENT_KEY_UP:
				key[event.keyboard.keycode] = false;
				break;

			case ALLEGRO_EVENT_TIMER:

				redraw = true;
				world.update();

				for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
					key_press[i] = false;
				}

				break;
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			world.render();
#if DEBUG
			double game_time = al_get_time();
			if (game_time - old_time >= 1.0) {
				fps = frames_done / (game_time - old_time);
				frames_done = 0;
				old_time = game_time;
			}
			frames_done++;
			al_draw_textf(debug_font, al_map_rgb(0,255,0), 10, 10, 0, "FPS: %.2f", fps);
#endif

			al_flip_display();
		}
	}
}

int main(int argc, char* argv[]) {
	LOG("----------------\ngame start" << std::endl);
	init(argc, argv);
	game_loop();
	LOG(std::endl << "shutting down\n----------------");
	shutdown();

	return 0;
}
