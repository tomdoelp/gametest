#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

bool done;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_TIMER* timer;
ALLEGRO_DISPLAY* display;
ALLEGRO_FONT *font;

bool key[4] = {false, false, false, false};

#endif
