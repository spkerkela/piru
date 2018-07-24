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
#include "player.h"
#include "direction.h"

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;

bool gGameRunning;
bool gGamePaused;
SDL_Rect gPlayerSprites[8 * 16];

char *direction_str[PLAYER_DIRECTION_COUNT] = {
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

extern Player gPlayer;
Monster monsters[MAX_MONSTERS];

int created_monsters = 0;

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

bool create_monster(const Point at)
{
  if (gDungeonBlockTable[at.y][at.x])
  {
    return false;
  }
  if (created_monsters >= MAX_MONSTERS)
  {
    return false;
  }
  Monster monster;
  monster.world_x = at.x;
  monster.world_y = at.y;
  monster.id = created_monsters;
  monster.level = 1;
  monster.state = MONSTER_STANDING;
  monster.direction = MONSTER_SOUTH;
  Point monster_target = {monster.world_x, monster.world_y};
  monster.target = monster_target;
  memset(monster.path, -1, MAX_PATH_LENGTH);
  monsters[created_monsters++] = monster;
  return true;
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
  gPlayer.direction = PLAYER_SOUTH;
  gPlayer.point_in_path = 0;
  gPlayer.state = PLAYER_STANDING;
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
  cartesian_point.x = selectedTile.x;
  cartesian_point.y = selectedTile.y;
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
    enum PATH_CODE code = (enum PATH_CODE)gPlayer.path[i];
    Point dir = get_direction_from_path(code);
    draw_point.x += dir.x;
    draw_point.y += dir.y;

    isometric_point = cartesian_to_isometric(draw_point);
    SDL_Rect fillRect = {isometric_point.x - TILE_WIDTH_HALF + (SCREEN_WIDTH / 2),
                         isometric_point.y + (SCREEN_HEIGHT / 2), TILE_WIDTH, TILE_HEIGHT};
    SDL_RenderCopy(gRenderer, gImageAssets[3].texture,
                   NULL,
                   &fillRect);
  }
}

void draw_cursor()
{
  int mx, my;
  SDL_GetMouseState(&mx, &my);
  SDL_Rect cursorQuad = {mx, my, 24, 24};
  SDL_RenderCopy(gRenderer, gImageAssets[4].texture,
                 NULL,
                 &cursorQuad);
}

void draw_monsters()
{
  int i;
  int offset_x = gPlayer.world_x;
  int offset_y = gPlayer.world_y;
  for (i = 0; i < created_monsters; i++)
  {
    Point monster_point = {
        monsters[i].world_x - offset_x,
        monsters[i].world_y - offset_y};
    Point isometric_point = cartesian_to_isometric(monster_point);

    Animation currentMonsterAnimation = animations[1][monsters[i].direction];
    int width = currentMonsterAnimation.frames[currentMonsterAnimation.currentFrame].w;
    int height = currentMonsterAnimation.frames[currentMonsterAnimation.currentFrame].h;
    SDL_Rect monster_quad = {
        isometric_point.x + (SCREEN_WIDTH / 2) + currentMonsterAnimation.offset_x,
        isometric_point.y + (SCREEN_HEIGHT / 2) + currentMonsterAnimation.offset_y,
        width,
        height};
    SDL_RenderCopy(gRenderer, gImageAssets[5].texture,
                   &currentMonsterAnimation.frames[currentMonsterAnimation.currentFrame],
                   &monster_quad);
  }
}
void draw_and_blit()
{
  //Clear screen
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
  SDL_RenderClear(gRenderer);

  draw_dungeon();
  draw_debug_path();

  draw_monsters();

  //Render texture to screen
  Animation currentPlayerAnimation = animations[0][gPlayer.direction];
  int width = currentPlayerAnimation.frames[currentPlayerAnimation.currentFrame].w;
  int height = currentPlayerAnimation.frames[currentPlayerAnimation.currentFrame].h;
  SDL_Rect playerRenderQuad = {(SCREEN_WIDTH / 2) + currentPlayerAnimation.offset_x,  // 108
                               (SCREEN_HEIGHT / 2) + currentPlayerAnimation.offset_y, //120
                               width,
                               height};

  SDL_RenderCopy(gRenderer, gImageAssets[0].texture,
                 &currentPlayerAnimation.frames[currentPlayerAnimation.currentFrame],
                 &playerRenderQuad);

  draw_cursor();
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
      bool mouse_was_pressed = e.type == SDL_MOUSEBUTTONDOWN;
      int mx, my;
      Point player_position = {gPlayer.world_x, gPlayer.world_y};
      SDL_GetMouseState(&mx, &my);
      Point mouse_point;
      Point offset = cartesian_to_isometric(player_position);
      mouse_point.x = mx - (SCREEN_WIDTH / 2) + offset.x;
      mouse_point.y = my - (SCREEN_HEIGHT / 2) + offset.y;
      selectedTile = isometric_to_cartesian(mouse_point);

      if (mouse_was_pressed)
      {
        memset(gPlayer.path, -1, MAX_PATH_LENGTH);
      }
      if (mouse_was_pressed && find_path(player_position, selectedTile, gPlayer.path))
      {
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
          gPlayer.state = PLAYER_MOVING;
          gPlayer.point_in_path = 0;
          gPlayer.target = selectedTile;
        }
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
        if (gPlayer.direction >= PLAYER_DIRECTION_COUNT)
        {
          gPlayer.direction = PLAYER_SOUTH;
        }
        break;
      case SDLK_RIGHT:
        if (gPlayer.direction == PLAYER_SOUTH)
        {
          gPlayer.direction = PLAYER_SOUTH_EAST_3;
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

void update_player_animations()
{
  enum PLAYER_DIRECTION dir;
  for (dir = PLAYER_SOUTH; dir < PLAYER_DIRECTION_COUNT; dir++)
  {
    int animFrames = animations[0][dir].columns;
    animations[0][dir].currentFrame += 1;
    if (animations[0][dir].currentFrame >= animFrames)
    {
      animations[0][dir].currentFrame = 0;
    }
  }
}

void update_monster_animations()
{
  enum MONSTER_DIRECTION dir;
  for (dir = MONSTER_SOUTH_WEST; dir < MONSTER_DIRECTION_COUNT; dir++)
  {
    int animFrames = animations[0][dir].columns;
    animations[1][dir].currentFrame += 1;
    if (animations[1][dir].currentFrame >= animFrames)
    {
      animations[1][dir].currentFrame = 0;
    }
  }
}

void update_animations()
{
  update_player_animations();
  update_monster_animations();
}

void game_loop()
{
  if (!gGamePaused)
  {
    SDL_Delay(50);
    update_input();
    update_animations();
    update_player();
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
  Point monster_point = {1, 1};
  create_monster(monster_point);
  monster_point.x = 6;
  monster_point.y = 6;
  create_monster(monster_point);
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
