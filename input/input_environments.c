#include "input_environments.h"
#include "syscalls/syscalls.h"

route_env environment;

kbd_handler keyboard_handlers[4];
mouse_handler mouse_handlers[4];

kbd_handler universal_kbd_handler;
mouse_handler universal_mouse_handler;

void register_kbd_handle_universal(kbd_handler keyboard){
    universal_kbd_handler = keyboard;
}

void register_mouse_handle_universal(mouse_handler mouse){
    universal_mouse_handler = mouse;
}

void register_input_environment(route_env environment, kbd_handler keyboard, mouse_handler mouse){
    keyboard_handlers[environment] = keyboard;
    mouse_handlers[environment] = mouse;
}

void input_system(float dt){
    kbd_handler key_handle = keyboard_handlers[environment];
    if (key_handle || universal_kbd_handler){
        kbd_event event = {};
            while (read_event(&event)){
                if ((!key_handle || !key_handle(event, dt)) && universal_kbd_handler)
                    universal_kbd_handler(event,dt);
            }
        }
    mouse_handler mouse_handle = mouse_handlers[environment];
    if (mouse_handle || universal_mouse_handler){
        mouse_data mouse;
        get_mouse_status(&mouse);
        if ((!mouse_handle || !mouse_handle(mouse, dt)) && universal_mouse_handler)
            universal_mouse_handler(mouse,dt);
    }
}

void switch_input_env(route_env new_environment){
    environment = new_environment;
}