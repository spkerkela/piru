#include "projectile.h"

Projectile projectiles[MAX_PROJECTILES];
int projectile_count = 0;

void init_projectiles() {
  projectile_count = 0;
  memset(projectiles, 0, MAX_PROJECTILES - 1);
}

int get_projectile_count() { return projectile_count; }

void create_projectile(Projectile projectile, Point at, Point target) {
  if (projectile_count < MAX_PROJECTILES) {
    projectile.active = true;
    projectile.x = (double)at.x;
    projectile.y = (double)at.y;
    get_normalized(target, &projectile.vel_x, &projectile.vel_y);
    projectile.animation_frame = 0;
    projectile.previous_animation_frame = 0;
    projectile.frames_since_animation_frame = 0;
    projectile.animation_interval = 10;
    projectiles[projectile_count] = projectile;
    projectile_count++;
  }
};

Point get_projectile_position(int i) {
  int x, y;
  Point p;
  x = (int)projectiles[i].x;
  y = (int)projectiles[i].y;
  p.x = x;
  p.y = y;
  return p;
}

void update_projectile(int i) {
  projectiles[i].x +=
      (projectiles[i].speed * projectiles[i].vel_x) * gClock.delta_seconds;
  projectiles[i].y +=
      (projectiles[i].speed * projectiles[i].vel_y) * gClock.delta_seconds;
  Point projectile_position = get_projectile_position(i);
  if (projectile_position.x < 0 || projectile_position.x >= DUNGEON_SIZE ||
      projectile_position.y < 0 || projectile_position.y >= DUNGEON_SIZE ||
      tile_is_blocked(projectile_position)) {
    projectiles[i].active = false;
    projectile_count--;
  }
};