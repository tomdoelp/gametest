#ifndef __LOAD_H__
#define __LOAD_H__

#include <allegro5/allegro.h>
#include <fstream>

#include "global.h"
#include "json.h"
#include "sprite.h"

using json = nlohmann::json;

ALLEGRO_BITMAP *load_bitmap(const char* fname);
json load_json(const char* fname);
SpriteSheet create_spritesheet_from_file(std::vector<std::string> fnames);

#endif
