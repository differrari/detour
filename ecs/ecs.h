#pragma once

#include "engine.h"
#include "string/slice.h"

typedef uint64_t entity; 
typedef void (*logic_sys_function)(float dt);
typedef void (*render_sys_function)(draw_ctx *ctx, float dt);

#define MAX_ENTITIES 1024
#define MAX_SYSTEMS 256

#define COMPONENT(name, values) \
typedef struct \
    values name;\
extern name name##_list[MAX_ENTITIES];\
extern uint64_t name##_category;

#define GET_COMP_ARRAY(name) name##_list
#define GET_COMPONENT(name, index) GET_COMP_ARRAY(name)[index]
#define GET_COMPONENT_PTR(name, index) &GET_COMPONENT(name, index)

#define COMP_IMPL(name,cat) name name##_list[MAX_ENTITIES] = {};\
uint64_t name##_category = cat;

#define TAG(name) \
COMPONENT(name, { bool active; })

TAG(generic_tag)

extern entity debug_focused_entity;

#define FOCUS_PRINT(ftm, ...) if (eid == debug_focused_entity) printf(ftm, ##__VA_ARGS__)

#define make_logic_system_1d(name, comp1, function) \
    static void name(float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = GET_COMPONENT_PTR(comp1,i);\
            if (c1->active){\
                function(i, c1, dt);\
            }\
        }\
    }\

#define make_logic_system(name, comp1, comp2, function) \
    static void name(float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = GET_COMPONENT_PTR(comp1,i);\
            comp2 *c2 = GET_COMPONENT_PTR(comp2,i);\
            if (c1->active && c2->active){\
                function(i, c1, c2, dt);\
            }\
        }\
    }\

#define make_render_system(name, comp1, comp2, function) \
    static void name(draw_ctx *ctx, float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = GET_COMPONENT_PTR(comp1,i);\
            comp2 *c2 = GET_COMPONENT_PTR(comp2,i);\
            if (c1->active && c2->active){\
                function(i, c1, c2, ctx, dt);\
            }\
        }\
    }\

#define make_render_system_1d(name, comp1, function) \
    static void name(draw_ctx *ctx, float dt){\
        for (int i = 1; i < MAX_ENTITIES; i++){\
            comp1 *c1 = GET_COMPONENT_PTR(comp1,i);\
            if (c1->active && c2->active){\
                function(i, c1,  ctx, dt);\
            }\
        }\
    }\

#define has_component(eid, comp) GET_COMPONENT(comp,eid).active

#define set_tag(eid, comp, value) GET_COMPONENT(comp,eid).active = value

#define get_tag(eid, comp) GET_COMPONENT(comp,eid).active

#define get_value(eid, comp, prop) GET_COMPONENT(comp,eid).prop

#define set_value(eid, comp, prop, value) GET_COMPONENT(comp,eid).prop = value

#define find_unique(eid, list, action, fallback) do {\
    bool found = false;\
    float min_distance = FLOAT_MAX;\
    entity min_entity = 0;\
    for (int uid = 1; uid < MAX_ENTITIES; uid++){\
        if (list[uid].active){ \
            if (eid > 0 && GET_COMPONENT(transform,eid).active && GET_COMPONENT(transform,uid).active){\
                float m = vector2_magnitude(vector2_sub(GET_COMPONENT(transform,uid).location, GET_COMPONENT(transform,eid).location));\
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

#define find_all(condition, action) do {\
    for (int uid = 1; uid < MAX_ENTITIES; uid++){\
        if (condition) action;\
    }\
} while(0)

typedef struct {
    string_slice name;
    size_t size;
    uintptr_t data;
    uint64_t id;
    uint64_t category;
} component_data;

void register_component(void* comp_list, size_t comp_size, char* comp_name, uint64_t comp_category);
typedef void (*comp_iter)(component_data data);
void all_components(comp_iter iterator, uint64_t category);
bool entity_has_component(entity ent, component_data data);
uint64_t get_comp_id(string_slice comp_name);
void* get_comp(entity ent, component_data data);

#define COMP_TO_ID(type) get_comp_id(#type,strlen(#type));

#define create_component(eid, type, initializer) do {\
    register_component(GET_COMP_ARRAY(type), sizeof(type), #type, type##_category);\
    type *component = GET_COMPONENT_PTR(type,eid);\
    component->active = true;\
    initializer;\
} while(0)

#define point_collision(point, type, action) do {\
    for (int cid = 1; cid < MAX_ENTITIES; cid++){\
        if (GET_COMPONENT(type,cid).active && GET_COMPONENT(transform,cid).active && point_inside(point, &GET_COMPONENT(transform,cid))){\
            action;\
            break;\
        }\
    }\
} while(0);

#define check_collision(source, type, action) do {\
    if (!GET_COMPONENT(transform,source).active) break;\
    for (int cid = 1; cid < MAX_ENTITIES; cid++){\
        if (cid != source){\
            if (GET_COMPONENT(type,cid).active && GET_COMPONENT(transform,cid).active && collide(&GET_COMPONENT(transform,source),&GET_COMPONENT(transform,cid))){\
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

void input_system(float dt);
void pre_render(draw_ctx *ctx, float dt);
void post_render(draw_ctx *ctx, float dt);

entity create_entity();