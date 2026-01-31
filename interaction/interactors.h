#pragma once

#include "ecs/ecs.h"

#define INTERACTOR(name) COMPONENT(name##_interactor, {\
    bool active;\
    uint64_t target_component;\
    string_slice readable_action;\
})

INTERACTOR(base)