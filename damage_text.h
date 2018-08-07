#ifndef _DAMAGE_TEXT_H
#define _DAMAGE_TEXT_H

#include "constants.h"
#include "sdl2.h"
#include "string.h"
#include "structs.h"
#include "time.h"
#include <stdio.h>
extern DamageText damage_text[MAX_DAMAGE_TEXT];
extern int damage_text_count;

DamageText pop_damage_text();
void create_damage_text(Point point, int damage, RGB_Color color);
void init_damage_text();
void push_damage_text(DamageText damage);
bool update_damage_text(int id);

#endif