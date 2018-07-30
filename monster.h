#ifndef _MONSTER_H
#define _MONSTER_H
#include <stdlib.h>
#include "structs.h"
#include "enums.h"
#include "point.h"
#include "direction.h"
#include "pathfinding.h"
#include "time.h"
#include "dungeon.h"
#include "damage_text.h"

extern Monster monsters[];
void update_monster(int id);
bool create_monster(const Point at);

#endif