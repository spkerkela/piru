#include "player.h"

Player gPlayer;

player_state_fn stand, move, move_offset, try_attack, attack;

void init_player() {
  gPlayer.next_state_fn = stand;
  gPlayer.world_x = gPlayerLevelSpawn.x;
  gPlayer.world_y = gPlayerLevelSpawn.y;

  gPlayer.previous_world_x = gPlayer.world_x;
  gPlayer.previous_world_y = gPlayer.world_y;
  gPlayer.next_x = -1;
  gPlayer.next_y = -1;
  gPlayer.pixel_x = 0;
  gPlayer.pixel_y = 0;
  gPlayer.attack_radius = 2.0;
  gPlayer.current_game_level = 0;
  gPlayer.direction = PLAYER_SOUTH;
  gPlayer.point_in_path = 0;
  gPlayer.next_state = PLAYER_NO_STATE;
  gPlayer.destination_action = PLAYER_DESTINATION_NONE;
  gPlayer.max_hp = 1000;
  gPlayer.hp = gPlayer.max_hp;
  gPlayer.max_mana = 60;
  gPlayer.mana = 22;
  gPlayer.base_damage_min = 5;
  gPlayer.base_damage_max = 8;

  gPlayer.active_spell = gSpells[SPELL_BASH];
  gPlayer.right_spell = gSpells[SPELL_BASH];
  gPlayer.left_spell = gSpells[SPELL_BASE_ATTACK];
  gPlayer.no_mana_fallback_spell = gSpells[SPELL_BASE_ATTACK];

  Point target = {-1, -1};
  gPlayer.target = target;
  Point new_target = {-1, -1};
  gPlayer.new_target = new_target;

  gPlayer.target_monster_id = -1;
  gPlayer.moving_between_points = false;
  gPlayer.walk_interval = 200;
  gPlayer.frames_since_walk = 0;
  gPlayer.frames_since_animation_frame = 0;
  gPlayer.animation_intervals[ANIM_WARRIOR_ATTACK] = 30;
  gPlayer.animation_intervals[ANIM_WARRIOR_WALK] = 80;
  gPlayer.animation_intervals[ANIM_WARRIOR_IDLE] = 100;
}

Point get_player_point(Player *player) {
  Point p = {player->world_x, player->world_y};
  return p;
}

void clear_player_path(Player *player) {
  player->point_in_path = 0;
  player->target = get_player_point(player);
  player->new_target = player->target;
}

void try_attack(Player *player) {
  Point player_point = get_player_point(player);
  Point monster_point = get_monster_point(player->target_monster_id);
  Spell spell;
  bool enough_mana = player->mana >= player->active_spell.base_mana_cost;
  if (enough_mana) {
    spell = player->active_spell;
  } else {
    spell = player->no_mana_fallback_spell;
  }
  if (spell.type == SPELL_TYPE_TARGET_AREA &&
      get_distance(player_point, gSelectedTile) <= spell.range) {
    player->direction = player_get_direction8(player->world_x, player->world_y,
                                              gSelectedTile.x, gSelectedTile.y);
    player->animation_frame = 0;
    player->next_state_fn = attack;
    player->mana -= spell.base_mana_cost;
  } else if (spell.type == SPELL_TYPE_TARGET_PLAYER_POSITION ||
             spell.type == SPELL_TYPE_TARGET_SELF) {
    player->direction = player_get_direction8(player->world_x, player->world_y,
                                              gSelectedTile.x, gSelectedTile.y);
    player->animation_frame = 0;
    player->next_state_fn = attack;
    player->mana -= spell.base_mana_cost;
  } else if (player->target_monster_id >= 0 &&
             get_distance(player_point, monster_point) <= spell.range) {
    player->direction = player_get_direction8(player->world_x, player->world_y,
                                              monster_point.x, monster_point.y);
    player->animation_frame = 0;
    player->next_state_fn = attack;
    player->mana -= spell.base_mana_cost;
  } else if (player->target_monster_id >= 0 &&
             find_path(player_point,
                       find_nearest_node_to_monster(player->target_monster_id,
                                                    player_point),
                       player->path, &tile_is_blocked)) {
    player->target = monster_point;
    player->point_in_path = 0;
    player->next_state_fn = move;
  } else {
    player->next_state_fn = stand;
    player->target_monster_id = -1;
  }
}

void stand(Player *player) {
  player->animation = ANIM_WARRIOR_IDLE;
  player->state = PLAYER_STANDING;
  if (gCursor.rightButtonDown || player->target_monster_id >= 0) {
    player->next_state_fn = try_attack;
  } else if (!point_equal(player->target, player->new_target)) {
    Point player_point = get_player_point(player);
    player->target = player->new_target;
    if (find_path(player_point, player->target, player->path,
                  &tile_is_blocked)) {
      player->point_in_path = 0;
      player->next_state_fn = move;
    }
  }
}

void player_do_walk(Player *player) {
  char raw_code = player->path[gPlayer.point_in_path];
  enum PATH_CODE code = (enum PATH_CODE)raw_code;
  Point direction = get_direction_from_path(code);

  int new_x = player->world_x + direction.x;
  int new_y = player->world_y + direction.y;

  Point check = {new_x, new_y};
  if (tile_is_blocked(check)) {
    player->next_state_fn = stand;
    player->pixel_x = 0;
    player->pixel_y = 0;
  } else {
    player->world_x = new_x;
    player->world_y = new_y;
    player->direction = player_get_direction_from_path_code(code);
    Point direction = get_direction_from_path(code);
    Point isometric = cartesian_to_isometric(direction);
    player->pixel_x = isometric.x;
    player->pixel_y = isometric.y;
    player->point_in_path++;
    player->frames_since_walk = 0;
    update_fov(get_player_point(player), 5);
  }
}

void move(Player *player) {
  player->previous_world_x = player->world_x;
  player->previous_world_y = player->world_y;
  if (player->world_x == player->target.x &&
      player->world_y == player->target.y) {
    if (player->target_monster_id >= 0) {
      player->next_state_fn = try_attack;
    } else {
      player->next_state_fn = stand;
    }
    return;
  }
  if (!point_equal(player->target, player->new_target)) {
    Point player_position = get_player_point(player);
    player->target = player->new_target;
    if (!find_path(player_position, player->target, player->path,
                   &tile_is_blocked)) {
      player->next_state_fn = stand;
      return;
    } else {
      player->point_in_path = 0;
    }
  }
  char raw_code = player->path[gPlayer.point_in_path];
  if (raw_code == -1) {
    player->next_state_fn = stand;
    return;
  }
  player->animation = ANIM_WARRIOR_WALK;
  player->state = PLAYER_MOVING;
  player->next_state_fn = move_offset;
  player_do_walk(player);
}

void move_offset(Player *player) {

  player->frames_since_walk += gClock.delta;
  double percentage_walked =
      (double)player->frames_since_walk / (double)player->walk_interval;
  if (percentage_walked >= 1.0) {
    percentage_walked = 1.0;
  }
  Point direction = get_direction_from_player_direction(player->direction);
  Point isometric = cartesian_to_isometric(direction);
  int off_x, off_y;
  off_x = (int)(isometric.x * (1.0 - percentage_walked));
  off_y = (int)(isometric.y * (1.0 - percentage_walked));
  player->pixel_x = off_x;
  player->pixel_y = off_y;
  if (player->frames_since_walk >= player->walk_interval) {
    player->next_state_fn = move;
    player->pixel_x = 0;
    player->pixel_y = 0;
  }
}

int player_base_damage(Player *player) {
  int damage = random_between(player->base_damage_min, player->base_damage_max);
  return (int)((double)damage * player->active_spell.dps_multiplier);
}

void handle_target_attack(Player *player) {
  int target_id = player->target_monster_id;
  Point monster_point = get_monster_point(target_id);
  Point player_point = get_player_point(player);

  if (get_distance(player_point, monster_point) <= player->active_spell.range) {
    int base_damage = player_base_damage(player);
    int damage = rand() % 100 < 20 ? base_damage * 2 : base_damage;
    Point monster_point = get_monster_point(target_id);
    RGB_Color color = {255, 255, 0};
    create_damage_text(monster_point, damage, color);
    monsters[target_id].hp -= damage;
  }
}

void handle_area(Player *player, const Point point) {
  Spell spell = player->active_spell;
  int x, y;
  int radius = (int)spell.radius;
  for (y = point.y - radius; y < point.y + radius + 1; y++) {
    for (x = point.x - radius; x < point.x + radius + 1; x++) {
      int base_damage = player_base_damage(player);
      int monster_id = gDungeonMonsterTable[y][x];
      if (monster_id >= 0) {
        int damage = rand() % 100 < 20 ? base_damage * 2 : base_damage;
        RGB_Color color = {255, 255, 0};
        create_damage_text(get_monster_point(monster_id), damage, color);
        monsters[monster_id].hp -= damage;
      }
      if (spell.leaves_ground_effect) {
        GroundEffect effect = spell.effect;
        Point p = {x, y};
        create_ground_effect(p, effect);
      }
    }
  }
}

void handle_target_player_position(Player *player) {
  Point point = get_player_point(player);
  handle_area(player, point);
}

void handle_target_area(Player *player) { handle_area(player, gSelectedTile); }

void attack(Player *player) {
  player->animation = ANIM_WARRIOR_ATTACK;
  player->state = PLAYER_ATTACKING;
  if (player->animation_frame == 8 && player->previous_animation_frame != 8) {
    Spell spell;
    bool enough_mana = player->mana > player->active_spell.base_mana_cost;
    if (enough_mana) {
      spell = player->active_spell;
    } else {
      spell = player->no_mana_fallback_spell;
    }
    switch (spell.type) {
    case SPELL_TYPE_TARGET_ONE:
      handle_target_attack(player);
      break;
    case SPELL_TYPE_TARGET_PLAYER_POSITION:
      handle_target_player_position(player);
      break;
    case SPELL_TYPE_TARGET_AREA:
      handle_target_area(player);
      break;
    default:
      break;
    }
    player->target_monster_id = -1;
  }

  int animFrames = animations[player->animation][player->direction].columns;
  if (player->animation_frame >= animFrames - 1) {
    player->animation_frame = 0;
    player->next_state_fn = stand;
    clear_player_path(player);
  }
}

void update_player() {
  if (gPlayer.next_state_fn) {
    gPlayer.next_state_fn(&gPlayer);
  }
}
