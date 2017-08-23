#include "menu.h"
#include "world.h"

Overlay::Overlay() {}
Overlay::~Overlay() {}
void Overlay::draw(float x, float y) {}
void Overlay::update() {}





Dialog::Dialog() {}
Dialog::~Dialog() {
	while (!tweens.empty()) {
		delete tweens.front();
		tweens.pop();
	}
}

void Dialog::script_advance() { script_position++; }
void Dialog::set_script(std::vector<std::string> script) { this->script = script; }
void Dialog::update() {
	if (tweens.size() > 0) {
		std::queue<GenericTween*> tempqueue;
		while (!tweens.empty()) {
			if (tweens.front()->step()) {
				tempqueue.push(tweens.front());
				tweens.pop();
			}
			else {
				delete tweens.front();
				tweens.pop();
			}
		}
		swap(tweens, tempqueue);
	}
}

/* x is center of screen, y is bottom */
void Dialog::draw(float x, float y) {
	float w = SCREEN_W;
	float h = SCREEN_H;
	draw_vertical_gradient_rect(x-w/2, y, w, h, al_map_rgba_f(0.0f, 0.0f, 0.0f, 0.0f), al_map_rgba_f(0.0f, 0.0f, 0.0f, 0.5f) );
}







Textbox::Textbox() {
	pos = 0;
	part = 0;
	posmax = 0;
	partmax = 0;
	active = false;
	timer = 0;
}
Textbox::~Textbox() {}

void Textbox::set_msg(const char *msg) {
/*	this->msg = msg; */
	pos = 0;
	part = 0;
	msgparts = process_msg(msg);
	posmax = msgparts[part].length();
	partmax = msgparts.size()-1;
	timer = 0;
	active = true;
}

std::vector<std::string> Textbox::process_msg(const char *msg) {
	std::vector<std::string> lines;
	std::vector<std::string> words = split(msg);
	int linecount = 0;
	int charcount = 0;

/*	LOG("String: " << msg); */

	std::string line = "";
	for (const auto& word : words) {
		int wordlen = word.length();
		if (wordlen > 0) {
			if (charcount == 0) {
/*				LOG("first word in a new part: " << word); */
				line += word;
				charcount += wordlen;
			}
			else if (charcount + wordlen < linelen) {
/*				LOG("new word: " << word); */
				line += " " + word;
				charcount += wordlen+1;
			}
			else {
/*				LOG("! new line !"); */
				linecount++;

				if (linecount < linemax) {
/*					LOG("first word in a new line but old part: " << word); */
					line += "\n" + word;
					charcount = wordlen;
				}
				else {
/*					LOG("\n\"" << line << "\"\n" ); */
					linecount = 0;
					lines.push_back(line);
					line.clear();
					line += word;
					charcount = wordlen;
				}
			}
			if (word == words.back()) {
/*				LOG("Any last words? " << word); */
				lines.push_back(line);
				line.clear();
				charcount = 0;
			}
		}
	}
return lines;
}


void Textbox::update() {
	if (active) {
		timer--;
		if (pos < posmax) {
			if (timer <= 0) {
				pos++;
				char c = msgparts[part][pos];
				float gain = 1.0f;
				if (kmap(ALLEGRO_KEY_X)) {
/*					world->sndmgr->play_sound(SND_TICK, gain/2 , 1.0f); */
					timer = 0;
				}
				else {
					switch (c) {
						case '\n':
						case ' ':
							timer = 0;
/*							if (pos < posmax) pos++; */
							break;
						case ',':
							world->sndmgr->play_sound(SND_TICK, gain, 0.95f);
							timer = 10;
							break;
						case '.':
							world->sndmgr->play_sound(SND_TICK, gain, 0.92f);
							timer = 30;
							break;
						case '!':
							world->sndmgr->play_sound(SND_TICK, gain , 0.90f);
							timer = 30;
							break;
						case '?':
							world->sndmgr->play_sound(SND_TICK, gain, 1.1f);
							timer = 30;
							break;
						default:
							world->sndmgr->play_sound(SND_TICK, gain, 0.95f + (float(rand()%8)/80.0f)); 
							timer = 3;
					}
				}
			}
		}


		else if (key_press[ALLEGRO_KEY_X]) {
			if (part < partmax){
				pos=0;
				part++;
				posmax = msgparts[part].length();
			}
			else
				active = false;
		}
	}
	else {
		if (world)
			world->stop_text();
		msgparts.clear();
	}
}



void Textbox::draw(float x, float y) {
	al_draw_filled_rectangle(x+8.0f, y+8.0f, x+8.0f+window_w, y+8.0f+window_h, al_map_rgb(0,0,0));
	/*	al_draw_rectangle(x+8.0f, y+8.0f, x+8.0f+window_w, y+8.0f+window_h, al_map_rgb(255,255,255), 0); */

	/*	Box b(x+8.0f, y+8.0f, window_w, window_h); */
	/*	b.draw(al_map_rgb(0,0,0)); */
	al_draw_multiline_textf(
			font_tamsyn, 
			al_map_rgb(255,255,255), 
			x+16.0f, 
			y+16.0f, 
			window_w-16.0f, 
			12.0f, 
			ALLEGRO_ALIGN_LEFT | ALLEGRO_ALIGN_INTEGER, 
			msgparts[part].substr(0,pos+1).c_str()
			);
	/*	if (pos == posmax) { */
	if (pos == posmax && (timer % 60)/2) { 
		al_draw_text(
				font_tamsyn,
				al_map_rgb(255,255,255), 
				x+window_w,
				y+window_h-8.0f,
				ALLEGRO_ALIGN_RIGHT | ALLEGRO_ALIGN_INTEGER,
				"*"
				);
	}
}



Pause::Pause() {
	ops = OP_MIN;
	world = NULL;
	timer = 0;
	interact = true;
}
Pause::~Pause() {}
void Pause::update() {
	if (key_press[ALLEGRO_KEY_ESCAPE]) 
		ops = OP_MIN; 
	if (key_press[ALLEGRO_KEY_X] || key_press[ALLEGRO_KEY_ENTER]){
		world->sndmgr->play_sound(SND_ACCEPT);
		switch (ops) {
			case OP_RESUME:
				ops = OP_MIN;
				if (world) world->set_mode(World::MODE_OVERWORLD);
				break;
			case OP_QUIT:
				done = true;
				break;
		}
	}
	if (key_press[ALLEGRO_KEY_Z]) {
		ops = OP_MIN; 
		world->sndmgr->play_sound(SND_REJECT);
		if (world) world->set_mode(World::MODE_OVERWORLD);
	}



	if (key_press[ALLEGRO_KEY_DOWN] || key_press[ALLEGRO_KEY_UP]) {
		interact = true;
		timer = 0;
	}
	if (interact) {
		if (key_press[ALLEGRO_KEY_DOWN]) {
			ops++;
			world->sndmgr->play_sound(SND_SELECT);
			interact = false;
			timer = 30;
		}
		else if (kmap(ALLEGRO_KEY_DOWN)) {
			ops++;
			world->sndmgr->play_sound(SND_SELECT);
			interact = false;
			timer = 5;
		}

		if (key_press[ALLEGRO_KEY_UP]) {
			ops--;
			world->sndmgr->play_sound(SND_SELECT);
			interact = false;
			timer = 30;
		}
		else if (kmap(ALLEGRO_KEY_UP)) {
			ops--;
			world->sndmgr->play_sound(SND_SELECT);
			interact = false;
			timer = 5;
		}
	}

	if (ops > OP_MAX)
		ops = OP_MIN;
	if (ops < OP_MIN)
		ops = OP_MAX;

	if (timer > 0)
		timer--;
	else
		interact = true;

}

void Pause::draw(float centerx, float centery) {
	al_draw_text(font_tamsynb, al_map_rgb(255,255,255), centerx, centery - 24, ALLEGRO_ALIGN_CENTRE, "Quit?");
	switch (ops) {
		case OP_RESUME:
			al_draw_multiline_text(font_tamsynb, al_map_rgb(255,255,255), centerx, centery, 340, 17, ALLEGRO_ALIGN_CENTER, "yes\n>no<");
			break;
		case OP_QUIT:
			al_draw_multiline_text(font_tamsynb, al_map_rgb(255,255,255), centerx, centery, 340, 17, ALLEGRO_ALIGN_CENTER, ">yes<\nno");
			break;
	}
}




