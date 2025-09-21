//Detour engine

#pragma once

#include "platform.h" 

typedef struct ivector2 {
    float x,y;
} ivector2;

typedef struct primitive_rect {
    ivector2 point;
    ivector2 size;
} primitive_rect;

void run_game(uint64_t target_fps);
void setup();
void get_render_ctx(draw_ctx *new_ctx);
void update(float delta);