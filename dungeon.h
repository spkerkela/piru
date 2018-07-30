#ifndef _DUNGEON_H
#define _DUNGEON_H

#include <stdbool.h>
#include <stdlib.h>
#include "constants.h"
#include "structs.h"
#include "player.h"

extern char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];

bool tile_is_blocked(const Point p);
bool tile_is_blocked_for_monster(const Point p);
void create_dungeon();

#endif