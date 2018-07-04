#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.H>
#else
#include <SDL2/SDL.H>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;

enum CHARACTER_CLASS
{
  MAGE,
  ROGUE,
  WARRIOR,
  CHARACTER_CLASS_COUNT
};

char *character_class_str[CHARACTER_CLASS_COUNT] = {
    "Mage",
    "Rogue",
    "Warrior"};

enum ARMOR_CLASS
{
  LIGHT,
  MEDIUM,
  HEAVY,
  ARMOR_CLASS_COUNT
};

typedef struct
{
  enum CHARACTER_CLASS character_class;
  enum ARMOR_CLASS armor_class;
  int level;
  int current_game_level;
  int world_x;
  int world_y;
} Player;

Player gPlayer;

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

enum GAME_START_MODE
{
  NEW_GAME,
  LOAD_GAME
};

bool load_file_exists()
{
  return false;
}

void create_new_character()
{
  printf("Create a new character\n");
  bool in_character_create = true;
  SDL_Event e;
  int selected = 0;
  while (in_character_create)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
          selected--;
          if (selected < 0)
          {
            selected = CHARACTER_CLASS_COUNT - 1;
          }
          break;
        case SDLK_DOWN:
          selected++;
          if (selected > CHARACTER_CLASS_COUNT - 1)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          in_character_create = false;
          break;
        }
      }
      printf("%s\n", character_class_str[selected]);
    }
  }
  gPlayer.level = 1;
  gPlayer.character_class = selected;
  gPlayer.armor_class = selected;
}

void select_character_menu()
{
  printf("Select character\n");
  const int CHAR_COUNT = 5;
  char *temp_characters[5] = {
      "Char1",
      "Char2",
      "Char3",
      "Char4",
      "Create New"};
  int selected = 0;
  SDL_Event e;
  bool in_character_select = true;
  while (in_character_select)
  {
    while (SDL_PollEvent(&e) != 0)
    {
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
          if (selected > CHAR_COUNT)
          {
            selected = 0;
          }
          break;
        case SDLK_RETURN:
          if (selected == CHAR_COUNT)
          {
            create_new_character();
          }
          else
          {
            // Load selected char
          }
          in_character_select = false;
          break;
        case SDLK_ESCAPE:
          return;
        default:
          break;
        }
        printf("%s\n", temp_characters[selected]);
      }
    }
  }
}

void init_player_position()
{
  gPlayer.world_x = 0;
  gPlayer.world_y = 0;
  gPlayer.current_game_level = 0;
}

void draw_and_blit()
{
  SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 255, 0, 255));
  SDL_UpdateWindowSurface(gWindow);
}

void run_game_loop(enum GAME_START_MODE start_mode)
{
  bool gameRunning = true;
  SDL_Event e;
  draw_and_blit();
  while (gameRunning)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT)
      {
        gameRunning = false;
      }
      else if (e.type == SDL_KEYDOWN)
      {
        switch (e.key.keysym.sym)
        {
        case SDLK_RETURN:
          gameRunning = false;
          break;
        }
      }
    }
    draw_and_blit();
  }
}

bool start_game(enum GAME_START_MODE start_mode)
{
  switch (start_mode)
  {
  case LOAD_GAME:
    select_character_menu();
    break;
  case NEW_GAME:
    create_new_character();
    break;
  }
  printf("level: %d class: %d\n", gPlayer.level, gPlayer.character_class);
  init_player_position();
  run_game_loop(start_mode);
  printf("Started game..\n");
  printf("Woah, that was quick, game over!\n");
  return false;
}

bool game_init()
{
  stop_music();
  if (load_file_exists())
  {
    return start_game(LOAD_GAME);
  }
  else
  {
    return start_game(NEW_GAME);
  }
}

void show_intro()
{
  stop_music();
  printf("Amazing intro with cool effects.. Wow!\n");
  start_menu_music();
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
  char *menu_items[MAIN_MENU_ITEM_COUNT] = {"Start Game", "Show Intro", "Credits",
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
    SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 255, 0, 0));
    SDL_UpdateWindowSurface(gWindow);
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
