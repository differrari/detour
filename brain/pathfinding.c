#include "pathfinding.h"
#include "syscalls/syscalls.h"
#include "data/struct/p_queue.h"
#include "std/memory.h"

vector2 *nodes;
int amount;
node_connection *connections;
int conn_amount;

void bake_pathfinding(vector2 *_nodes, int _amount, node_connection *_connections, int _conn_amount){
    nodes = malloc(9 * sizeof(vector2));
    connections = malloc(9 * sizeof(node_connection));
    amount = _amount;
    memcpy(nodes, _nodes, amount * sizeof(vector2));
    memcpy(connections,_connections, _conn_amount * sizeof(node_connection));
    conn_amount = _conn_amount;
    for (int i = 0; i < conn_amount; i++)
        printf("%i -> %i = %f",connections[i].index_a,connections[i].index_b, connections[i].weight);
}

int array_contains(int *array, int count, int value){
    for (int i = 0; i < count; i++)
        if (array[i] == value) return i;
    return -1;
}

typedef struct {
    uint64_t node;
    uint64_t src_node;
} path_node;

int frontier_contains(p_queue_t*root, int value){
    for (int i = 0; i < root->size; i++){
        if (((path_node*)root->array[i].ptr)->node == value) return i;
    }
    return -1;
}

bool find_path(uint64_t src_node, uint64_t dst_node, vector2 *path, int *path_count){
    if (src_node > amount || dst_node > amount) return false;
    int node = src_node;

    path_node front_nodes[amount] = {};

    p_queue_t *frontier = p_queue_create(conn_amount);
    front_nodes[src_node].node = src_node;
    front_nodes[src_node].src_node = src_node;
    p_queue_insert(frontier, &front_nodes[src_node], 0);
    // printf("Inserted node %i",src_node);
    int expanded[amount] = {};
    int expanded_count = 0;
    while (1){
        if (frontier->size == 0){
            p_queue_free(frontier);
            return false;
        }
        float w = frontier->max_priority;
        path_node *node_info = p_queue_pop(frontier);
        node = node_info->node;
        // printf("Inspecting node %i",node);
        if (node == dst_node){
            // printf("Destination found %i",node);
            int count = 0;
            while (node != src_node){
                path[count++] = nodes[node];
                node = node_info->src_node;
                node_info = &front_nodes[node];
            }
            *path_count = count;
            p_queue_free(frontier);
            return true;
        }
        expanded[expanded_count++] = node;
        // printf("Connections %i",conn_amount);
        for (int i = 0; i < conn_amount; i++){
            if (connections[i].index_a == node || connections[i].index_b == node){
                uint64_t n = connections[i].index_a == node ? connections[i].index_b : connections[i].index_a;
                // printf("Has connection with %i",n);
                if (array_contains(expanded, expanded_count, n) != -1){
                    // printf("Node %i already visited",n);
                    continue;
                }
                int findex = frontier_contains(frontier, n);
                if (findex == -1){
                    front_nodes[n].node = n;
                    front_nodes[n].src_node = node;
                    p_queue_insert(frontier, (void*)&front_nodes[n], w + connections[i].weight);
                    // printf("Inserted node %i",n);
                } else {
                    path_node *pnode = frontier->array[findex].ptr;
                    // printf("Already have a connection to %i worth %f",frontier->array[findex].val);
                    if (frontier->array[findex].val < w + connections[i].weight){
                        frontier->array[findex].val = w + connections[i].weight;  
                        pnode->src_node = node;                  
                    }
                }
            } //else printf("Irrelevant connection %i <-> %i",connections[i].index_a,connections[i].index_b);
        }
    }

    p_queue_free(frontier);
    return false;
}