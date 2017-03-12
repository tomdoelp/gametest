#include "render.h"

Renderer::Renderer() {}
Renderer::~Renderer() {}
Renderer::Renderer(ALLEGRO_DISPLAY *display, View &v) : display(display), v(v) {
	scale_shader = create_scale_shader();
}

void Renderer::register_visible(VisibleObj *o) {
	visibles.insert(visibles.begin() + o->get_id(), o);
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
	
void Renderer::set_view_focus(PhysicalObj *o) {
	v.set_focus(o);
}

void Renderer::render(Map &m) {
	std::vector<VisibleObj*> row_objs; /* objs on the same row */
	al_set_target_bitmap(v.get_buffer());

	al_identity_transform(&trans);
	al_translate_transform(&trans, -v.get_x(), -v.get_y());
	al_use_transform(&trans);

	if (paused) {
		al_draw_text(font, al_map_rgb(255,255,255), v.get_x()+v.get_w()/2, v.get_y()+v.get_h()/2, ALLEGRO_ALIGN_CENTRE, "PAUSED");
	}
	else {
		al_clear_to_color(al_map_rgb(0,0,0));

		Box vbox = v.get_view_box();

		/* Draw the entire background layer first */
		m.draw_layer_region(0,0,LAYER_BACKGROUND, vbox);

		depth_sort();
		int row = 0;
		int tileh = m.tileh;
		for (auto &o : visibles) {
			int objrow = o->depth / tileh;
			if (row < objrow) {
				for (int i = row; i <= objrow; i++) {
					m.draw_row_region(0,0,i,LAYER_MIXED, vbox);
				}
				o->draw();
				row = objrow;
			} else {
				o->draw();
			}
		}
		/* draw the rest of the mixed layer */
		m.draw_layer_region_from_row(0,0,row,LAYER_MIXED, vbox);



		/* draw the entire forground layer afterwards */
		m.draw_layer(0,0,LAYER_FOREGROUND);
	}

	al_set_target_backbuffer(display);
	al_clear_to_color(al_map_rgb(0,0,0));

	float dispw = al_get_display_width(display);
	float disph = al_get_display_height(display);
	float scale = v.get_scale(dispw, disph);
	float vw = v.get_w();
	float vh = v.get_h();
	if (scale_shader) {
		al_use_shader(scale_shader);
		al_set_shader_float("bitmap_width", vw);
		al_set_shader_float("bitmap_height", vh);
		al_set_shader_float("x_scale", scale);
		al_set_shader_float("y_scale", scale);
	} else {
		al_use_shader(NULL);
	}
	al_draw_scaled_bitmap(
			v.get_buffer(), 
			0, 
			0, 
			vw, 
			vh, 
			dispw/2-(vw*scale)/2, 
			disph/2-(vh*scale)/2, 
			scale * vw, 
			scale * vh, 
			0);
	/*dispw/2-(vw * scale)/2, 
	  disph/2-(vh * scale)/2,*/ 

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

