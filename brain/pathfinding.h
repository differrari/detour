#pragma once

#include "types.h"
#include "math/vector.h"

typedef struct {
    int index_a, index_b;
    float weight;
} node_connection;

void bake_pathfinding(vector2 *nodes, int amount, node_connection *connections, int conn_amount);
bool find_path(uint64_t src_node, uint64_t dst_node, vector2 *path, int *path_count);