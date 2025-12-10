#include "ecs.h"
#include "math/math.h"

draw_ctx *ctx;

entity debug_focused_entity;

uint8_t logic_sys_count;
logic_sys_function logic_systems[MAX_SYSTEMS];

uint8_t render_sys_count;
render_sys_function render_systems[MAX_SYSTEMS];

bool setup(){
    return register_systems() && create_entities();
}

void update(float dt){
    input_system(dt);
    pre_render(ctx);
    run_logic_systems;
    run_render_systems;
    post_render(ctx);
}

entity eid = 1;

entity create_entity(){
    return eid++;
}

void debug_entity(entity eid){
    debug_focused_entity = eid;
}