#include "monster.h"

extern Player gPlayer;
extern int created_monsters;
monster_state_fn monster_stand, monster_move, monster_move_offset,
    monster_try_attack, monster_attack, monster_dead;

double get_distance_to_player(int i) {
  return get_distance(get_monster_point(i), get_player_point(&gPlayer));
}

void monster_stand(int id) {
  monsters[id].animation = ANIM_SKELETON_IDLE;
  monsters[id].state = MONSTER_STANDING;
  Point monster_point = get_monster_point(id);
  Point player_point = get_player_point(&gPlayer);
  if (get_distance(monster_point, player_point) <= monsters[id].aggro_radius) {
    monsters[id].next_state_fn = monster_try_attack;
  }
}

void monster_attack(int id) {
  monsters[id].animation = ANIM_SKELETON_ATTACK;
  monsters[id].state = MONSTER_ATTACKING;
  if (monsters[id].animation_frame == 8 &&
      monsters[id].previous_animation_frame != 8) {
    if (get_distance_to_player(id) <= monsters[id].attack_radius) {
      char *str = calloc(10, sizeof(char));
      sprintf(str, "%d", monsters[id].damage);
      DamageText dt;
      dt.text = str;
      dt.x = gPlayer.world_x;
      dt.y = gPlayer.world_y;
      dt.r = 255;
      dt.g = 0;
      dt.b = 0;
      push_damage_text(dt);
      gPlayer.hp -= monsters[id].damage;
    }
  }
  int animFrames =
      animations[monsters[id].animation][monsters[id].direction].columns;
  if (monsters[id].animation_frame >= animFrames - 1) {
    monsters[id].animation_frame = 0;
    monsters[id].next_state_fn = monster_stand;
  }
}

void monster_try_attack(int id) {
  Point monster_point = get_monster_point(id);
  Point player_point = get_player_point(&gPlayer);

  if (get_distance(player_point, monster_point) <= monsters[id].attack_radius) {
    monsters[id].direction = monster_get_direction8(
        monster_point.x, monster_point.y, player_point.x, player_point.y);
    monsters[id].animation_frame = 0;
    monsters[id].next_state_fn = monster_attack;
  } else if (find_path(monster_point, player_point, monsters[id].path,
                       &tile_is_blocked_for_monster)) {
    monsters[id].point_in_path = 0;
    monsters[id].next_state_fn = monster_move_offset;
    monsters[id].target = player_point;
  } else {
    monsters[id].next_state_fn = monster_stand;
  }
}

void monster_move_offset(int id) {
  monsters[id].animation = ANIM_SKELETON_WALK;
  monsters[id].state = MONSTER_MOVING;
  monsters[id].frames_since_walk += gClock.delta;
  double percentage_walked = (double)monsters[id].frames_since_walk /
                             (double)monsters[id].walk_interval;
  if (percentage_walked >= 1.0) {
    percentage_walked = 1.0;
  }
  char raw_code = monsters[id].path[monsters[id].point_in_path];

  enum PATH_CODE code = (enum PATH_CODE)raw_code;
  monsters[id].direction = monster_get_direction_from_path_code(code);
  Point direction = get_direction_from_path(code);
  Point isometric = cartesian_to_isometric(direction);
  int off_x, off_y;
  off_x = (int)(isometric.x * percentage_walked);
  off_y = (int)(isometric.y * percentage_walked);
  monsters[id].pixel_x = off_x;
  monsters[id].pixel_y = off_y;
  if (monsters[id].frames_since_walk >= monsters[id].walk_interval) {
    monsters[id].next_state_fn = monster_move;
  }
}

void monster_do_walk(int i) {

  char raw_code = monsters[i].path[monsters[i].point_in_path];
  if (raw_code != -1) {
    enum PATH_CODE code = (enum PATH_CODE)raw_code;
    Point direction = get_direction_from_path(code);
    int new_x = monsters[i].world_x + direction.x;
    int new_y = monsters[i].world_y + direction.y;
    Point check = {new_x, new_y};

    if (tile_is_blocked_for_monster(check)) {
      monsters[i].next_state_fn = monster_stand;
      return;
    }

    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = -1;
    monsters[i].point_in_path++;
    monsters[i].world_x = new_x;
    monsters[i].world_y = new_y;
    gDungeonMonsterTable[monsters[i].world_y][monsters[i].world_x] = i;
  }
}

void monster_move(id) {
  monsters[id].animation = ANIM_SKELETON_WALK;
  monsters[id].state = MONSTER_MOVING;
  monsters[id].next_state_fn = monster_move_offset;
  monsters[id].frames_since_walk = 0;
  monsters[id].pixel_x = 0;
  monsters[id].pixel_y = 0;
  monster_do_walk(id);
  if (get_distance_to_player(id) <= monsters[id].attack_radius) {
    monster_try_attack(id);
    return;
  }

  Point player_point = get_player_point(&gPlayer);
  if (!point_equal(monsters[id].target, player_point)) {
    monsters[id].next_state_fn = monster_stand;
  }
}

Point get_monster_point(int id) {
  Monster monster = monsters[id];
  Point monster_point = {monster.world_x, monster.world_y};
  return monster_point;
}

Point find_nearest_node_to_monster(int monster_clicked, Point from) {
  // Find nearest free node to monster
  int i;

  Point monster_point = get_monster_point(monster_clicked);
  Point lookup;
  double smallest_distance = 1000.0;
  int dir = -1;
  for (i = 0; i < 8; i++) {
    lookup.x = monster_point.x + movement_directions_x[i];
    lookup.y = monster_point.y + movement_directions_y[i];
    double distance = get_distance(from, lookup);
    if (distance < smallest_distance && !tile_is_blocked(lookup)) {
      smallest_distance = distance;
      dir = i;
    }
  }

  lookup.x = monster_point.x + movement_directions_x[dir];
  lookup.y = monster_point.y + movement_directions_y[dir];
  return lookup;
}

bool create_monster(const Point at) {
  if (point_equal(at, gPlayerLevelSpawn)) {
    return false;
  }
  if (gDungeonBlockTable[at.y][at.x] || gDungeonMonsterTable[at.y][at.x] >= 0) {
    return false;
  }
  if (created_monsters >= MAX_MONSTERS) {
    return false;
  }
  Monster monster;
  monster.next_state_fn = &monster_stand;
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
  monster.aggro_radius = 5;
  monster.target = monster_target;
  memset(monster.path, -1, MAX_PATH_LENGTH);
  monster.max_hp = 100;
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

void monster_dead(id) {
  monsters[id].animation = ANIM_SKELETON_DEAD;
  monsters[id].animation_frame = 0;
  gDungeonMonsterTable[monsters[id].world_y][monsters[id].world_x] = -1;
  monsters[id].next_state_fn = NULL;
}

void update_monster(int id) {
  if (monsters[id].next_state_fn) {
    if (monsters[id].hp <= 0) {
      monsters[id].next_state_fn = monster_dead;
    }
    monsters[id].next_state_fn(id);
  }
}