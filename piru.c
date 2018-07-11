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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TTF_Font *gFont = NULL;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define DUNGEON_SIZE 112
#define MAX_NODES 300
#define MAX_PATH_LENGTH 25
const int TILE_WIDTH = 64;
const int TILE_HEIGHT = 32;
const int TILE_WIDTH_HALF = TILE_WIDTH / 2;
const int TILE_HEIGHT_HALF = TILE_HEIGHT / 2;

char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];

bool gGameRunning;
bool gGamePaused;
SDL_Rect gPlayerSprites[8 * 16];

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

enum DIRECTION
{
  SOUTH,
  SOUTH_WEST_1,
  SOUTH_WEST_2,
  SOUTH_WEST_3,
  WEST,
  NORTH_WEST_1,
  NORTH_WEST_2,
  NORTH_WEST_3,
  NORTH,
  NORTH_EAST_1,
  NORTH_EAST_2,
  NORTH_EAST_3,
  EAST,
  SOUTH_EAST_1,
  SOUTH_EAST_2,
  SOUTH_EAST_3,
  DIRECTION_COUNT
};

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

typedef struct
{
  int x;
  int y;
} Point;

Point selectedTile;

Point cartesian_to_isometric(const Point cartesian_point)
{
  Point isometric_point;
  isometric_point.x = (cartesian_point.x - cartesian_point.y) * TILE_WIDTH_HALF;
  isometric_point.y = (cartesian_point.x + cartesian_point.y) * TILE_HEIGHT_HALF;
  return isometric_point;
}

Point isometric_to_cartesian(const Point isometric_point)
{
  Point cartesian_point;
  cartesian_point.x = (isometric_point.x / TILE_WIDTH_HALF + isometric_point.y / TILE_HEIGHT_HALF) / 2;
  cartesian_point.y = (isometric_point.y / TILE_HEIGHT_HALF - (isometric_point.x / TILE_WIDTH_HALF)) / 2;
  return cartesian_point;
}

Point get_tile_coordinates(const Point cartesian_point)
{
  Point tile_coordinates;
  tile_coordinates.x = cartesian_point.x / TILE_HEIGHT;
  tile_coordinates.y = cartesian_point.y / TILE_HEIGHT;
  return tile_coordinates;
}

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

typedef struct
{
  enum CHARACTER_CLASS character_class;
  enum ARMOR_CLASS armor_class;
  enum DIRECTION direction;
  int level;
  int current_game_level;
  int world_x;
  int world_y;
  char path[MAX_PATH_LENGTH];
} Player;

Player gPlayer;

typedef struct
{
  char *assetName;
  SDL_Texture *texture;
} ImageAsset;

typedef struct
{
  SDL_Rect frames[1024];
  int columns;
  int rows;
  int currentFrame;
  ImageAsset image;
  int speed;
} Animation;

ImageAsset gImageAssets[256];
Animation gPlayerAnimations[256];

bool tile_is_blocked(const Point p)
{
  return gDungeonBlockTable[p.y][p.x];
}

void create_dungeon()
{
  int x, y;
  for (x = 0; x < DUNGEON_SIZE; x++)
  {
    for (y = 0; y < DUNGEON_SIZE; y++)
    {
      if (x == 0 || x == DUNGEON_SIZE - 1 || y == 0 || y == DUNGEON_SIZE - 1)
      {
        gDungeon[y][x] = 'w';
        gDungeonBlockTable[y][x] = true;
      }
      else
      {
        gDungeon[y][x] = 'f';
        gDungeonBlockTable[y][x] = false;
      }
    }
  }

  gDungeon[3][5] = 'w';
  gDungeonBlockTable[3][5] = true;
  gDungeon[2][5] = 'w';
  gDungeonBlockTable[2][5] = true;
  gDungeon[1][5] = 'w';
  gDungeonBlockTable[1][5] = true;
}
// Path finding
typedef char *Path;

typedef struct PathNode
{
  char f_score;
  char heuristic;
  short g_movement_cost;
  int x;
  int y;
  struct PathNode *parent;
  struct PathNode *children[8];
  struct PathNode *next_node;
} PathNode;

PathNode pre_allocated_nodes[MAX_NODES];
int path_nodes_in_use;

// for iterating over the 8 possible movement directions
char movement_directions_x[8] = {-1, -1, 1, 1, -1, 0, 1, 0};
char movement_directions_y[8] = {-1, 1, -1, 1, 0, -1, 0, 1};

char path_directions[9] = {5, 1, 6, 2, 0, 3, 8, 4, 7};
PathNode *visited_nodes_list;
PathNode *distance_sorted_frontier_list;

PathNode *search_stack[MAX_NODES];
int search_stack_size;

int reconstructed_path[MAX_PATH_LENGTH];

PathNode *get_new_node()
{
  if (path_nodes_in_use >= MAX_NODES)
  {
    printf("Too many nodes!!\n");
    return NULL;
  }
  else
  {
    PathNode *new_node = &pre_allocated_nodes[path_nodes_in_use++];
    memset(new_node, 0, sizeof(PathNode));
    return new_node;
  }
}
bool path_solid_pieces(PathNode *path, int a, int b)
{
  return true;
}
void path_next_node(PathNode *path)
{
  PathNode *current;
  PathNode *next;

  current = distance_sorted_frontier_list;
  next = distance_sorted_frontier_list->next_node;
  if (next)
  {
    do
    {
      if (next->f_score >= path->f_score)
        break;
      current = next;
      next = next->next_node;
    } while (next);
    path->next_node = next;
  }
  current->next_node = path;
}

char get_heuristic_cost(const Point source, const Point destination)
{

  int delta_x = abs(source.x - destination.x);
  int delta_y = abs(source.y - destination.y);
  return 2 * (delta_x + delta_y);
}

PathNode *get_next_path_node()
{
  PathNode *result = distance_sorted_frontier_list->next_node;
  if (result)
  {
    distance_sorted_frontier_list->next_node = result->next_node;
    result->next_node = visited_nodes_list->next_node;
    visited_nodes_list->next_node = result;
  }
  else
  {
    printf("Failed \n");
  }
  return result;
}

PathNode *get_frontier_node(const Point point)
{
  PathNode *result;
  result = distance_sorted_frontier_list;
  do
  {
    result = result->next_node;
  } while (result && (result->x != point.x || result->y != point.y));
  return result;
}

PathNode *get_visited_node(const Point point)
{
  PathNode *result;
  result = visited_nodes_list;
  do
  {
    result = result->next_node;
  } while (result && (result->x != point.x || result->y != point.y));
  return result;
}

int path_check_equal(PathNode *path, const Point destination)
{
  if (path->x == destination.x || path->y == destination.y)
  {
    return 2;
  }
  else
  {
    return 3;
  }
}

PathNode *path_pop_active_step()
{
  return search_stack[--search_stack_size];
}

void path_push_active_step(PathNode *path)
{
  search_stack[search_stack_size++] = path;
}

void path_set_coords(PathNode *path)
{
  PathNode *old_path;
  PathNode *active_path;
  char next_g_movement_score;
  PathNode **child_pointer;

  path_push_active_step(path);
  while (search_stack_size)
  {
    old_path = path_pop_active_step();
    child_pointer = old_path->children;
    int i;
    for (i = 0; i < 8; i++)
    {
      active_path = *child_pointer;
      if (!*child_pointer)
        break;

      Point p = {active_path->x, active_path->y};
      if (old_path->g_movement_cost + path_check_equal(old_path, p) <
          active_path->g_movement_cost)
      {
        if (path_solid_pieces(old_path, active_path->x, active_path->y))
        {
          active_path->parent = old_path;
          Point p2 = {active_path->x, active_path->y};
          next_g_movement_score =
              old_path->g_movement_cost + path_check_equal(old_path, p2);
          active_path->g_movement_cost = next_g_movement_score;
          active_path->f_score = next_g_movement_score + active_path->heuristic;
          path_push_active_step(active_path);
        }
      }
      ++child_pointer;
    }
  }
}

bool path_parent_path(PathNode *path, const Point next_destination, const Point from)
{
  int next_g_movement_cost;
  char dxdy_heuristic;
  char dxdy_f_score;

  signed int empty_slot;
  PathNode **path_child_pointer;

  char new_heuristic;
  PathNode *visited_node;

  next_g_movement_cost = path->g_movement_cost + path_check_equal(path, next_destination);

  PathNode *next_frontier;
  PathNode *new_node;
  next_frontier = get_frontier_node(next_destination);
  if (next_frontier)
  {
    empty_slot = 0;
    path_child_pointer = path->children;
    do
    {
      if (!*path_child_pointer)
      {
        break;
      }
      ++empty_slot;
      ++path_child_pointer;
    } while (empty_slot < 8);
    path->children[empty_slot] = next_frontier;
    if (next_g_movement_cost < next_frontier->g_movement_cost)
    {
      if (path_solid_pieces(path, next_destination.x, next_destination.y))
      {
        // we'll explore it later, just update
        dxdy_heuristic = next_frontier->heuristic;
        next_frontier->parent = path;
        next_frontier->g_movement_cost = next_g_movement_cost;
        next_frontier->f_score = next_g_movement_cost + dxdy_heuristic;
      }
    }
  }
  else
  {
    visited_node = get_visited_node(next_destination);
    if (visited_node)
    {
      empty_slot = 0;
      path_child_pointer = path->children;
      do
      {
        if (!*path_child_pointer)
        {
          break;
        }
        ++empty_slot;
        ++path_child_pointer;
      } while (empty_slot < 8);
      path->children[empty_slot] = visited_node;
      if (next_g_movement_cost < visited_node->g_movement_cost)
      {
        if (path_solid_pieces(path, next_destination.x, next_destination.y))
        {
          dxdy_f_score = next_g_movement_cost + visited_node->heuristic;
          visited_node->parent = path;
          visited_node->g_movement_cost = next_g_movement_cost;
          visited_node->f_score = dxdy_f_score;
          path_set_coords(visited_node);
        }
      }
    }
    else
    {
      new_node = get_new_node();
      if (!new_node)
      {
        return false;
      }

      new_node->parent = path;
      new_node->g_movement_cost = next_g_movement_cost;
      new_heuristic = get_heuristic_cost(next_destination, from);
      new_node->heuristic = new_heuristic;
      new_node->f_score = next_g_movement_cost + new_heuristic;
      new_node->x = next_destination.x;
      new_node->y = next_destination.y;
      // add it to the frontier
      path_next_node(new_node);
      empty_slot = 0;
      path_child_pointer = path->children;
      do
      {
        if (!*path_child_pointer)
          break;
        ++empty_slot;
        ++path_child_pointer;
      } while (empty_slot < 8);
      path->children[empty_slot] = new_node;
    }
  }

  return true;
}

bool path_get_path(PathNode *path, const Point from)
{
  int i;
  Point next_destination;

  for (i = 0; i < 8; i++)
  {
    next_destination.x = path->x + movement_directions_x[i];
    next_destination.y = path->y + movement_directions_y[i];
    if (tile_is_blocked(next_destination))
    {
      continue;
    }
    path_parent_path(path, next_destination, from);
    if (path_nodes_in_use >= MAX_NODES)
    {
      return false;
    }
  }
  return true;
}

bool find_path(const Point source, const Point destination, Path out_path)
{
  PathNode *path_start;
  char initial_heuristic;
  PathNode *next_node;
  int result;
  PathNode *current_node;
  PathNode **previous_node;
  int path_length;
  bool path_is_full;
  int *step_ptr;
  char step;
  if (destination.x < 0 || destination.y < 0)
  {
    return false;
  }

  path_nodes_in_use = 0;
  distance_sorted_frontier_list = get_new_node();
  search_stack_size = 0;
  visited_nodes_list = get_new_node();
  initial_heuristic = get_heuristic_cost(source, destination);

  path_start = get_new_node();
  path_start->g_movement_cost = 0;
  path_start->heuristic = initial_heuristic;
  path_start->x = source.x;
  path_start->y = source.y;
  path_start->f_score = initial_heuristic;

  distance_sorted_frontier_list->next_node = path_start;

  while (true)
  {
    next_node = get_next_path_node();
    if (!next_node)
    {
      printf("Failed to get node\n");
      return false;
    }
    if (next_node->x == destination.x && next_node->y == destination.y)
    {
      break;
    }
    if (!path_get_path(next_node, destination))
    {
      return false;
    }
  }
  current_node = next_node;
  previous_node = &next_node->parent;
  path_length = 0;
  if (*previous_node)
  {
    while (true)
    {
      path_is_full = path_length == MAX_PATH_LENGTH;
      if (path_length >= MAX_PATH_LENGTH)
        break;
      reconstructed_path[++path_length - 1] =
          path_directions[3 * (current_node->y - (*previous_node)->y) - (*previous_node)->x +
                          4 + current_node->x];
      current_node = *previous_node;
      previous_node = &(*previous_node)->parent;
      if (!*previous_node)
      {
        path_is_full = path_length == MAX_PATH_LENGTH;
        break;
      }
    }
    if (path_is_full)
      return false;
  }
  result = 0;
  if (path_length > 0)
  {
    step_ptr = &reconstructed_path[path_length - 1];
    do
    {
      step = (char)*step_ptr;
      --step_ptr;
      out_path[result++] = step;
    } while (result < path_length);
  }

  return true;
}

// End Path finding

bool init_SDL()
{
  return SDL_Init(SDL_INIT_EVERYTHING) >= 0;
}

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

ImageAsset load_image_asset(char *fileName)
{
  SDL_Texture *texture = NULL;
  SDL_Surface *loadedSurface = IMG_Load(fileName);
  if (loadedSurface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", fileName, IMG_GetError());
  }
  else
  {
    texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (texture == NULL)
    {
      printf("Unable to optimize image %s! SDL Error: %s\n", fileName, SDL_GetError());
    }
    SDL_FreeSurface(loadedSurface);
  }
  ImageAsset asset;
  asset.assetName = fileName;
  asset.texture = texture;
  return asset;
}

bool load_animations()
{
  ImageAsset playerSpriteSheet = gImageAssets[0];
  int width;
  int height;
  SDL_QueryTexture(playerSpriteSheet.texture, NULL, NULL, &width, &height);
  int animationColumns = 8;
  int animationRows = 16;
  int frameWidth = width / animationColumns;
  int frameHeight = height / animationRows;
  enum DIRECTION dir;
  for (dir = SOUTH; dir < DIRECTION_COUNT; dir++)
  {
    int x;
    for (x = 0; x < animationColumns; x++)
    {
      gPlayerAnimations[dir].currentFrame = 0;
      gPlayerAnimations[dir].columns = animationColumns;
      gPlayerAnimations[dir].rows = 1;
      gPlayerAnimations[dir].speed = 1;
      gPlayerAnimations[dir].image = playerSpriteSheet;
      gPlayerAnimations[dir].frames[x].x = x * frameWidth;
      gPlayerAnimations[dir].frames[x].y = dir * frameHeight;
      gPlayerAnimations[dir].frames[x].w = frameWidth;
      gPlayerAnimations[dir].frames[x].h = frameHeight;
    }
  }

  return true;
}

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
bool load_assets()
{
  int asset_index = 0;
  ImageAsset playerSpriteSheet = load_image_asset("assets/player2.png");
  ImageAsset grovelSpriteSheet = load_image_asset("assets/iso_dirt_1.png");
  ImageAsset stoneSpriteSheet = load_image_asset("assets/iso_stone_1.png");
  ImageAsset selectionSpriteSheet = load_image_asset("assets/iso_selection.png");
  gImageAssets[asset_index++] = playerSpriteSheet;
  gImageAssets[asset_index++] = grovelSpriteSheet;
  gImageAssets[asset_index++] = stoneSpriteSheet;
  gImageAssets[asset_index++] = selectionSpriteSheet;
  load_animations();

  return true;
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
  draw_point.x = 1;
  draw_point.y = 1;
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

typedef struct
{
  int x;
  int y;
  int monster_id;
  int item_id;
  int object_id;
  int player_id;
} MouseCursor;

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

bool load_font()
{
  bool success = true;

  //Open the font
  gFont = TTF_OpenFont("font.ttf", 28);
  if (gFont == NULL)
  {
    printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  }

  return success;
}

bool init()
{
  if (!init_SDL())
  {
    printf("%s", SDL_GetError());
    return false;
  }

  gWindow =
      SDL_CreateWindow("Piru", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!gWindow)
  {
    printf("%s", SDL_GetError());
    return false;
  }
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!gRenderer)
  {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
  {
    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    return false;
  }
  if (TTF_Init() == -1)
  {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
  if (!load_font())
  {
    printf("SDL_ttf could not load font! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
  return true;
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
