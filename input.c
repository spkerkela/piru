#include "input.h"

MouseCursor gCursor;
bool gShiftIsDown = false;
Point gSelectedTile;
extern bool gGameRunning;
extern bool gGamePaused;

void handle_cursor() {
  Point player_position = {gPlayer.world_x, gPlayer.world_y};
  SDL_GetMouseState(&gCursor.x, &gCursor.y);
  Point mouse_point;
  Point offset = cartesian_to_isometric(player_position);
  mouse_point.x = gCursor.x - (SCREEN_WIDTH / 2) + offset.x;
  mouse_point.y = gCursor.y - (SCREEN_HEIGHT / 2) + offset.y;
  gSelectedTile = isometric_to_cartesian(mouse_point);
  int monster_clicked = -1;
  if (gSelectedTile.x >= 0 && gSelectedTile.y >= 0 &&
      gSelectedTile.x < DUNGEON_SIZE - 1 &&
      gSelectedTile.y < DUNGEON_SIZE - 1) {
    if (gCursor.leftButtonDown || gCursor.rightButtonDown) {
      Point saveSelect = gSelectedTile;
      int x, y;
      x = gSelectedTile.x;
      y = gSelectedTile.y;
      monster_clicked = gDungeonMonsterTable[y][x];
      int directions_checked = 0;
      // check one tile below as well
      while (monster_clicked < 0 && directions_checked < 3) {
        switch (directions_checked) {
        case 0:
          x = gSelectedTile.x + 1;
          y = gSelectedTile.y + 1;
          break;
        case 1:
          x = gSelectedTile.x;
          y = gSelectedTile.y + 1;
          break;
        case 2:
          x = gSelectedTile.x + 1;
          y = gSelectedTile.y;
          break;
        }
        monster_clicked = gDungeonMonsterTable[y][x];
        directions_checked++;
      }

      if (monster_clicked >= 0) {
        gSelectedTile =
            find_nearest_node_to_monster(monster_clicked, player_position);
        if (gPlayer.state != PLAYER_ATTACKING) {
          gPlayer.target_monster_id = monster_clicked;
        }
      }
      if (gPlayer.state != PLAYER_ATTACKING) {
        if (gCursor.leftButtonDown) {
          gPlayer.active_spell = gPlayer.left_spell;
        } else if (gCursor.rightButtonDown) {
          gPlayer.active_spell = gPlayer.right_spell;
        }
      }
      if (gCursor.leftButtonDown && !gShiftIsDown) {
        gPlayer.new_target = gSelectedTile;
      }
      gSelectedTile = saveSelect;
    }
  }
}

void update_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_MOUSEBUTTONUP) {
      if (e.button.button == SDL_BUTTON_LEFT) {
        gCursor.leftButtonDown = false;
      } else if (e.button.button == SDL_BUTTON_RIGHT) {
        gCursor.rightButtonDown = false;
      }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      if (e.button.button == SDL_BUTTON_LEFT) {
        gCursor.leftButtonDown = true;
      } else if (e.button.button == SDL_BUTTON_RIGHT) {
        gCursor.rightButtonDown = true;
      }
    } else if (e.type == SDL_QUIT) {
      gGameRunning = false;
      break;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_RETURN:
        gGameRunning = false;
        break;
      case SDLK_ESCAPE:
        gGamePaused = !gGamePaused;
        break;
      case SDLK_1:
        gPlayer.hp = gPlayer.max_hp;
        break;
      case SDLK_2:
        gPlayer.mana = gPlayer.max_mana;
        break;
      case SDLK_3:
        gPlayer.right_spell = gSpells[SPELL_BASH];
        break;
      case SDLK_4:
        gPlayer.right_spell = gSpells[SPELL_STOMP];
        break;
      case SDLK_5:
        gPlayer.right_spell = gSpells[SPELL_QUAKE];
        break;
      case SDLK_LSHIFT:
        gShiftIsDown = true;
        gPlayer.target.x = gPlayer.world_x;
        gPlayer.target.y = gPlayer.world_y;
        gPlayer.new_target.x = gPlayer.world_x;
        gPlayer.new_target.y = gPlayer.world_y;
        break;
      default:
        break;
      }
    } else if (e.type == SDL_KEYUP) {
      switch (e.key.keysym.sym) {
      case SDLK_LSHIFT:
        gShiftIsDown = false;
      default:
        break;
      }
    }
  }
  handle_cursor();
}