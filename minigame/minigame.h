#pragma once
#include "input/input_environments.h"
#include "engine.h"

typedef void (*minigame_finish_callback)(bool);

typedef struct {
    const char *name;
    kbd_handler kbd_input;
    mouse_handler mouse_input;
    void (*render_func)(draw_ctx *, float);
} minigame_module;

void load_minigame(minigame_module mod, minigame_finish_callback callback);
void play_minigame(const char* name);
void end_minigame(bool result);

void minigame_render(draw_ctx *ctx, float dt);