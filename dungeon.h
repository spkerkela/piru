#ifndef _DUNGEON_H
#define _DUNGEON_H

#include "constants.h"
#include "piru_math.h"
#include "player.h"
#include "structs.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct BSP BSP;
struct BSP {
  int x, y, width, height;
  BSP *child1;
  BSP *child2;
  SDL_Rect *room;
};

extern char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern Point gPlayerLevelSpawn;

bool split_bsp(BSP *root);

Point center(const SDL_Rect *rect);

SDL_Rect *get_room(BSP *root);

bool tile_is_blocked(const Point p);
bool tile_is_blocked_for_monster(const Point p);

void carve_dungeon(BSP *bsp);
void clear_dungeon();
void connect_points(Point point1, Point point2);
void connect_rooms(SDL_Rect *room1, SDL_Rect *room2);
void create_bsp_dungeon();
void create_dungeon();
void create_horizontal_tunnel(int x1, int x2, int y);
void create_vertical_tunnel(int y1, int y2, int x);

#endif