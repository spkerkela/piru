#ifndef _DAMAGE_TEXT_H
#define _DAMAGE_TEXT_H

#include "structs.h"
#include "time.h"
#include "string.h"
#include "constants.h"
extern DamageText damage_text[MAX_DAMAGE_TEXT];
extern int damage_text_count;

DamageText pop_damage_text();
void init_damage_text();
void push_damage_text(DamageText damage);
bool update_damage_text(int id);

#endif