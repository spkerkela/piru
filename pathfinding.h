#ifndef _PATHFINDING_H
#define _PATHFINDING_H

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "dungeon.h"
#include "constants.h"
#include "structs.h"
#include "enums.h"

typedef char *Path;

// for iterating over the 8 possible movement directions
extern PathNode *distance_sorted_frontier_list;
extern PathNode *search_stack[MAX_NODES];
extern PathNode *visited_nodes_list;
extern PathNode pre_allocated_nodes[MAX_NODES];
extern char movement_directions_x[8];
extern char movement_directions_y[8];
extern char path_directions[9];
extern int path_nodes_in_use;
extern int reconstructed_path[MAX_PATH_LENGTH];
extern int search_stack_size;

PathNode *get_frontier_node(const Point point);
PathNode *get_new_node();
PathNode *get_next_path_node();
PathNode *get_visited_node(const Point point);
PathNode *path_pop_active_step();

bool find_path(const Point source, const Point destination, Path out_path);
bool path_get_path(PathNode *path, const Point from);
bool path_parent_path(PathNode *path, const Point next_destination, const Point from);
bool path_solid_pieces(PathNode *path, int a, int b);

char get_heuristic_cost(const Point source, const Point destination);

int path_check_equal(PathNode *path, const Point destination);

void path_next_node(PathNode *path);
void path_push_active_step(PathNode *path);
void path_set_coords(PathNode *path);

#endif