#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <errno.h>

#define DEBUG 1

const float FPS = 60;
const int WINDOW_W = 1024;
const int WINDOW_H = 768;
const int SCREEN_W = 320;
const int SCREEN_H = 240;
/*enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}; */

extern bool done;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* timer;
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_FONT *font;
extern int screen_scale;

extern bool key[ALLEGRO_KEY_MAX];

void abort(const char *format, ...);
void alert(const char *format, ...);

typedef enum SPRITE_ORIGIN { ORIGIN_DEFAULT, ORIGIN_CENTER_MIDDLE, ORIGIN_CENTER_BOTTOM, ORIGIN_CENTER_TOP } Origin;

/* void debug(const char *format, ...)
{
	if (!DEBUG)
		return;

	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr,format, argptr);
	fprintf(stderr,"\n");
}*/ 


#endif
