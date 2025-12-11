#include "minigame.h"
#include "input/input_environments.h"

minigame_module loaded_game;
bool playing = false;
minigame_finish_callback callback;

void load_minigame(minigame_module mod, minigame_finish_callback cback){
    loaded_game = mod;
    callback = cback;
}

void play_minigame(const char* name){
    register_input_environment(route_minigame, loaded_game.kbd_input, loaded_game.mouse_input);
    switch_input_env(route_minigame);
    playing = true;
}

void end_minigame(bool result){
    if (!playing) return;
    playing = false;
    if (!callback) return;
    callback(result);
}

void minigame_render(draw_ctx *ctx, float dt){
    if (!playing) return;
    loaded_game.render_func(ctx, dt);
}