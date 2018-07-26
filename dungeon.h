#ifndef _DUNGEON_H
#define _DUNGEON_H

#include <stdbool.h>
#include <stdlib.h>
#include "constants.h"
#include "structs.h"

extern char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
extern bool gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];

bool tile_is_blocked(const Point p);
void create_dungeon();

#endif