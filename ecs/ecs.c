#include "ecs.h"
#include "math/math.h"
#include "minigame/minigame.h"
#include "timer.h"

draw_ctx *ctx;

entity debug_focused_entity = 0;

uint8_t logic_sys_count = 0;
logic_sys_function logic_systems[MAX_SYSTEMS] = {};

uint8_t render_sys_count = 0;
render_sys_function render_systems[MAX_SYSTEMS] = {};

bool setup(){
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

entity eid = 1;

entity create_entity(){
    return eid++;
}

void debug_entity(entity eid){
    debug_focused_entity = eid;
}