//Detour engine

#pragma once

#include "types.h"
#include "ui/draw/draw.h"
#include "syscalls/syscalls.h"
#include "math/vector.h"

draw_ctx* prepare_graphics();
void commit_graphics(draw_ctx *ctx);

typedef struct primitive_rect {
    vector2 point;
    vector2 size;
} primitive_rect;

void run_game(uint64_t target_fps);
bool setup();
void get_render_ctx(draw_ctx *new_ctx);
void update(float delta);