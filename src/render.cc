#include "render.h"

Renderer::Renderer(ALLEGRO_DISPLAY *display, View &v) : display(display), v(v) {
	scale_shader = create_scale_shader();
}

void Renderer::register_visible(VisibleObj *o) {
	visibles.push_back(o);
}

/* Thanks to SiegeLord, from https://www.allegro.cc/forums/thread/612318 */
ALLEGRO_SHADER *Renderer::create_scale_shader() {
	ALLEGRO_SHADER* shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER))) {
		printf("%s\n", al_get_shader_log(shader));
		return NULL;
	}
	if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, al_get_shader_platform(shader) == ALLEGRO_SHADER_GLSL ? "./res/shaders/coverage_pixel.glsl" : "./res/shaders/coverage_pixel.hlsl")) {
		printf("%s\n", al_get_shader_log(shader));
		return NULL;
	}
	if(!al_build_shader(shader)) {
		printf("%s\n", al_get_shader_log(shader));
		return NULL;
	}
	return shader;
}
	

void Renderer::render(Map &m) {
	std::vector<VisibleObj*> row_objs; /* objs on the same row */
	al_set_target_bitmap(v.get_buffer());
	al_clear_to_color(al_map_rgb(64,64,64));

	/* Draw the entire background layer first */
	m.draw_layer(LAYER_BACKGROUND);

	depth_sort();
	int row = 0;
	int tileh = m.tileh;
	for (auto &o : visibles) {
		int objrow = o->depth / tileh;
		if (row < objrow) {
			for (int i = row; i < objrow; i++) {
				m.draw_row(i,LAYER_MIXED);
			}
			o->draw();
			row = objrow;
		} else {
			o->draw();
		}
	}
	/* draw the rest of the mixed layer */
	m.draw_layer_from_row(row,LAYER_MIXED);



	/* draw the entire forground layer afterwards */
	m.draw_layer(LAYER_FOREGROUND);

	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));

	float dispw = al_get_display_width(display);
	float disph = al_get_display_height(display);
	float scale = v.get_scale(dispw, disph);
	if (scale_shader) {
		al_use_shader(scale_shader);
		al_set_shader_float("bitmap_width", v.get_w());
		al_set_shader_float("bitmap_height", v.get_h());
		al_set_shader_float("x_scale", scale);
		al_set_shader_float("y_scale", scale);
	} else {
		al_use_shader(NULL);
	}
	al_draw_scaled_bitmap(v.get_buffer(), 0, 0, SCREEN_W, SCREEN_H, dispw/2-(SCREEN_W * scale)/2, disph/2-(SCREEN_H * scale)/2, scale * SCREEN_W, scale * SCREEN_H, 0);

	al_use_shader(NULL);
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

