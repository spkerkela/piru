#ifndef _MOCK_SDL_TTF_H
#define _MOCK_SDL_TTF_H

#include "SDL.h"

typedef struct _TTF_Font TTF_Font;

static inline const char *TTF_GetError(void) { return "mock"; }
static inline SDL_Surface *TTF_RenderText_Solid(TTF_Font *font, const char *text, SDL_Color fg) {
    (void)font; (void)text; (void)fg;
    return (SDL_Surface*)0;
}
static inline TTF_Font *TTF_OpenFont(const char *file, int ptsize) {
    (void)file; (void)ptsize;
    return (TTF_Font*)0;
}

#endif
