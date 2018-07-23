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
    enum CHARACTER_CLASS character_class;
    enum ARMOR_CLASS armor_class;
    enum DIRECTION direction;
    int level;
    int current_game_level;
    int world_x;
    int world_y;
    char path[MAX_PATH_LENGTH];
    int point_in_path;
    bool moving;
    Point target;
} Player;

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