#ifndef _STRUCTS_H
#define _STRUCTS_H
#include "enums.h"
#include "sdl2.h"
#include "constants.h"

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    Point target;
    char path[MAX_PATH_LENGTH];
    enum ANIMATION animation;
    enum ARMOR_CLASS armor_class;
    enum CHARACTER_CLASS character_class;
    enum PLAYER_DIRECTION direction;
    enum PLAYER_STATE state;
    int animation_frame;
    int current_game_level;
    int level;
    int point_in_path;
    int world_x;
    int world_y;
} Player;

typedef struct
{
    Point target;
    char path[MAX_PATH_LENGTH];
    enum ANIMATION animation;
    enum MONSTER_DIRECTION direction;
    enum MONSTER_STATE state;
    enum MONSTER_STATE next_state;
    int animation_frame;
    int id;
    int level;
    int point_in_path;
    int world_x;
    int world_y;
} Monster;

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
    ImageAsset image;
    int speed;
    int offset_x;
    int offset_y;
} Animation;

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

typedef struct
{
    int x;
    int y;
    int monster_id;
    int item_id;
    int object_id;
    int player_id;
} MouseCursor;

#endif