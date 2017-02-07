#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define DEBUG 1

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
/*enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}; */

extern bool done;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* timer;
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_FONT *font;

extern bool key[ALLEGRO_KEY_MAX];

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
