#include "minigame.h"
#include "input/input_environments.h"

minigame_module *loaded_game;
void *game_ctx;
bool playing = false;
minigame_finish_callback callback;

void play_minigame(minigame_module *mod, void* data, minigame_finish_callback cback, draw_ctx *ctx){
    loaded_game = mod;
    register_input_environment(route_minigame, loaded_game->kbd_input, loaded_game->mouse_input);
    switch_input_env(route_minigame);
    game_ctx = data;
    if (loaded_game->init) loaded_game->init(ctx, data);
    callback = cback;
    playing = true;
}

void end_minigame(bool result){
    if (!playing) return;
    playing = false;
    if (!callback) return;
    callback(result,game_ctx);
}

void minigame_render(draw_ctx *ctx, float dt){
    if (!playing) return;
    loaded_game->render_func(ctx, dt);
}