#include "effect.h"

int ground_effect_count;
GroundEffect ground_effects[MAX_GROUND_EFFECTS];
void create_ground_effect(const Point at, GroundEffect effect) {
  if (!gDungeonBlockTable[at.y][at.x]) {
    effect.frames_since_animation_frame = 0;
    effect.active = true;
    effect.animation_frame = 0;
    effect.previous_animation_frame = effect.animation_frame;
    ground_effects[ground_effect_count] = effect;
    gDungeonGroundEffectTable[at.y][at.x] = ground_effect_count;
    ground_effect_count++;
  }
}