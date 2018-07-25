#include "time.h"

Clock gClock;

void init_clock()
{
  gClock.delta = 0;
  gClock.last_tick_time = 0;
}

void tick()
{
  uint32_t tick_time = SDL_GetTicks();
  gClock.delta = tick_time - gClock.last_tick_time;
  gClock.last_tick_time = tick_time;
}