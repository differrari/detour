#include "engine.h"

draw_ctx* prepare_graphics(){
    draw_ctx *ctx = malloc(sizeof(draw_ctx));
    request_draw_ctx(ctx);
    return ctx;
}

void commit_graphics(draw_ctx *ctx){
    commit_draw_ctx(ctx);
}

void run_game(uint64_t target_fps){
    draw_ctx *ctx = prepare_graphics();
    get_render_ctx(ctx);
    if (!setup()){
        printl("Game setup failed");
        return;
    }
    uint64_t time = get_time();
    uint64_t delta_time;
    uint64_t target_dt = target_fps == 0 ? 0 : (1.f/target_fps)*1000;
    while (!should_close_ctx()){
        float dt = delta_time/1000.f;
        update(dt);

        commit_graphics(ctx);
        uint64_t new_time = get_time();
        delta_time = new_time - time;
        time = new_time;
        if (delta_time < target_dt){
            msleep(target_dt - delta_time);
            delta_time = target_dt;
        }
    }
}