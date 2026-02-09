#ifndef _MOCK_SDL_IMAGE_H
#define _MOCK_SDL_IMAGE_H

#include "SDL.h"

static inline SDL_Surface *IMG_Load(const char *file) { (void)file; return (SDL_Surface*)0; }
static inline const char *IMG_GetError(void) { return "mock"; }

#endif
