#include "timer.h"

#define TIMERS_MAX 16

timer_t timers[16] = {};
uint8_t timer_count = 0;

bool start_timer(float time, void (*function)()){
    for (int i = 0; i < TIMERS_MAX; i++){
        if (!timers[i].active){
            timers[i].trigger = function;
            timers[i].amount = time;
            timers[i].active = true;
        }
    }
}

void timer_update(float dt){
    for (int i = 0; i < TIMERS_MAX; i++){
        if (timers[i].active) {
            timers[i].amount -= dt;
            if (timers[i].amount < 0){
                if (timers[i].trigger) timers[i].trigger();
                timers[i].active = false;
                timers[i].trigger = 0;
            }
        }
    }
}