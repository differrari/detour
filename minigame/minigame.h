#pragma once
#include "input/input_environments.h"
#include "engine.h"

typedef void (*minigame_finish_callback)(bool,void*);

typedef struct {
    const char *name;
    kbd_handler kbd_input;
    mouse_handler mouse_input;
    void (*init)(draw_ctx *, void*);
    void (*render_func)(draw_ctx *, float);
} minigame_module;

void play_minigame(minigame_module *mod, void* data, minigame_finish_callback cback, draw_ctx *ctx);
void end_minigame(bool result);

void minigame_render(draw_ctx *ctx, float dt);