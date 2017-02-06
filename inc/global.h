#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

extern bool done;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* timer;
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_FONT *font;

extern bool key[4];

#endif
