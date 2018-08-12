#ifndef _PROJECTILE_H
#define _PROJECTILE_H
#include "dungeon.h"
#include "enums.h"
#include "structs.h"
#include "time.h"

extern Projectile projectiles[MAX_PROJECTILES];
extern int projectile_count;
void create_projectile(Projectile projectile, Point at, Point target);
void update_projectile(int id);

#endif