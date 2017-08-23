#ifndef DIALOG_H
#define DIALOG_H

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

class Dialog {
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

/* TODO a tree structure for scripts that allows branching and etc */

#endif
