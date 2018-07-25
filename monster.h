#ifndef _MONSTER_H
#define _MONSTER_H
#include <stdlib.h>
#include "structs.h"
#include "enums.h"
#include "point.h"
#include "direction.h"
#include "pathfinding.h"
#include "time.h"

extern Monster monsters[];
void update_monster(int id);

#endif