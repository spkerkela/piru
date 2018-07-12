#include "dungeon.h"

char gDungeon[DUNGEON_SIZE][DUNGEON_SIZE];
bool gDungeonBlockTable[DUNGEON_SIZE][DUNGEON_SIZE];
bool tile_is_blocked(const Point p)
{
    return gDungeonBlockTable[p.y][p.x];
}

void create_dungeon()
{
    int x, y;
    for (x = 0; x < DUNGEON_SIZE; x++)
    {
        for (y = 0; y < DUNGEON_SIZE; y++)
        {
            if (x == 0 || x == DUNGEON_SIZE - 1 || y == 0 || y == DUNGEON_SIZE - 1)
            {
                gDungeon[y][x] = 'w';
                gDungeonBlockTable[y][x] = true;
            }
            else
            {
                gDungeon[y][x] = 'f';
                gDungeonBlockTable[y][x] = false;
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
}
