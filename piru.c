#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "assets.h"
#include "constants.h"
#include "direction.h"
#include "dungeon.h"
#include "enums.h"
#include "monster.h"
#include "pathfinding.h"
#include "player.h"
#include "point.h"
#include "sdl2.h"
#include "structs.h"
#include "time.h"

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

void draw_dungeon()
{
  int x, y;
  Point isometric_point, cartesian_point;
  ImageAsset asset;
  for (y = 0; y < DUNGEON_SIZE; y++)
  {
    for (x = 0; x < DUNGEON_SIZE; x++)
    {
      if (x < gPlayer.world_x - CUTOFF_X || x > gPlayer.world_x + CUTOFF_X || y < gPlayer.world_y - CUTOFF_Y || y > gPlayer.world_y + CUTOFF_Y)
      {
        continue;
      }
      cartesian_point.x = x - gPlayer.world_x;
      cartesian_point.y = y - gPlayer.world_y;
      isometric_point = cartesian_to_isometric(cartesian_point);

      //Render texture to screen

      if (gDungeon[y][x] == 'w')
      {
        asset = gImageAssets[SPRITE_STONE];
      }
      if (gDungeon[y][x] == 'f')
      {
        asset = gImageAssets[SPRITE_GROVEL];
      }
      SDL_Rect fillRect = {isometric_point.x - TILE_WIDTH_HALF + (SCREEN_WIDTH / 2) - gPlayer.pixel_x,
                           isometric_point.y + (SCREEN_HEIGHT / 2) - gPlayer.pixel_y, TILE_WIDTH, TILE_HEIGHT};
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
  SDL_RenderCopy(gRenderer, gImageAssets[SPRITE_SELECTION].texture,
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
    SDL_RenderCopy(gRenderer, gImageAssets[SPRITE_SELECTION].texture,
                   NULL,
                   &fillRect);
  }
}

void draw_cursor()
{
  int mx, my;
  SDL_GetMouseState(&mx, &my);
  SDL_Rect cursorQuad = {mx, my, 24, 24};
  SDL_RenderCopy(gRenderer, gImageAssets[SPRITE_CURSOR].texture,
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
    if (monsters[i].world_x < gPlayer.world_x - CUTOFF_X || monsters[i].world_x > gPlayer.world_x + CUTOFF_X || monsters[i].world_y < gPlayer.world_y - CUTOFF_Y || monsters[i].world_y > gPlayer.world_y + CUTOFF_Y)
    {
      continue;
    }
    Point monster_point = {
        monsters[i].world_x - offset_x,
        monsters[i].world_y - offset_y};
    Point isometric_point = cartesian_to_isometric(monster_point);

    int current_frame = monsters[i].animation_frame;
    Animation currentMonsterAnimation = animations[monsters[i].animation][monsters[i].direction];
    int width = currentMonsterAnimation.frames[current_frame].w;
    int height = currentMonsterAnimation.frames[current_frame].h;
    SDL_Rect monster_quad = {
        isometric_point.x + (SCREEN_WIDTH / 2) + currentMonsterAnimation.offset_x + monsters[i].pixel_x - gPlayer.pixel_x,
        isometric_point.y + (SCREEN_HEIGHT / 2) + currentMonsterAnimation.offset_y + monsters[i].pixel_y - gPlayer.pixel_y,
        width,
        height};
    SDL_RenderCopy(gRenderer, currentMonsterAnimation.image.texture,
                   &currentMonsterAnimation.frames[current_frame],
                   &monster_quad);
  }
}
void draw_and_blit()
{
  //Clear screen
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
  SDL_RenderClear(gRenderer);

  draw_dungeon();
  //draw_debug_path();

  draw_monsters();

  //Render texture to screen
  Animation currentPlayerAnimation = animations[gPlayer.animation][gPlayer.direction];
  int current_frame = gPlayer.animation_frame;
  int width = currentPlayerAnimation.frames[current_frame].w;
  int height = currentPlayerAnimation.frames[current_frame].h;
  SDL_Rect playerRenderQuad = {(SCREEN_WIDTH / 2) + currentPlayerAnimation.offset_x,
                               (SCREEN_HEIGHT / 2) + currentPlayerAnimation.offset_y,
                               width,
                               height};

  SDL_RenderCopy(gRenderer, currentPlayerAnimation.image.texture,
                 &currentPlayerAnimation.frames[current_frame],
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
      int monster_clicked = -1;

      if (mouse_was_pressed)
      {
        memset(gPlayer.path, -1, MAX_PATH_LENGTH);
        monster_clicked = gDungeonMonsterTable[selectedTile.y][selectedTile.x];
        if (monster_clicked >= 0)
        {
          if (gPlayer.state != PLAYER_ATTACKING && get_distance(player_position, selectedTile) <= gPlayer.attack_radius)
          {
            gPlayer.state = PLAYER_ATTACKING;
            gPlayer.next_state = PLAYER_STANDING;
            gPlayer.animation_frame = 0;
            gPlayer.direction = player_get_direction8(gPlayer.world_x, gPlayer.world_y, selectedTile.x, selectedTile.y);
            gPlayer.target_monster_id = monster_clicked;
          }
          else
          {
            gPlayer.destination_action = PLAYER_DESTINATION_ATTACK;
          }
        }
        else
        {
          gPlayer.destination_action = PLAYER_DESTINATION_STAND;
        }
      }
      if (mouse_was_pressed && !(player_position.x == selectedTile.x && player_position.y == selectedTile.y) && find_path(player_position, selectedTile, gPlayer.path))
      {
        gPlayer.state = PLAYER_MOVING;
        gPlayer.point_in_path = 0;
        gPlayer.target = selectedTile;
      }
      else if (mouse_was_pressed && gPlayer.state != PLAYER_ATTACKING)
      {
        gPlayer.state = PLAYER_STANDING;
        gPlayer.point_in_path = 0;
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
  gPlayer.previous_animation_frame = gPlayer.animation_frame;
  if (gPlayer.frames_since_animation_frame >= gPlayer.animation_intervals[gPlayer.animation])
  {
    gPlayer.frames_since_animation_frame = 0;
    int animFrames = animations[gPlayer.animation][gPlayer.direction].columns;
    gPlayer.animation_frame++;
    if (gPlayer.animation_frame >= animFrames)
    {
      gPlayer.animation_frame = 0;
      if (gPlayer.next_state != PLAYER_NO_STATE)
      {
        gPlayer.state = gPlayer.next_state;
        gPlayer.next_state = PLAYER_NO_STATE;
      }
    }
  }
  else
  {
    gPlayer.frames_since_animation_frame += gClock.delta;
  }
}

void update_monster_animations()
{
  int id;
  for (id = 0; id < created_monsters; id++)
  {
    monsters[id].previous_animation_frame = monsters[id].animation_frame;
    if (monsters[id].frames_since_animation_frame >= monsters[id].animation_intervals[monsters[id].animation])
    {
      monsters[id].frames_since_animation_frame = 0;
      int animFrames = animations[monsters[id].animation][monsters[id].direction].columns;
      monsters[id].animation_frame++;
      if (monsters[id].animation_frame >= animFrames)
      {
        if (monsters[id].state != MONSTER_DEAD && monsters[id].next_state != MONSTER_NO_STATE)
        {
          monsters[id].state = monsters[id].next_state;
          monsters[id].next_state = MONSTER_NO_STATE;
        }
        monsters[id].animation_frame = 0;
      }
    }
    else
    {
      monsters[id].frames_since_animation_frame += gClock.delta;
    }
  }
}

void update_animations()
{
  update_player_animations();
  update_monster_animations();
}

void update_monsters()
{
  int i;
  for (i = 0; i < created_monsters; i++)
  {
    if (monsters[i].world_x < gPlayer.world_x - CUTOFF_X * 2 || monsters[i].world_x > gPlayer.world_x + CUTOFF_X * 2 || monsters[i].world_y < gPlayer.world_y - CUTOFF_Y * 2 || monsters[i].world_y > gPlayer.world_y + CUTOFF_Y * 2)
    {
      continue;
    }
    update_monster(i);
  }
}

void game_loop()
{
  if (!gGamePaused)
  {
    update_input();
    update_animations();
    update_player();
    update_monsters();
    //printf("%d, %d\n", gClock.delta, gClock.last_tick_time);
    gGameRunning = gPlayer.hp > 0;
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
  tick();
  while (gGameRunning)
  {
    game_loop();
    draw_and_blit();
    tick();
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
  init_clock();
  load_assets();
  init_player();
  create_dungeon();
  Point monster_point;
  memset(monsters, 0, MAX_MONSTERS);
  created_monsters = 0;
  int ms;
  for (ms = 0; ms < MAX_MONSTERS; ms++)
  {
    monster_point.x = (rand() % DUNGEON_SIZE - 1) + 1;
    monster_point.y = (rand() % DUNGEON_SIZE - 1) + 1;
    create_monster(monster_point);
  }
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
