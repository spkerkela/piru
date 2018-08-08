#ifndef _EFFECT_H
#define _EFFECT_H
#include "dungeon.h"
#include "enums.h"
#include "point.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

extern int ground_effect_count;
extern GroundEffect ground_effects[MAX_GROUND_EFFECTS];
void create_ground_effect(const Point at, GroundEffect effect);

#endif