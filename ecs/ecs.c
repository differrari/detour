#include "ecs.h"
#include "math/math.h"
#include "minigame/minigame.h"
#include "timer.h"
#include "data/struct/chunk_array.h"
#include "data/struct/hashmap.h"
#include "string/slice.h"

draw_ctx *ctx;

entity debug_focused_entity = 0;

uint8_t logic_sys_count = 0;
logic_sys_function logic_systems[MAX_SYSTEMS] = {};

uint8_t render_sys_count = 0;
render_sys_function render_systems[MAX_SYSTEMS] = {};

chunk_array_t *component_directory;
chashmap_t *component_map;

bool setup(){
    component_directory = chunk_array_create(sizeof(component_data),128);
    component_map = chashmap_create(128);
    return register_systems() && create_entities();
}

void update(float dt){
    input_system(dt);
    pre_render(ctx, dt);
    run_logic_systems;
    run_render_systems;
    timer_update(dt);
    post_render(ctx, dt);
    minigame_render(ctx, dt);
}

static entity eid = 1;

entity create_entity(){
    return eid++;
}

void debug_entity(entity eid){
    debug_focused_entity = eid;
}

static uint64_t cid = 1;

uint64_t get_comp_id(string_slice comp_name){
    void *hash = chashmap_get(component_map, comp_name.data, comp_name.length);
    return hash ? *(int64_t*)hash : 0;
}

void register_component(void* comp_list, size_t comp_size, char* comp_name){
    size_t len = strlen(comp_name);
    if (!get_comp_id((string_slice){ .data = comp_name, .length = len})){
        int64_t *nid = zalloc(sizeof(int64_t));
        *nid = cid++;
        component_data comp_data = {
            .data = (uintptr_t)comp_list,
            .name = make_string_slice(comp_name, 0, len),
            .size = comp_size,
            .id = *nid
        };
        chunk_array_push(component_directory, &comp_data);
        chashmap_put(component_map, comp_name, len, nid);
        // print("COMP %s = %i",comp_name,*nid);
    }
}

void all_components(comp_iter iterator){
    size_t count = cid-1;
    for (size_t i = 0; i < count; i++)
        iterator(*(component_data*)chunk_array_get(component_directory, i));
}

bool entity_has_component(entity ent, component_data data){
    return *(bool*)(data.data + (ent *data.size));
}