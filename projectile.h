#ifndef _PROJECTILE_H
#define _PROJECTILE_H
#include "dungeon.h"
#include "enums.h"
#include "structs.h"
#include "time.h"
#include <string.h>

extern Projectile projectiles[MAX_PROJECTILES];
void create_projectile(Projectile projectile, Point at, Point target);
void init_projectiles();
void update_projectile(int id);
int get_projectile_count();

#endif