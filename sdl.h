#ifndef _PIRU_SDL_H
#define _PIRU_SDL_H
#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.H>
#include <SDL2/SDL_render.H>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#include <stdbool.h>
#include "constants.h"

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;

bool init();
bool init_SDL();
bool load_font();

#endif