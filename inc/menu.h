#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

#include "global.h"
#include "utils.h"
#include "tweens.h"

class World;
class Overlay {
	public:
		Overlay();
		virtual ~Overlay();
		virtual void draw(float x=0.0f, float y=0.0f);
		virtual void update();
};

class Dialog : public Overlay {
	public:
		Dialog();
		~Dialog();
		void update();
		void draw(float x, float y);
		void script_advance();
		void script_goto(int script_position);

		void set_script(std::vector<std::string> script);

	private:
		int script_position = 0;
		std::vector<std::string> script;
		std::vector<std::string> interrupted_script;
		std::queue<GenericTween*> tweens;
};

class Textbox : public Overlay {
	friend class World;
	public:
		Textbox();
		virtual ~Textbox();
		virtual void draw(float x=0.0f, float y=0.0f);
		virtual void update();
		virtual void set_msg(const char* msg);
	protected:
		bool active;
		int pos, part, posmax, partmax;
/*		std::string msg; */
		std::vector<std::string> msgparts;
		int timer;
		const float window_w = SCREEN_W - 16.0f;
		const float window_h = 59.0f;
		const int linelen = 36;
		const int linemax = 3;
		Box background;
		World *world;

		std::vector<std::string> process_msg(const char* msg);
};

class Pause : public Overlay {
	friend class World;
	public:
		Pause();
		virtual ~Pause();
		virtual void draw(float centerx, float centery);
		virtual void update();
	protected:
		typedef enum OPTIONS {OP_QUIT, OP_RESUME, OP_MAX = OP_RESUME, OP_MIN = OP_QUIT} Options;
		int ops;
		World *world;
		int timer;
		bool interact;
};



#endif
