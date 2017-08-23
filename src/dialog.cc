#include "dialog.h"

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
