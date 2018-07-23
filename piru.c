#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "assets.h"
#include "constants.h"
#include "dungeon.h"
#include "enums.h"
#include "pathfinding.h"
#include "point.h"
#include "sdl2.h"
#include "structs.h"

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;

bool gGameRunning;
bool gGamePaused;
SDL_Rect gPlayerSprites[8 * 16];

char *direction_str[DIRECTION_COUNT] = {
    "SOUTH",
    "SOUTH_WEST_1",
    "SOUTH_WEST_2",
    "SOUTH_WEST_3",
    "WEST",
    "NORTH_WEST_1",
    "NORTH_WEST_2",
    "NORTH_WEST_3",
    "NORTH",
    "NORTH_EAST_1",
    "NORTH_EAST_2",
    "NORTH_EAST_3",
    "EAST",
    "SOUTH_EAST_1",
    "SOUTH_EAST_2",
    "SOUTH_EAST_3"};

char *character_class_str[CHARACTER_CLASS_COUNT] = {
    "Mage",
    "Rogue",
    "Warrior"};

Point selectedTile;

enum DIRECTION get_direction(const int x1, const int y1, const int x2, const int y2)
{
  static const double step = 360.0 / DIRECTION_COUNT;
  double angle = -atan2((double)y2 - (double)y1, (double)x2 - (double)x1) * 180 / M_PI;
  if (angle >= 0)
  {
    enum DIRECTION dir = EAST;
    double start = -step / 2;
    while (true)
    {

      if (angle < step + start)
      {
        return dir;
      }
      start += step;
      --dir;

      if (start > 1000.0)
      {
        break;
      }
    }
  }
  else
  {
    enum DIRECTION dir = EAST;
    double start = step / 2;
    while (true)
    {
      if (angle > (start - step))
      {
        return dir;
      }
      start -= step;
      dir++;
      if (dir >= DIRECTION_COUNT)
      {
        dir = SOUTH;
      }

      if (start < -1000.0)
      {
        break;
      }
    }
  }

  return SOUTH;
}

Player gPlayer;

// Path finding
void start_menu_music() { printf("Ominous music playing..\n"); }

void stop_music() { printf("Music stopped..\n"); }

void draw_text(char *text, SDL_Color textColor, SDL_Rect *source, SDL_Rect *dest)
{
  SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, text, textColor);
  if (textSurface == NULL)
  {
    printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
  }
  else
  {
    //Create texture from surface pixels
    SDL_Texture *mTexture = NULL;
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL)
    {
      printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }
    //Get rid of old surface
    SDL_RenderCopy(gRenderer, mTexture, source, dest);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
  }
}

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
      SDL_RenderClear(gRenderer);

      SDL_Color color = {255, 255, 0};
      draw_text(character_class_str[selected], color, NULL, NULL);

      //Update screen
      SDL_RenderPresent(gRenderer);
    }
  }
  gPlayer.level = 1;
  gPlayer.character_class = selected;
  gPlayer.armor_class = selected;
  memset(gPlayer.path, -1, MAX_PATH_LENGTH);
}

void render_select_character_screen(const int selected, const char *character_names[], int char_count)
{
  //Clear screen
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
  SDL_RenderClear(gRenderer);
  SDL_Rect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 0);
  SDL_RenderFillRect(gRenderer, &fillRect);

  //Render texture to screen
  SDL_RenderPresent(gRenderer);
}

void select_character_menu()
{
  printf("Select character\n");
  static const int CHAR_COUNT = 5;
  static const char *temp_characters[5] = {
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
    render_select_character_screen(selected, temp_characters, CHAR_COUNT);
  }
}

void init_player_position()
{
  gPlayer.world_x = 1;
  gPlayer.world_y = 1;
  gPlayer.current_game_level = 0;
  gPlayer.direction = SOUTH;
}

void draw_dungeon()
{
  int x, y;
  Point isometric_point, cartesian_point;
  ImageAsset asset;
  for (y = 0; y < DUNGEON_SIZE; y++)
  {
    for (x = 0; x < DUNGEON_SIZE; x++)
    {
      cartesian_point.x = x - gPlayer.world_x;
      cartesian_point.y = y - gPlayer.world_y;
      isometric_point = cartesian_to_isometric(cartesian_point);

      //Render texture to screen

      if (gDungeon[y][x] == 'w')
      {
        asset = gImageAssets[2];
      }
      if (gDungeon[y][x] == 'f')
      {
        asset = gImageAssets[1];
      }
      SDL_Rect fillRect = {isometric_point.x - TILE_WIDTH_HALF + (SCREEN_WIDTH / 2),
                           isometric_point.y + (SCREEN_HEIGHT / 2), TILE_WIDTH, TILE_HEIGHT};
      SDL_RenderCopy(gRenderer, asset.texture,
                     NULL,
                     &fillRect);

      SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 0);
    }
  }
  cartesian_point.x = selectedTile.x - gPlayer.world_x;
  cartesian_point.y = selectedTile.y - gPlayer.world_y;
  isometric_point = cartesian_to_isometric(cartesian_point);
  SDL_Rect fillRect = {isometric_point.x - TILE_WIDTH_HALF + (SCREEN_WIDTH / 2),
                       isometric_point.y + (SCREEN_HEIGHT / 2), TILE_WIDTH, TILE_HEIGHT};
  SDL_RenderCopy(gRenderer, gImageAssets[3].texture,
                 NULL,
                 &fillRect);
}

void draw_debug_path()
{
  int i;
  Point draw_point;
  Point isometric_point;
  draw_point.x = 0;
  draw_point.y = 0;
  for (i = 0; i < sizeof(gPlayer.path); i++)
  {
    int code = (int)gPlayer.path[i];
    switch (code)
    {
    case 5:
      draw_point.x--;
      draw_point.y--;
      break;
    case 1:
      draw_point.y--;
      break;
    case 6:
      draw_point.x++;
      draw_point.y--;
      break;
    case 2:
      draw_point.x--;
      break;
    case 0:
      break;
    case 3:
      draw_point.x++;
      break;
    case 8:
      draw_point.x--;
      draw_point.y++;
      break;
    case 4:
      draw_point.y++;
      break;
    case 7:
      draw_point.x++;
      draw_point.y++;
      break;
    default:
      continue;
    }

    isometric_point = cartesian_to_isometric(draw_point);
    SDL_Rect fillRect = {isometric_point.x - TILE_WIDTH_HALF + (SCREEN_WIDTH / 2),
                         isometric_point.y + (SCREEN_HEIGHT / 2), TILE_WIDTH, TILE_HEIGHT};
    SDL_RenderCopy(gRenderer, gImageAssets[3].texture,
                   NULL,
                   &fillRect);
  }
}

void draw_and_blit()
{
  //Clear screen
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
  SDL_RenderClear(gRenderer);

  draw_dungeon();
  draw_debug_path();

  //Render texture to screen
  SDL_Rect playerRenderQuad = {(SCREEN_WIDTH / 2) - 108,
                               (SCREEN_HEIGHT / 2) - 120,
                               150,
                               150};

  Animation currentPlayerAnimation = gPlayerAnimations[gPlayer.direction];

  SDL_RenderCopy(gRenderer, gImageAssets[0].texture,
                 &currentPlayerAnimation.frames[currentPlayerAnimation.currentFrame],
                 &playerRenderQuad);

  //Update screen
  SDL_RenderPresent(gRenderer);
}

void update_input()
{
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEMOTION)
    {
      int mx, my;
      SDL_GetMouseState(&mx, &my);
      Point mouse_point;
      mouse_point.x = mx - (SCREEN_WIDTH / 2);
      mouse_point.y = my - (SCREEN_HEIGHT / 2);
      Point tile_coordinates = isometric_to_cartesian(mouse_point);
      selectedTile = tile_coordinates;
      Point player_position = {gPlayer.world_x, gPlayer.world_y};

      memset(gPlayer.path, -1, MAX_PATH_LENGTH);
      printf("TILE_COORDINATES(%d, %d)\n", tile_coordinates.x, tile_coordinates.y);
      if (find_path(player_position, tile_coordinates, gPlayer.path))
      {
        gPlayer.direction = get_direction(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, mx, my);
        printf("TILE_COORDINATES(%d, %d)\n", tile_coordinates.x, tile_coordinates.y);
        printf("PLAYER POSITION(%d, %d)\n", player_position.x, player_position.y);
      }
    }
    if (e.type == SDL_QUIT)
    {
      gGameRunning = false;
      break;
    }
    if (e.type == SDL_KEYDOWN)
    {
      switch (e.key.keysym.sym)
      {
      case SDLK_RETURN:
        gGameRunning = false;
        break;
      case SDLK_ESCAPE:
        gGamePaused = !gGamePaused;
        break;
      case SDLK_LEFT:
        gPlayer.direction++;
        if (gPlayer.direction >= DIRECTION_COUNT)
        {
          gPlayer.direction = SOUTH;
        }
        break;
      case SDLK_RIGHT:
        if (gPlayer.direction == SOUTH)
        {
          gPlayer.direction = SOUTH_EAST_3;
        }
        else
        {
          gPlayer.direction--;
        }

        break;
      default:
        break;
      }
    }
  }
}

void update_animations()
{
  enum DIRECTION dir;
  for (dir = SOUTH; dir < DIRECTION_COUNT; dir++)
  {
    int animFrames = gPlayerAnimations[dir].columns;
    gPlayerAnimations[dir].currentFrame += 1;
    if (gPlayerAnimations[dir].currentFrame >= animFrames)
    {
      gPlayerAnimations[dir].currentFrame = 0;
    }
  }
}

void game_loop()
{
  if (!gGamePaused)
  {
    SDL_Delay(50);
    update_input();
    update_animations();
  }
  else
  {

    update_input();
  }
}

MouseCursor gCursor;

void init_cursor()
{
  SDL_GetMouseState(&gCursor.x, &gCursor.y);
  gCursor.monster_id = -1;
  gCursor.item_id = -1;
  gCursor.object_id = -1;
  gCursor.player_id = -1;
}

void run_game_loop(enum GAME_START_MODE start_mode)
{
  gGameRunning = true;
  gGamePaused = false;
  init_cursor();
  draw_and_blit();
  while (gGameRunning)
  {
    game_loop();
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
  load_assets();
  init_player_position();
  create_dungeon();
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

void show_text_screen(char *text)
{
  bool show_credits = true;
  SDL_Event e;
  while (show_credits)
  {
    while (SDL_PollEvent(&e) != 0)
    {
      if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN)
      {
        return;
      }
    }
    SDL_RenderClear(gRenderer);

    SDL_Color color = {255, 0, 0};
    SDL_Rect dest = {10, 10, SCREEN_WIDTH, 200};
    draw_text(text, color, NULL, &dest);

    //Update screen
    SDL_RenderPresent(gRenderer);
  }
}

void show_intro()
{
  stop_music();
  show_text_screen("Amazing intro with cool effects.. Wow!");
  start_menu_music();
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
    show_text_screen("Piru was made by Simo-Pekka Kerkela!");
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
      }
    }
    //Clear screen
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gRenderer);

    SDL_Color color = {255, 255, 0};
    draw_text(menu_items[selected], color, NULL, NULL);

    //Update screen
    SDL_RenderPresent(gRenderer);
  }
  stop_music();
  return 0;
}

int main(int argc, char const *argv[])
{
  if (init())
  {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
    srand(SDL_GetTicks());
    printf("Hello Piru\n");
    printf("%d\n", rand());
    main_menu();
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
  }
  else
  {
    return 1;
  }
}
