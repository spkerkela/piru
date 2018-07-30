#ifndef _PLAYER_H
#define _PLAYER_H
#include <string.h>
#include "damage_text.h"
#include "direction.h"
#include "enums.h"
#include "monster.h"
#include "point.h"
#include "structs.h"
#include "time.h"

extern Player gPlayer;
void init_player();
void update_player();

#endif