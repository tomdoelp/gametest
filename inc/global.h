#ifndef _GLOBAL_H_
#define _GLOBAL_H_

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

const float FPS = 60;
const int WINDOW_W = 1024;
const int WINDOW_H = 768;
const int SCREEN_W = 320;
const int SCREEN_H = 240;

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
extern ALLEGRO_FONT *font;
extern int screen_scale;

extern bool key[ALLEGRO_KEY_MAX];
extern int key_map[ALLEGRO_KEY_MAX];
extern bool key_press[ALLEGRO_KEY_MAX];

extern bool paused;

void abort(const char *format, ...);
/*void alert(const char *format, ...); */

bool kmap(const int key_code);

#endif
