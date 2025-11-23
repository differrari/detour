#pragma once

#include "engine.h"

typedef uint8_t entity; 
typedef void (*logic_sys_function)(float dt);
typedef void (*render_sys_function)(draw_ctx *ctx, float dt);

#define MAX_ENTITIES 256
#define MAX_SYSTEMS 256

#define COMPONENT(name, values) \
typedef struct \
    values name;\
extern name name##_list[MAX_ENTITIES];

#define COMP_IMPL(name) name name##_list[MAX_ENTITIES];

#define TAG(name) \
COMPONENT(name, { bool active; })

extern entity debug_focused_entity;

#define FOCUS_PRINT(ftm, ...) if (eid == debug_focused_entity) printf(ftm, ##__VA_ARGS__)

#define make_logic_system_1d(name, comp1, function) \
    static void name(float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = &comp1##_list[i];\
            if (c1->active){\
                function(i, c1, dt);\
            }\
        }\
    }\

#define make_logic_system(name, comp1, comp2, function) \
    static void name(float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = &comp1##_list[i];\
            comp2 *c2 = &comp2##_list[i];\
            if (c1->active && c2->active){\
                function(i, c1, c2, dt);\
            }\
        }\
    }\

#define make_render_system(name, comp1, comp2, function) \
    static void name(draw_ctx *ctx, float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = &comp1##_list[i];\
            comp2 *c2 = &comp2##_list[i];\
            if (c1->active && c2->active){\
                function(i, c1, c2, ctx, dt);\
            }\
        }\
    }\

#define make_render_system_1d(name, comp1, function) \
    static void name(draw_ctx *ctx, float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = &comp1##_list[i];\
            if (c1->active && c2->active){\
                function(i, c1,  ctx, dt);\
            }\
        }\
    }\

#define has_component(eid, comp) comp##_list[eid].active

#define set_tag(eid, comp, value) comp##_list[eid].active = value

#define get_tag(eid, comp) comp##_list[eid].active

#define get_value(eid, comp, prop) comp##_list[eid].prop

#define set_value(eid, comp, prop, value) comp##_list[eid].prop = value

#define find_unique(eid, list, action, fallback) do {\
    bool found = false;\
    float min_distance = FLOAT_MAX;\
    entity min_entity = 0;\
    for (int uid = 1; uid < MAX_ENTITIES; uid++){\
        if (list[uid].active){ \
            if (eid > 0 && transform_list[eid].active && transform_list[uid].active){\
                float m = vector2_magnitude(vector2_sub(transform_list[uid].location, transform_list[eid].location));\
                if (m < min_distance){\
                    min_distance = m;\
                    min_entity = uid;\
                    found = true;\
                }\
            } else {\
                found = true; action; \
                break;\
            }\
        }\
    }\
    if (!found){ fallback; }\
    else {\
        entity uid = min_entity;\
        action;\
    }\
} while(0)

#define create_component(eid, type, initializer) do {\
    type *component = &type##_list[eid];\
    component->active = true;\
    initializer;\
} while(0)

#define check_collision(source, type, action) do {\
    if (!transform_list[source].active) break;\
    for (int cid = 1; cid < MAX_ENTITIES; cid++){\
        if (cid != source){\
            if (type##_list[cid].active && transform_list[cid].active && collide(&transform_list[source],&transform_list[cid])){\
                action;\
                break;\
            }\
        }\
    }\
} while(0);

#define register_system(func, level) level##_systems[level##_sys_count++] = func

#define run_logic_systems do {\
    for (int i = 0; i < logic_sys_count; i++)\
        logic_systems[i](dt);\
} while(0);

#define run_render_systems do {\
    for (int i = 0; i < render_sys_count; i++)\
        render_systems[i](ctx, dt);\
} while(0);

extern uint8_t logic_sys_count;
extern logic_sys_function logic_systems[MAX_SYSTEMS];
extern uint8_t render_sys_count;
extern render_sys_function render_systems[MAX_SYSTEMS];

extern draw_ctx *ctx;

bool register_systems();
bool create_entities();

void debug_entity(entity eid);

void manual_logic(float dt);
void manual_render(draw_ctx *ctx);

entity create_entity();