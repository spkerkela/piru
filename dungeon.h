#ifndef _DUNGEON_H
#define _DUNGEON_H

#include "constants.h"
#include "piru_math.h"
#include "player.h"
#include "structs.h"
#include <stdbool.h>
#include <stdlib.h>

extern char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];

bool tile_is_blocked(const Point p);
bool tile_is_blocked_for_monster(const Point p);
void create_dungeon();
void create_bsp_dungeon();
void clear_dungeon();

#endif