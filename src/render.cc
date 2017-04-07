#include "render.h"
#include "world.h"

View::View() {}
View::~View() {}

View::View(float w, float h, ALLEGRO_DISPLAY *d) : w(w), h(h), display(d) {
	buffer = al_create_bitmap(w,h);

	scale = MIN(WINDOW_W / w, WINDOW_H / h);

	scale_w = SCREEN_W * scale;
	scale_h = SCREEN_H * scale;
	scale_x = (al_get_display_width(display) - scale_w) / 2;
	scale_y = (WINDOW_H - scale_h) / 2;	

	focus = NULL;

	dispw = al_get_display_width(display);
	disph = al_get_display_height(display);

}

void View::set_focus(PhysicalObj *o) {
	focus = o;
}

float View::get_x() {
	if (!focus) {
		float sc = get_scale(dispw, disph);
		return dispw/2-(w * sc)/2;
	}
	else {
		return focus->get_x() - w/2;
	}
}
float View::get_y() {
	if (!focus) {
		float sc = get_scale(dispw, disph);
		return disph/2-(h * sc)/2;
	}
	else {
		return focus->get_y() - h/2;
	}
}



float View::get_w() const { return w; }
float View::get_h() const { return h; }

ALLEGRO_BITMAP *View::get_buffer() { return buffer; }

float View::get_scale(float dispw, float disph) {
	if (screen_scale < 1) {
		scale_w = (float) dispw / w;
		scale_h = (float) disph / h;
		return scale_w < scale_h ? scale_w : scale_h;
	} else {
		return screen_scale;
	}
}

Box View::get_view_box() { return Box(get_x(),get_y(),w,h); }

Renderer::Renderer() {}
Renderer::~Renderer() {}
Renderer::Renderer(ALLEGRO_DISPLAY *display, View &v) : display(display), v(v) {
	scale_shader = create_scale_shader();
	temp_buffer = al_create_bitmap(v.get_w(), v.get_h());
}

void Renderer::register_visible(VisibleObj *o) {
	//visibles.insert(visibles.begin() + o->get_id(), o);
	visibles.push_back(o);
}
void Renderer::destroy_visible(Obj *o) {
 	for (unsigned int i = 0; i<visibles.size(); i++) {
		if (*(visibles[i]) == *o) {
			std::swap(visibles[i], visibles.back());
			visibles.pop_back();
			i--;
		}
	}
}

Vec2f Renderer::window2world(float x, float y){
	float dispw = al_get_display_width(display);
	float disph = al_get_display_height(display);
	float scale = v.get_scale(dispw, disph);
	return Vec2f(
			(x - (dispw/2-(v.get_w()*scale/2)))/scale + v.get_x(),
			(y - (disph/2-(v.get_h()*scale/2)))/scale + v.get_y()
	);
}



/* Thanks to SiegeLord, from https://www.allegro.cc/forums/thread/612318 */
ALLEGRO_SHADER *Renderer::create_scale_shader() {
	ALLEGRO_SHADER* shader = al_create_shader(ALLEGRO_SHADER_AUTO);
	if(!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER))) {
		printf("%s\n", al_get_shader_log(shader));
		return NULL;
	}
	if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, al_get_shader_platform(shader) == ALLEGRO_SHADER_GLSL ? "shaders/coverage_pixel.glsl" : "shaders/coverage_pixel.hlsl")) {
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
	al_set_target_bitmap(temp_buffer);
	al_draw_bitmap(v.get_buffer(), 0.0f, 0.0f, 0);

/*	std::vector<VisibleObj*> row_objs; */
	al_set_target_bitmap(v.get_buffer());

	al_identity_transform(&trans);
	al_translate_transform(&trans, -v.get_x(), -v.get_y());
	al_use_transform(&trans);

	al_clear_to_color(al_map_rgb(0,0,0));

	Box vbox = v.get_view_box();

	/* Draw the entire background layer first */
	m.draw_layer_region(0,0,LAYER_BACKGROUND, vbox);

	depth_sort();
	int row = 0;
	int tileh = m.tileh;

	/* Don't draw overworld objects during a battle */
	if (world->mode == World::MODE_BATTLE) {
		m.draw_layer_region(0,0,LAYER_MIXED, vbox);
	}
	else {
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
	}
	/* draw the rest of the mixed layer */
	m.draw_layer_region_from_row(0,0,row,LAYER_MIXED, vbox);


	/* draw the entire forground layer afterwards */
	m.draw_layer(0,0,LAYER_FOREGROUND);

	/* Draw the overlay, if there should be one. */
	switch(world->mode){
		case World::MODE_PAUSE:
			world->pmenu.draw(v.get_x()+v.get_w()/2, v.get_y()+v.get_h()/2);
			break;
		case World::MODE_TEXT:
			world->textbox.draw(v.get_x(), v.get_y());
			break;
		case World::MODE_BATTLE:
			/* world->battle.draw(v.get_x(), v.get_y()) */
			break;
		default:
			break;
	}

/*	float a = 0.5; */
/*	al_draw_tinted_bitmap(temp_buffer, al_map_rgba_f(a,a,a,a), v.get_x(), v.get_y(), 0); */

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

