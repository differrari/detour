#pragma once

#include "types.h"

typedef struct {
    bool active;
    float amount;
    void (*trigger)();
} game_timer_t;

bool start_timer(float time, void (*function)());
void timer_update(float dt);