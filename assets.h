#ifndef _ASSETS_H
#define _ASSETS_H

#include <stdbool.h>
#include "sdl2.h"
#include "structs.h"

extern Animation gPlayerAnimations[256];
extern ImageAsset gImageAssets[256];
extern TTF_Font *gFont;

ImageAsset load_image_asset(char *fileName);

bool load_animations();
bool load_assets();
bool load_font();

#endif