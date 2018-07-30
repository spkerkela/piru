#include "damage_text.h"
DamageText damage_text[MAX_DAMAGE_TEXT];
int damage_text_count;

void init_damage_text()
{
  int i;
  damage_text_count = 0;
  for (i = 0; i < MAX_DAMAGE_TEXT; i++)
  {
    memset(&damage_text[i], 0, sizeof(DamageText));
  }
}

DamageText pop_damage_text()
{
  return damage_text[--damage_text_count];
}

void push_damage_text(DamageText damage)
{
  int i;
  for (i = 0; i < MAX_DAMAGE_TEXT; i++)
  {
    if (!damage_text[i].alive)
    {
      damage.alive = true;
      damage.frames_alive = 0;
      damage_text[i] = damage;
      damage_text_count++;
      break;
    }
  }
}

bool update_damage_text(int id)
{
  if (damage_text[id].alive)
  {
    if (damage_text[id].frames_alive < 700)
    {
      damage_text[id].frames_alive += gClock.delta;
      damage_text[id].y_offset -= 3;
    }
    else
    {
      damage_text[id].alive = false;
      damage_text_count--;
    }

    return true;
  }
  return false;
}