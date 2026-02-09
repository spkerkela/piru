#ifndef _MOCK_SDL_H
#define _MOCK_SDL_H

#include <stdint.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Surface SDL_Surface;

typedef struct {
    int x, y, w, h;
} SDL_Rect;

typedef struct {
    uint8_t r, g, b, a;
} SDL_Color;

typedef enum {
    SDL_BLENDMODE_NONE = 0,
    SDL_BLENDMODE_BLEND = 1
} SDL_BlendMode;

static inline uint32_t SDL_GetTicks(void) { return 0; }
static inline const char *SDL_GetError(void) { return "mock"; }
static inline int SDL_SetTextureAlphaMod(SDL_Texture *t, uint8_t a) { (void)t; (void)a; return 0; }
static inline int SDL_SetTextureBlendMode(SDL_Texture *t, SDL_BlendMode m) { (void)t; (void)m; return 0; }
static inline int SDL_RenderCopy(SDL_Renderer *r, SDL_Texture *t, const SDL_Rect *s, const SDL_Rect *d) {
    (void)r; (void)t; (void)s; (void)d; return 0;
}
static inline SDL_Texture *SDL_CreateTextureFromSurface(SDL_Renderer *r, SDL_Surface *s) {
    (void)r; (void)s; return (SDL_Texture*)0;
}
static inline void SDL_FreeSurface(SDL_Surface *s) { (void)s; }
static inline int SDL_QueryTexture(SDL_Texture *t, uint32_t *f, int *a, int *w, int *h) {
    (void)t; (void)f; (void)a;
    if (w) *w = 0;
    if (h) *h = 0;
    return 0;
}
static inline void SDL_DestroyTexture(SDL_Texture *t) { (void)t; }

/* Provide min/max macros for portability (used in dungeon.c) */
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#endif
