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
    enum PLAYER_STATE next_state;
    enum PLAYER_DESTINATION_ACTION destination_action;
    int damage;
    int hp;
    int max_hp;
    int mana;
    int max_mana;
    int animation_frame;
    int current_game_level;
    int level;
    int point_in_path;
    int world_x;
    int world_y;
    int walk_interval;
    int frames_since_walk;
    int frames_since_animation_frame;
    int animation_intervals[MAX_ANIMATIONS];
    double attack_radius;
} Player;

typedef struct
{
    Point target;
    char path[MAX_PATH_LENGTH];
    enum ANIMATION animation;
    enum MONSTER_DIRECTION direction;
    enum MONSTER_STATE state;
    enum MONSTER_STATE next_state;
    int damage;
    int hp;
    int max_hp;
    int animation_frame;
    int id;
    int level;
    int point_in_path;
    int world_x;
    int world_y;
    int walk_interval;
    int frames_since_walk;
    int frames_since_animation_frame;
    int animation_intervals[MAX_ANIMATIONS];
    double attack_radius;
    double aggro_radius;
} Monster;

typedef struct
{
    uint32_t last_tick_time;
    uint32_t delta;
} Clock;

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