#include "pathfinding.h"

PathNode pre_allocated_nodes[MAX_NODES];
int path_nodes_in_use;

// for iterating over the 8 possible movement directions
char movement_directions_x[8] = {-1, -1, 1, 1, -1, 0, 1, 0};
char movement_directions_y[8] = {-1, 1, -1, 1, 0, -1, 0, 1};

char path_directions[9] = {5, 1, 6, 2, 0, 3, 8, 4, 7};
PathNode *visited_nodes_list;
PathNode *distance_sorted_frontier_list;

PathNode *search_stack[MAX_NODES];
int search_stack_size;

int reconstructed_path[MAX_PATH_LENGTH];

PathNode *get_new_node()
{
    if (path_nodes_in_use >= MAX_NODES)
    {
        printf("Too many nodes!!\n");
        return NULL;
    }
    else
    {

        PathNode *new_node = &pre_allocated_nodes[path_nodes_in_use++];
        memset(new_node, 0, sizeof(PathNode));
        return new_node;
    }
}
bool path_solid_pieces(PathNode *path, int a, int b)
{
    return true;
}
void path_next_node(PathNode *path)
{
    PathNode *current;
    PathNode *next;

    current = distance_sorted_frontier_list;
    next = distance_sorted_frontier_list->next_node;
    if (next)
    {
        do
        {
            if (next->f_score >= path->f_score)
                break;
            current = next;
            next = next->next_node;
        } while (next);
        path->next_node = next;
    }
    current->next_node = path;
}

char get_heuristic_cost(const Point source, const Point destination)
{

    int delta_x = abs(source.x - destination.x);
    int delta_y = abs(source.y - destination.y);
    return 2 * (delta_x + delta_y);
}

PathNode *get_next_path_node()
{
    PathNode *result = distance_sorted_frontier_list->next_node;
    if (result)
    {
        distance_sorted_frontier_list->next_node = result->next_node;
        result->next_node = visited_nodes_list->next_node;
        visited_nodes_list->next_node = result;
    }
    else
    {
        printf("Failed \n");
    }
    return result;
}

PathNode *get_frontier_node(const Point point)
{
    PathNode *result;
    result = distance_sorted_frontier_list;
    do
    {
        result = result->next_node;
    } while (result && (result->x != point.x || result->y != point.y));
    return result;
}

PathNode *get_visited_node(const Point point)
{
    PathNode *result;
    result = visited_nodes_list;
    do
    {
        result = result->next_node;
    } while (result && (result->x != point.x || result->y != point.y));
    return result;
}

int path_check_equal(PathNode *path, const Point destination)
{
    if (path->x == destination.x || path->y == destination.y)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

PathNode *path_pop_active_step()
{
    return search_stack[--search_stack_size];
}

void path_push_active_step(PathNode *path)
{
    search_stack[search_stack_size++] = path;
}

void path_set_coords(PathNode *path)
{
    PathNode *old_path;
    PathNode *active_path;
    char next_g_movement_score;
    PathNode **child_pointer;

    path_push_active_step(path);
    while (search_stack_size)
    {
        old_path = path_pop_active_step();
        child_pointer = old_path->children;
        int i;
        for (i = 0; i < 8; i++)
        {
            active_path = *child_pointer;
            if (!*child_pointer)
                break;

            Point p = {active_path->x, active_path->y};
            if (old_path->g_movement_cost + path_check_equal(old_path, p) <
                active_path->g_movement_cost)
            {
                if (path_solid_pieces(old_path, active_path->x, active_path->y))
                {
                    active_path->parent = old_path;
                    Point p2 = {active_path->x, active_path->y};
                    next_g_movement_score =
                        old_path->g_movement_cost + path_check_equal(old_path, p2);
                    active_path->g_movement_cost = next_g_movement_score;
                    active_path->f_score = next_g_movement_score + active_path->heuristic;
                    path_push_active_step(active_path);
                }
            }
            ++child_pointer;
        }
    }
}

bool path_parent_path(PathNode *path, const Point next_destination, const Point destination)
{
    int next_g_movement_cost;
    char dxdy_heuristic;
    char dxdy_f_score;

    signed int empty_slot;
    PathNode **path_child_pointer;

    char new_heuristic;
    PathNode *visited_node;

    next_g_movement_cost = path->g_movement_cost + path_check_equal(path, next_destination);

    PathNode *next_frontier;
    PathNode *new_node;
    next_frontier = get_frontier_node(next_destination);
    if (next_frontier)
    {
        empty_slot = 0;
        path_child_pointer = path->children;
        do
        {
            if (!*path_child_pointer)
            {
                break;
            }
            ++empty_slot;
            ++path_child_pointer;
        } while (empty_slot < 8);
        path->children[empty_slot] = next_frontier;
        if (next_g_movement_cost < next_frontier->g_movement_cost)
        {
            if (path_solid_pieces(path, next_destination.x, next_destination.y))
            {
                // we'll explore it later, just update
                dxdy_heuristic = next_frontier->heuristic;
                next_frontier->parent = path;
                next_frontier->g_movement_cost = next_g_movement_cost;
                next_frontier->f_score = next_g_movement_cost + dxdy_heuristic;
            }
        }
    }
    else
    {
        visited_node = get_visited_node(next_destination);
        if (visited_node)
        {
            empty_slot = 0;
            path_child_pointer = path->children;
            do
            {
                if (!*path_child_pointer)
                {
                    break;
                }
                ++empty_slot;
                ++path_child_pointer;
            } while (empty_slot < 8);
            path->children[empty_slot] = visited_node;
            if (next_g_movement_cost < visited_node->g_movement_cost)
            {
                if (path_solid_pieces(path, next_destination.x, next_destination.y))
                {
                    dxdy_f_score = next_g_movement_cost + visited_node->heuristic;
                    visited_node->parent = path;
                    visited_node->g_movement_cost = next_g_movement_cost;
                    visited_node->f_score = dxdy_f_score;
                    path_set_coords(visited_node);
                }
            }
        }
        else
        {
            new_node = get_new_node();
            if (!new_node)
            {
                return false;
            }

            new_node->parent = path;
            new_node->g_movement_cost = next_g_movement_cost;
            new_heuristic = get_heuristic_cost(next_destination, destination);
            new_node->heuristic = new_heuristic;
            new_node->f_score = next_g_movement_cost + new_heuristic;
            new_node->x = next_destination.x;
            new_node->y = next_destination.y;
            // add it to the frontier
            path_next_node(new_node);
            empty_slot = 0;
            path_child_pointer = path->children;
            do
            {
                if (!*path_child_pointer)
                    break;
                ++empty_slot;
                ++path_child_pointer;
            } while (empty_slot < 8);
            path->children[empty_slot] = new_node;
        }
    }

    return true;
}

bool path_get_path(PathNode *path, const Point destination, bool (*path_check)(const Point))
{
    int i;
    Point next_destination;

    for (i = 0; i < 8; i++)
    {
        next_destination.x = path->x + movement_directions_x[i];
        next_destination.y = path->y + movement_directions_y[i];
        if (path_check(next_destination))
        {
            continue;
        }
        path_parent_path(path, next_destination, destination);
        if (path_nodes_in_use >= MAX_NODES)
        {
            return false;
        }
    }
    return true;
}

int find_path(const Point source, const Point destination, Path out_path, bool (*path_check)(const Point))
{
    PathNode *path_start;
    char initial_heuristic;
    PathNode *next_node;
    int result;
    PathNode *current_node;
    PathNode **previous_node;
    int path_length;
    bool path_is_full;
    int *step_ptr;
    char step;
    if (destination.x < 0 || destination.y < 0)
    {
        return 0;
    }

    memset(out_path, -1, MAX_PATH_LENGTH);
    path_nodes_in_use = 0;
    distance_sorted_frontier_list = get_new_node();
    search_stack_size = 0;
    visited_nodes_list = get_new_node();
    initial_heuristic = get_heuristic_cost(source, destination);

    path_start = get_new_node();
    path_start->g_movement_cost = 0;
    path_start->heuristic = initial_heuristic;
    path_start->x = source.x;
    path_start->y = source.y;
    path_start->f_score = initial_heuristic;

    distance_sorted_frontier_list->next_node = path_start;

    while (true)
    {
        next_node = get_next_path_node();
        if (!next_node)
        {
            printf("Failed to get node\n");
            return 0;
        }
        if (next_node->x == destination.x && next_node->y == destination.y)
        {
            break;
        }
        if (!path_get_path(next_node, destination, path_check))
        {
            return 0;
        }
    }
    current_node = next_node;
    previous_node = &next_node->parent;
    path_length = 0;
    if (*previous_node)
    {
        while (true)
        {
            path_is_full = path_length == MAX_PATH_LENGTH;
            if (path_length >= MAX_PATH_LENGTH)
                break;
            reconstructed_path[++path_length - 1] =
                path_directions[3 * (current_node->y - (*previous_node)->y) - (*previous_node)->x +
                                4 + current_node->x];
            current_node = *previous_node;
            previous_node = &(*previous_node)->parent;
            if (!*previous_node)
            {
                path_is_full = path_length == MAX_PATH_LENGTH;
                break;
            }
        }
        if (path_is_full)
        {
            return 0;
        }
    }
    result = 0;
    if (path_length > 0)
    {
        step_ptr = &reconstructed_path[path_length - 1];
        do
        {
            step = (char)*step_ptr;
            --step_ptr;
            out_path[result++] = step;
        } while (result < path_length);
    }

    return path_length;
}
