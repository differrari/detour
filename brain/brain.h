#pragma once

#include "ecs/ecs.h"
#include "ecs/ecs_core.h"
#include "math/math.h"
#include "math/rng.h"

typedef enum need_type { bar, inverted_bar } need_type;

typedef enum trigger_type { trigger_none, trigger_remove, trigger_add, } trigger_type;

typedef enum brain_states { brain_none, brain_choose_goal, brain_find_so, brain_use_so, brain_move, brain_delay } brain_states;

typedef struct activity {
    brain_states state;
    uint64_t value;
    vector2 vector_value;
    float float_value;
} activity;

COMPONENT(human_brain_t, {
    bool active;
    activity current_activity;
})

COMPONENT(activity_queue, {
   bool active;
   activity activity_queue[16];
   uint8_t activity_ptr; 
})

void enqueue_activity(entity eid, activity act);

activity dequeue_activity(entity eid);

void clear_activities(entity eid);

#define NEED(name) COMPONENT(name##_need, {\
    bool active;\
    float current;\
    float max;\
    float min;\
    float desired;\
    float input;\
    float delta;\
    float priority;\
    float cooldown;\
    trigger_type triggered;\
})

NEED(generic)

enum bar_fill_type { bar_full, bar_empty, bar_random };

#define create_bar(eid, need, minimum, maximum, delta_value, current_type, desired_type) create_component(eid, need##_need, {\
    component->min = minimum;\
    component->max = maximum;\
    float cur = 0;\
    switch (current_type){\
        case bar_full: cur = maximum; break;\
        case bar_empty: cur = minimum; break;\
        case bar_random: cur = rng_next01f(rng); break;\
    }\
    float des = 0;\
    switch (desired_type){\
        case bar_full: des = maximum; break;\
        case bar_empty: des = minimum; break;\
        case bar_random: des = rng_next01f(rng); break;\
    }\
    component->current = cur;\
    component->desired = des;\
    component->delta = delta_value;\
});

#define need_update_system(comp, min_tag, max_tag)\
static void update_##comp##_need(entity eid, comp##_need *c, float dt){\
    c->current = clampf(c->current + c->input + (c->delta * dt), c->min, c->max);\
    c->input = 0;\
    if (c->current == c->min) set_tag(eid, min_tag, true);\
    if (c->current == c->max) set_tag(eid, max_tag, true);\
    c->priority = (absf(c->current - c->desired) / (c->max - c->min));\
}\
make_logic_system_1d(comp##_need_system, comp##_need, update_##comp##_need)

#define need_trigger_system(comp, trigger_tag)\
static void trigger_##comp(entity eid, comp##_need *c, float dt){\
    if (c->triggered == trigger_add){\
        if (!get_tag(eid, trigger_tag)){\
            set_tag(eid, trigger_tag, true);\
            human_brain_t_list[eid].current_activity.state = brain_none;\
            enqueue_activity(eid, (activity){\
                .state = brain_find_so\
            });\
        }\
    } else if (c->triggered == trigger_remove){\
        if (get_tag(eid, trigger_tag)){\
            set_tag(eid, trigger_tag, false);\
        }\
        c->triggered = trigger_none;\
    }\
}\
make_logic_system_1d(comp##_trigger_system, comp##_need, trigger_##comp)

#define find_so(tag, search_tag, need)\
static void seek_##tag(entity eid, tag *d, human_brain_t *ai, float dt){\
    if (ai->current_activity.state != brain_find_so) return;\
    find_unique(eid, search_tag##_list, {\
        enqueue_activity(eid, ((activity){\
            .state = brain_use_so,\
            .value = uid\
        }));\
        enqueue_activity(eid, ((activity){\
            .state = brain_move,\
            .vector_value = {transform_list[uid].location.x, transform_list[uid].location.y + transform_list[uid].size.y}\
        }));\
        need##_need_list[eid].triggered = trigger_remove;\
        human_brain_t_list[eid].current_activity.state = brain_none;\
    }, {\
        need##_need_list[eid].triggered = trigger_remove;\
        human_brain_t_list[eid].current_activity.state = brain_none;\
    });\
}\
make_logic_system(seek_##need, tag, human_brain_t, seek_##tag)

//TODO: set an activation tag, to only enable the action we want
#define simple_action_system(need, seek)\
static void seek##_simple_action(entity eid, human_brain_t *ai, need##_need *n, float dt){\
    if (ai->current_activity.state != brain_use_so) return;\
    entity so = (entity)ai->current_activity.value;\
    seek *action = &seek##_list[so];\
    if (!action->active) return;\
    ai->current_activity.float_value += dt;\
    n->input += dt;\
    if (ai->current_activity.float_value >= action->duration){\
        ai->current_activity.state = brain_none;\
    }\
}\
make_logic_system(need##_basic_use, human_brain_t, need##_need, seek##_simple_action)

COMPONENT(look_at, {
    bool active;
    vector2 eye;
    vector2 target;
})

void render_lookat_debug(entity eid, look_at *l, transform *t, draw_ctx*ctx, float dt);
make_render_system(lookat_debug, look_at, transform, render_lookat_debug)

void path_follow(entity eid, human_brain_t *ai, movement *m, float dt);
make_logic_system(path_following, human_brain_t, movement, path_follow)

generic_need** get_need_array(int eid, int *amount);

void handle_ai(entity eid, human_brain_t *ai, float dt);
make_logic_system_1d(brain_system, human_brain_t, handle_ai)

typedef enum { pf_not_determined, pf_in_progress, pf_incomplete, pf_invalid, pf_finished } pathfind_status;

COMPONENT(path_follower, {
    bool active;
    pathfind_status status;
    vector2 queue[9];
    int count;
    int ptr;
})