#ifndef _PROJECTILE_H
#define _PROJECTILE_H
#include "enums.h"
#include "structs.h"

extern Projectile projectiles[MAX_PROJECTILES];
void create_projectile(Projectile projectile, Point target);
void update_projectile(int id);

#endif