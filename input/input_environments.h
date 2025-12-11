#pragma once

#include "mouse_input.h"
#include "keyboard_input.h"

typedef enum { route_world, route_game_ui, route_minigame, route_pause_menu } route_env;

extern route_env environment;

typedef bool (*kbd_handler)(kbd_event event, float dt);
typedef bool (*mouse_handler)(mouse_data event, float dt);

void register_input_environment(route_env environment, kbd_handler keyboard, mouse_handler mouse);

void register_kbd_handle_universal(kbd_handler keyboard);
void register_mouse_handle_universal(mouse_handler mouse);

void switch_input_env(route_env environment);