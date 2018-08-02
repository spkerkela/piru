#include "dungeon.h"

char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
int gDungeonMonsterTable[DUNGEON_SIZE][DUNGEON_SIZE];
char gDungeonWallTable[DUNGEON_SIZE][DUNGEON_SIZE];

bool tile_is_blocked(const Point p) {
  return gDungeonBlockTable[p.y][p.x] || gDungeonMonsterTable[p.y][p.x] >= 0;
}

bool tile_is_blocked_for_monster(const Point p) {
  return tile_is_blocked(p) || (gPlayer.next_x == p.x && gPlayer.next_y == p.y);
}

void init_monster_table() {
  int x, y;

  for (x = 0; x < DUNGEON_SIZE; x++) {
    for (y = 0; y < DUNGEON_SIZE; y++) {
      gDungeonMonsterTable[y][x] = -1;
    }
  }
}

void create_walls() {
  int x, y;
  for (y = 0; y < DUNGEON_SIZE; y++) {
    for (x = 0; x < DUNGEON_SIZE; x++) {
      if (gDungeonBlockTable[y][x]) {
        continue;
      }
      if (gDungeonBlockTable[y - 1][x]) {
        gDungeonWallTable[y][x] |= WALL_NORTH_EAST;
      }
      if (gDungeonBlockTable[y + 1][x]) {
        gDungeonWallTable[y][x] |= WALL_SOUTH_WEST;
      }
      if (gDungeonBlockTable[y][x - 1]) {
        gDungeonWallTable[y][x] |= WALL_NORTH_WEST;
      }
      if (gDungeonBlockTable[y][x + 1]) {
        gDungeonWallTable[y][x] |= WALL_SOUTH_EAST;
      }
    }
  }
}

void create_dungeon() {
  int x, y;
  for (x = 0; x < DUNGEON_SIZE; x++) {
    for (y = 0; y < DUNGEON_SIZE; y++) {
      gDungeonWallTable[y][x] = 0;

      if (x == 0 || x == DUNGEON_SIZE - 1 || y == 0 || y == DUNGEON_SIZE - 1) {
        gDungeon[y][x] = 'w';
        gDungeonBlockTable[y][x] = true;
      } else {
        gDungeon[y][x] = 'f';
        gDungeonBlockTable[y][x] = false;
      }

      if (x > 6 && y > 6) {
        int random = rand();
        bool put_wall = random % 5 == 0;
        if (put_wall) {
          gDungeon[y][x] = 'w';
          gDungeonBlockTable[y][x] = true;
        }
      }
    }
  }

  gDungeon[3][5] = 'w';
  gDungeonBlockTable[3][5] = true;
  gDungeon[2][5] = 'w';
  gDungeonBlockTable[2][5] = true;
  gDungeon[1][5] = 'w';
  gDungeonBlockTable[1][5] = true;
  gDungeon[5][2] = 'w';
  gDungeonBlockTable[5][2] = true;
  gDungeon[5][3] = 'w';
  gDungeonBlockTable[5][3] = true;
  gDungeon[5][4] = 'w';
  gDungeonBlockTable[5][4] = true;
  gDungeon[5][5] = 'w';
  gDungeonBlockTable[5][5] = true;

  init_monster_table();
  create_walls();
}
