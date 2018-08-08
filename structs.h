#ifndef _STRUCTS_H
#define _STRUCTS_H
#include "constants.h"
#include "enums.h"
#include "sdl2.h"

// https://stackoverflow.com/questions/1371460/state-machines-tutorials
struct Player;
typedef void player_state_fn(struct Player *);
typedef void monster_state_fn(int monster_id);

typedef struct {
  int x;
  int y;
} Point;

typedef struct GroundEffect {
  enum ANIMATION animation;
  int animation_interval;
  bool active;
  int animation_frame;
  int frames_since_animation_frame;
  int previous_animation_frame;
  int x, y;
} GroundEffect;

typedef struct Spell {
  int id;
  char *name;
  enum SPELL_TYPE type;
  enum DAMAGE_TYPE damage_type;
  int base_mana_cost;
  double dps_multiplier;
  double range;
  double radius;
  bool leaves_ground_effect;
  GroundEffect effect;
} Spell;

typedef struct Player {
  player_state_fn *next_state_fn;
  Point target;
  Point new_target;
  char path[MAX_PATH_LENGTH];
  enum ANIMATION animation;
  enum ARMOR_CLASS armor_class;
  enum CHARACTER_CLASS character_class;
  enum PLAYER_DIRECTION direction;
  enum PLAYER_STATE state;
  enum PLAYER_STATE next_state;
  enum PLAYER_DESTINATION_ACTION destination_action;
  Spell active_spell;
  Spell left_spell;
  Spell right_spell;
  Spell no_mana_fallback_spell;
  int target_monster_id;
  int base_damage_min;
  int base_damage_max;
  int hp;
  int max_hp;
  int mana;
  int max_mana;
  int animation_frame;
  int previous_animation_frame;
  int current_game_level;
  int level;
  int point_in_path;
  int world_x;
  int world_y;
  int previous_world_x;
  int previous_world_y;
  int next_x;
  int next_y;
  bool moving_between_points;
  int pixel_x;
  int pixel_y;
  int frames_since_walk;
  int walk_interval;
  int frames_since_animation_frame;
  int animation_intervals[MAX_ANIMATIONS];
  double attack_radius;
} Player;

typedef struct {
  Point target;
  monster_state_fn *next_state_fn;
  char path[MAX_PATH_LENGTH];
  enum ANIMATION animation;
  enum MONSTER_DIRECTION direction;
  enum MONSTER_STATE state;
  enum MONSTER_STATE next_state;
  int damage;
  int hp;
  int max_hp;
  int animation_frame;
  int previous_animation_frame;
  int id;
  int level;
  int point_in_path;
  int world_x;
  int world_y;
  bool moving_between_points;
  int pixel_x;
  int pixel_y;
  int walk_interval;
  int frames_since_walk;
  int frames_since_animation_frame;
  int animation_intervals[MAX_ANIMATIONS];
  double attack_radius;
  double aggro_radius;
} Monster;

typedef struct {
  uint32_t last_tick_time;
  uint32_t delta;
} Clock;

typedef struct {
  char *assetName;
  SDL_Texture *texture;
  int width;
  int height;
} ImageAsset;

typedef struct {
  SDL_Rect frames[1024];
  int columns;
  int rows;
  ImageAsset image;
  int speed;
  int offset_x;
  int offset_y;
} Animation;

typedef struct PathNode {
  char f_score;
  char heuristic;
  short g_movement_cost;
  int x;
  int y;
  struct PathNode *parent;
  struct PathNode *children[8];
  struct PathNode *next_node;
} PathNode;

typedef struct {
  int x;
  int y;
  int monster_id;
  int item_id;
  int object_id;
  bool leftButtonDown;
  bool rightButtonDown;
} MouseCursor;

typedef struct {
  int r, g, b;
} RGB_Color;

typedef struct DamageText {
  int x, y, y_offset, frames_alive;
  bool alive;
  SDL_Texture *texture;
  int width;
} DamageText;

#endif