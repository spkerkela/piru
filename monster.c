#include "monster.h"

extern Player gPlayer;
extern int created_monsters;
bool create_monster(const Point at) {
  if (gDungeonBlockTable[at.y][at.x] || gDungeonMonsterTable[at.y][at.x] >= 0) {
    return false;
  }
  if (created_monsters >= MAX_MONSTERS) {
    return false;
  }
  Monster monster;
  monster.world_x = at.x;
  monster.world_y = at.y;
  monster.pixel_x = 0;
  monster.pixel_y = 0;
  monster.id = created_monsters;
  monster.level = 1;
  monster.state = MONSTER_STANDING;
  monster.next_state = MONSTER_NO_STATE;
  monster.direction = rand() % MONSTER_DIRECTION_COUNT;
  Point monster_target = {monster.world_x, monster.world_y};
  monster.attack_radius = 2.0;
  monster.aggro_radius = 10;
  monster.target = monster_target;
  memset(monster.path, -1, MAX_PATH_LENGTH);
  monster.max_hp = 10;
  monster.hp = monster.max_hp;
  monster.damage = 5;

  monster.animation_frame = 0;
  monster.walk_interval = 300;
  monster.frames_since_walk = 0;
  monster.animation = ANIM_SKELETON_IDLE;
  monster.frames_since_animation_frame = 0;
  monster.animation_intervals[ANIM_SKELETON_ATTACK] = 40;
  monster.animation_intervals[ANIM_SKELETON_IDLE] = 80;
  monster.animation_intervals[ANIM_SKELETON_WALK] = 70;
  gDungeonMonsterTable[at.y][at.x] = created_monsters;
  monsters[created_monsters++] = monster;
  return true;
}

void monster_do_attack(int i) {
  monsters[i].direction =
      monster_get_direction8(monsters[i].world_x, monsters[i].world_y,
                             gPlayer.world_x, gPlayer.world_y);
  monsters[i].state = MONSTER_ATTACKING;
  monsters[i].animation = ANIM_SKELETON_ATTACK;
  memset(monsters[i].path, -1, MAX_PATH_LENGTH);
  monsters[i].point_in_path = 0;
  monsters[i].animation_frame = 0;
  monsters[i].pixel_x = 0;
  monsters[i].pixel_y = 0;
}

void find_path_to_player(int id) {
  memset(monsters[id].path, -1, MAX_PATH_LENGTH);
  monsters[id].point_in_path = 0;
  Point pos = {monsters[id].world_x, monsters[id].world_y};
  Point player_pos = {gPlayer.world_x, gPlayer.world_y};
  int path_length;
  if ((path_length = find_path(pos, player_pos, monsters[id].path,
                               &tile_is_blocked_for_monster))) {
    if (path_length == 1) {
      monster_do_attack(id);
    } else {
      monsters[id].state = MONSTER_MOVING;
      monsters[id].target = player_pos;
    }
  }
}

double get_distance_to_player(int i) {
  Point monster_point = {monsters[i].world_x, monsters[i].world_y};
  Point player_point = {gPlayer.world_x, gPlayer.world_y};
  return get_distance(monster_point, player_point);
}

void monster_do_walk(int i) {
  if (get_distance_to_player(i) <= monsters[i].attack_radius) {
    monster_do_attack(i);
    return;
  }
  char raw_code = monsters[i].path[monsters[i].point_in_path];
  if (raw_code != -1) {

    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    Point direction = get_direction_from_path(code);
    Point check = {direction.x + monsters[i].world_x,
                   direction.y + monsters[i].world_y};
    if (tile_is_blocked(check)) {
      find_path_to_player(i);
      return;
    }

    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = -1;
    monsters[i].world_x += direction.x;
    monsters[i].world_y += direction.y;
    monsters[i].point_in_path++;
    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = i;
    monsters[i].animation = ANIM_SKELETON_WALK;
  } else {
    monsters[i].state = MONSTER_STANDING;
    monsters[i].animation = ANIM_SKELETON_IDLE;
  }
}

void update_monster_attack(int i) {
  if (monsters[i].animation_frame == 8 &&
      monsters[i].previous_animation_frame != 8) {
    if (get_distance_to_player(i) <= monsters[i].attack_radius) {
      char *str = calloc(10, sizeof(char));
      sprintf(str, "%d", monsters[i].damage);
      DamageText dt;
      dt.text = str;
      dt.x = gPlayer.world_x;
      dt.y = gPlayer.world_y;
      dt.r = 255;
      dt.g = 0;
      dt.b = 0;
      push_damage_text(dt);
      gPlayer.hp -= monsters[i].damage;
    }
  }
}

void update_monster_movement(int i) {
  if (gPlayer.world_x != monsters[i].target.x ||
      gPlayer.world_y != monsters[i].target.y) {
    find_path_to_player(i);
  }
  if (monsters[i].frames_since_walk >= monsters[i].walk_interval) {
    monsters[i].frames_since_walk = 0;
    monster_do_walk(i);
    monsters[i].pixel_x = 0;
    monsters[i].pixel_y = 0;
  } else {
    monsters[i].frames_since_walk += gClock.delta;
    double percentage_walked = (double)monsters[i].frames_since_walk /
                               (double)monsters[i].walk_interval;
    if (percentage_walked >= 1.0) {
      percentage_walked = 1.0;
    }
    char raw_code = monsters[i].path[monsters[i].point_in_path];

    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    monsters[i].direction = monster_get_direction_from_path_code(code);
    Point direction = get_direction_from_path(code);
    Point isometric = cartesian_to_isometric(direction);
    int off_x, off_y;
    off_x = (int)(isometric.x * percentage_walked);
    off_y = (int)(isometric.y * percentage_walked);
    monsters[i].pixel_x = off_x;
    monsters[i].pixel_y = off_y;
  }
}

void update_monster(int id) {
  double distance = get_distance_to_player(id);
  switch (monsters[id].state) {
  case MONSTER_DEAD:
    monsters[id].animation = ANIM_SKELETON_DEAD;
    break;
  case MONSTER_MOVING:
    if (distance <= monsters[id].attack_radius) {
      monster_do_attack(id);
    } else {
      update_monster_movement(id);
    }
    break;
  case MONSTER_STANDING:
    if (distance <= monsters[id].attack_radius) {
      monster_do_attack(id);
    } else if (distance <= monsters[id].aggro_radius) {
      find_path_to_player(id);
    } else {
      monsters[id].animation = ANIM_SKELETON_IDLE;
    }
    break;
  case MONSTER_ATTACKING:
    update_monster_attack(id);
    monsters[id].next_state = MONSTER_STANDING;
    break;
  default:
    break;
  }
}