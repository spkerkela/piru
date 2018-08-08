#include "effect.h"

GroundEffect ground_effects[MAX_GROUND_EFFECTS];
void create_ground_effect(const Point at, GroundEffect effect) {
  if (!gDungeonBlockTable[at.y][at.x]) {
    int i;
    for (i = 0; i < MAX_GROUND_EFFECTS; i++) {
      if (ground_effects[i].active) {
        continue;
      } else {
        effect.frames_since_animation_frame = 0;
        effect.active = true;
        effect.animation_frame = 0;
        effect.previous_animation_frame = effect.animation_frame;
        ground_effects[i] = effect;
        effect.x = at.x;
        effect.y = at.y;
        gDungeonGroundEffectTable[at.y][at.x] = i;
        break;
      }
    }
  }
}