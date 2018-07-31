#ifndef _MONSTER_H
#define _MONSTER_H
#include "damage_text.h"
#include "direction.h"
#include "dungeon.h"
#include "enums.h"
#include "pathfinding.h"
#include "point.h"
#include "structs.h"
#include "time.h"
#include <stdlib.h>

extern Monster monsters[];
void update_monster(int id);
bool create_monster(const Point at);

#endif