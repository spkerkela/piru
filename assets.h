#ifndef _ASSETS_H
#define _ASSETS_H

#include "sdl2.h"
#include "structs.h"
#include <stdbool.h>

extern Animation animations[256][256];
extern ImageAsset gImageAssets[MAX_SPRITES];
extern TTF_Font *gFont;

ImageAsset load_image_asset(char *fileName);

bool load_animations(ImageAsset spriteSheet, int columns, int rows,
                     enum ANIMATION animationIndex, int offset_x, int offset_y);
bool load_assets();
bool load_font();

#endif