# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PIRU is an old-school isometric ARPG (Action RPG) written in C using SDL2. It features procedural dungeon generation (BSP), A* pathfinding, and a state machine architecture for entities.

## Build & Run Commands

```bash
scons              # Build the game (requires SDL2, SDL2-mixer, SDL2-ttf, SDL2-image)
scons -c           # Clean build artifacts
./piru             # Run the game
```

## Testing

Tests use MinUnit (a minimal C unit testing framework in `tests/minunit.h`) with mock SDL2 headers so tests compile without SDL2 installed.

```bash
cd tests && make test              # Build and run all tests
cd tests && make test_piru_math    # Build and run a single test suite
cd tests && make clean             # Clean test build artifacts
```

Available test suites: `test_piru_math`, `test_point`, `test_direction`, `test_pathfinding`, `test_dungeon`.

To add a new test suite, add a new `test_*.c` file in `tests/`, add a corresponding make target in `tests/Makefile` linking the source files under test, and add the target name to the `TESTS` variable.

## Architecture

**Coordinate system:** The game uses Cartesian coordinates internally and converts to isometric projection for rendering. Conversion functions are in `point.c`. Tile size is 64x32 (isometric). Dungeon grid is 112x112.

**Entity state machines:** Both `Player` and `Monster` use function pointer-based state machines (`player_state_fn` / `monster_state_fn`). States are STANDING, MOVING, ATTACKING (and DEAD for monsters). State transition logic lives in `player.c` and `monster.c`.

**Dungeon generation:** `dungeon.c` uses BSP (Binary Space Partition) to generate rooms and corridors. Multiple global 2D arrays track tile data (`gDungeon`), collision (`gDungeonBlockTable`), visibility (`gDungeonVisibleTable`), monsters (`gDungeonMonsterTable`), and ground effects (`gDungeonGroundEffectTable`).

**Pathfinding:** `pathfinding.c` implements A* with a pre-allocated node pool (MAX_NODES=300) and 8-directional movement.

**Direction system:** Players have 16 directions, monsters have 8. Direction calculation from angles is in `direction.c`.

**Global state:** The codebase uses global variables extensively for game state (dungeon grids, cursor, entities). This is typical for C game engines.

**Main loop:** `piru.c` handles SDL initialization, the character creation menu, and the main update/render loop.
