#include "sdl2.h"
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

bool init() {
  if (!init_SDL()) {
    printf("%s", SDL_GetError());
    return false;
  }

  gWindow =
      SDL_CreateWindow("Piru", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!gWindow) {
    printf("%s", SDL_GetError());
    return false;
  }
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if (!gRenderer) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    return false;
  }
  if (TTF_Init() == -1) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
  if (!load_font()) {
    printf("SDL_ttf could not load font! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
  SDL_SetRelativeMouseMode(SDL_TRUE);
  return true;
}

bool init_SDL() { return SDL_Init(SDL_INIT_EVERYTHING) >= 0; }
