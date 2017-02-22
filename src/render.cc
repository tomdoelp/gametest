#include "render.h"

Renderer::Renderer(ALLEGRO_DISPLAY *display, View &v) : display(display), v(v) {}


void Renderer::register_visible(VisibleObj *o) {
	visibles.push_back(o);
}

void Renderer::render(Map &m) {
	al_set_target_bitmap(v.get_buffer());
	al_clear_to_color(al_map_rgb(64,64,64));
	
	m.draw_layer(0);

	depth_sort();
	for (auto &x : visibles) {
		x->draw();
	}

	m.draw_layer(1);

	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));

	float dispw = al_get_display_width(display);
	float disph = al_get_display_height(display);
	float scale = v.get_scale(dispw, disph);
	al_draw_scaled_bitmap(v.get_buffer(), 0, 0, SCREEN_W, SCREEN_H, dispw/2-(SCREEN_W * scale)/2, disph/2-(SCREEN_H * scale)/2, scale * SCREEN_W, scale * SCREEN_H, 0);
}

void Renderer::depth_sort(){
	for (int i = 1, n = visibles.size(); i < n; i++) {
		int j = i;
		VisibleObj *temp = visibles[i];
		while (j>0 && temp->depth < visibles[j-1]->depth) {
			visibles[j] = visibles[j-1];
			j--;
		}
		visibles[j] = temp;
	}
}

