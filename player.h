#ifndef _PLAYER_H
#define _PLAYER_H
#include "assets.h"
#include "damage_text.h"
#include "direction.h"
#include "enums.h"
#include "monster.h"
#include "point.h"
#include "spell.h"
#include "structs.h"
#include "time.h"
#include <string.h>

extern Player gPlayer;
void init_player();
void update_player();
void switch_state(enum PLAYER_STATE new_state);
Point get_player_point(Player *player);

#endif