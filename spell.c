#include "spell.h"

Spell gSpells[SPELL_COUNT] = {
    {SPELL_BASE_ATTACK, "Attack", SPELL_TYPE_TARGET_ONE, DAMAGE_PHYSICAL, 0,
     1.0, 2.0},
    {SPELL_BASH, "Bash", SPELL_TYPE_TARGET_ONE, DAMAGE_PHYSICAL, 4, 1.5, 5.0},
    {SPELL_STOMP, "Stomp", SPELL_TYPE_TARGET_PLAYER_POSITION, DAMAGE_PHYSICAL,
     8, 0.8, 2.0}};
