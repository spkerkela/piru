#include <SDL2/SDL.H>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MENU_ITEM_COUNT 4

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;
bool init_SDL() { return SDL_Init(SDL_INIT_EVERYTHING) >= 0; }

void start_menu_music() { printf("Ominous music playing..\n"); }

void stop_music() { printf("Music stopped..\n"); }

enum MAIN_MENU_SELECTIONS
{
  START_GAME,
  SHOW_INTRO,
  CREDITS,
  QUIT,
  MAIN_MENU_ITEM_COUNT
};

bool game_init()
{
  printf("Started game..\n");
  printf("Woah, that was quick, game over!\n");
  return false;
}

void show_intro()
{
  printf("Amazing intro with cool effects.. Wow!\n");
}

void show_credits()
{
  printf("Piru was made by Simo-Pekka Kerkelä!\n");
}

bool select_main_menu_item(enum MAIN_MENU_SELECTIONS selection)
{
  switch (selection)
  {
  case START_GAME:
    return game_init();
  case SHOW_INTRO:
    show_intro();
    return false;
  case CREDITS:
    show_credits();
    return false;
  case QUIT:
    return true;
  default:
    return false;
  }
}

int main_menu()
{
  bool quit = false;
  SDL_Event e;
  int selected = START_GAME;
  char *menu_items[MENU_ITEM_COUNT] = {"Start Game", "Show Intro", "Credits",
                                       "Quit"};

  start_menu_music();
  while (!quit)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      // User requests quit
      if (e.type == SDL_QUIT)
      {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
          selected--;
          if (selected < 0)
          {
            selected = MAIN_MENU_ITEM_COUNT - 1;
          }
          break;
        case SDLK_DOWN:
          selected++;
          if (selected >= MAIN_MENU_ITEM_COUNT)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          quit = select_main_menu_item(selected);
          break;
        default:
          break;
        }
        printf("%s\n", menu_items[selected]);
      }
    }
  }
  stop_music();
  return 0;
}

int main(int argc, char const *argv[])
{
  if (!init_SDL())
  {
    printf("%s", SDL_GetError());
    return 1;
  }

  gWindow =
      SDL_CreateWindow("Piru", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       640, 480, SDL_WINDOW_SHOWN);

  if (!gWindow)
  {
    printf("%s", SDL_GetError());
    return 1;
  }
  gScreenSurface = SDL_GetWindowSurface(gWindow);
  if (!gScreenSurface)
  {
    printf("%s", SDL_GetError());
    return 1;
  }

  srand(SDL_GetTicks());

  printf("Hello Piru\n");
  printf("%d\n", rand());

  main_menu();

  SDL_DestroyWindow(gWindow);
  SDL_Quit();
  return 0;
}
