#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <errno.h>

#define DEBUG 1
#define DEBUG_DRAW 0

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#if defined(DEBUG) && DEBUG > 0
#	define LOG(x) do { std::cerr << x << std::endl; } while (0)
#else
#	define LOG(x)
#endif

#if defined(DEBUG) && DEBUG > 0
#	define LLOG(x) do { std::cerr << __FILE__ << ":" << __LINE__ << ":" << __func__ << ": " << x << std::endl; } while (0)
#else
#	define LLOG(x)
#endif	

#if defined(DEBUG) && DEBUG > 0
#	define D(x) x
#else
#	define D(x)
#endif

class SoundManager;
class SheetManager;

const float FPS = 60;
const int WINDOW_W = 1024;
const int WINDOW_H = 768;
const int SCREEN_W = 320;
const int SCREEN_H = 240;
const unsigned int AUDIO_DEPTH_GOOD = 44100;
const unsigned int AUDIO_DEPTH_SHIT = 11025;

/* Might not use these ? */
const int KEY_UP = ALLEGRO_KEY_UP;
const int KEY_DOWN = ALLEGRO_KEY_DOWN;
const int KEY_LEFT = ALLEGRO_KEY_LEFT;
const int KEY_RIGHT = ALLEGRO_KEY_RIGHT;
const int KEY_QUIT = ALLEGRO_KEY_ESCAPE;
const int KEY_PAUSE = ALLEGRO_KEY_ENTER;
const int KEY_ACCEPT = ALLEGRO_KEY_X;
const int KEY_REJECT = ALLEGRO_KEY_Z;

extern bool done;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* timer;
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_FONT *font_tamsyn12;
extern int screen_scale;

extern bool key[ALLEGRO_KEY_MAX];
extern int key_map[ALLEGRO_KEY_MAX];
extern bool key_press[ALLEGRO_KEY_MAX];

extern bool paused;

/*extern SoundManager snd; */
/* something is fucky with externs so let's try to avoid */

void abort(const char *format, ...);
/*void alert(const char *format, ...); */

bool kmap(const int key_code);
/*
const ALLEGRO_COLOR DB_BLACK = al_map_rgb(0, 0, 0);
const ALLEGRO_COLOR DB_MIDNIGHT = al_map_rgb(34, 32, 52);
const ALLEGRO_COLOR DB_DKVIOLET = al_map_rgb(69, 40, 60);
const ALLEGRO_COLOR DB_DKBROWN = al_map_rgb(102, 57, 49);
const ALLEGRO_COLOR DB_BROWN = al_map_rgb(143, 86, 59);
const ALLEGRO_COLOR DB_ORANGE = al_map_rgb(223, 113, 38);
const ALLEGRO_COLOR DB_LTORANGE = al_map_rgb(217, 160, 102);
const ALLEGRO_COLOR DB_PEACH = al_map_rgb(238, 195, 154);
const ALLEGRO_COLOR DB_YELLOW = al_map_rgb(251, 241, 54);
const ALLEGRO_COLOR DB_LTGREEN = al_map_rgb(153, 229, 80);
const ALLEGRO_COLOR DB_GREEN = al_map_rgb(106, 190, 48);
const ALLEGRO_COLOR DB_GREENYBLUE = al_map_rgb(55, 148, 110);
const ALLEGRO_COLOR DB_DKGREEN = al_map_rgb(75, 105, 47);
const ALLEGRO_COLOR DB_DKOLIVE = al_map_rgb(82, 75, 36);
const ALLEGRO_COLOR DB_DKBLAY = al_map_rgb(50, 60, 57);
const ALLEGRO_COLOR DB_BLVIOLET = al_map_rgb(63, 63, 116);
const ALLEGRO_COLOR DB_DKBLUE = al_map_rgb(48, 96, 130);
const ALLEGRO_COLOR DB_BLUE = al_map_rgb(91, 110, 225);
const ALLEGRO_COLOR DB_SKYBLUE = al_map_rgb(99, 155, 255);
const ALLEGRO_COLOR DB_TURQUOISE = al_map_rgb(95, 205, 228);
const ALLEGRO_COLOR DB_LTBLUE = al_map_rgb(203, 219, 252);
const ALLEGRO_COLOR DB_WHITE = al_map_rgb(255, 255, 255);
const ALLEGRO_COLOR DB_BLAY = al_map_rgb(155, 173, 183);
const ALLEGRO_COLOR DB_VIOLETGRAY = al_map_rgb(132, 126, 135);
const ALLEGRO_COLOR DB_GRAY = al_map_rgb(105, 106, 106);
const ALLEGRO_COLOR DB_DKGRAY = al_map_rgb(89, 86, 82);
const ALLEGRO_COLOR DB_PURPLE = al_map_rgb(118, 66, 138);
const ALLEGRO_COLOR DB_RED = al_map_rgb(172, 50, 50);
const ALLEGRO_COLOR DB_LIGHTISHRED = al_map_rgb(217, 87, 99);
const ALLEGRO_COLOR DB_PINK = al_map_rgb(215, 123, 186);
const ALLEGRO_COLOR DB_OLIVE = al_map_rgb(143, 151, 74);
const ALLEGRO_COLOR DB_DKYELLOW = al_map_rgb(138, 111, 48);
*/
#endif
