#pragma once

#include "ecs.h"
#include "image/image.h"//TODO: make it platform-agnostic
#include "math/math.h"
#include "math/aabb2.h"
#include "math/vector.h"
#include "input/input_environments.h"

COMPONENT(transform, {
    bool active;
    vector2 location;
    //rotation
    vector2 size;
    vector2 collision_offset;
    vector2 collision_size;
});

COMPONENT(movement, {
    bool active;
    vector2 velocity;
    vector2 acceleration;
    vector2 max_speed;
    vector2 max_acceleration;
    vector2 min_acceleration;
});

COMPONENT(solid, {
    bool active;
    argbcolor color;
});

typedef enum { layer_bg, layer_objs, layer_npcs, layer_blocking, layer_player } game_layers;

COMPONENT(renderable, {
    bool active;
    uint8_t layer;
})

COMPONENT(sprite, {
    bool active;
    void *base_img;
    void *scaled_img;
    image_info info;
    image_info scaled_info;
    size_t scaled_img_size;
    bool visible;
    bool dirty;
    image_transform transform;
});

COMPONENT(collider, {
    bool active;
    uint64_t layer;
    uint64_t mask;
})

typedef enum colliderype {
    ct_none,
    ct_static,
    ct_dynamic,
    ct_npc,
    ct_environment,
    ct_player
} colliderype;

typedef struct {
    vector2 direction;
    entity hit;
    vector2 hit_location;
    float distance;
} raycast_result;

bool raycast(vector2 start, vector2 end, raycast_result *result);

bool find_movement_collision(entity eid, vector2 direction, float *f, entity *coll);

bool find_collision(entity eid, entity *coll);

TAG(player)

TAG(possessed)

TAG(camera_follow)

void render_system(draw_ctx *ctx, float dt);

void apply_movement(entity eid, transform *t, movement *m, float dt);
make_logic_system(movement_system, transform, movement, apply_movement)

void cam_follow(entity eid, transform *t, possessed *c, float dt);
make_logic_system(camera_follow_system, transform, possessed, cam_follow)

void resize_sprites(entity eid, sprite *s, transform *t, float dt);
make_logic_system(resize_sprite_system, sprite, transform, resize_sprites);

void draw_debug_line(vector2 start, vector2 end, color color);
void draw_debug_bar(entity eid, vector2 start, vector2 size, float fill, color color);

void map_zoom(mouse_input mouse, float dt);
vector2 screentoworld(gpu_point p);
gpu_point worldtoscreen(vector2 p);

static inline bool point_inside(vector2 p, transform *a){
    vector2 loc = vector2_add(a->location,a->collision_offset);
    return 
        loc.x < p.x && 
        loc.x + a->collision_size.x > p.x &&
        loc.y < p.y && 
        loc.y + a->collision_size.y > p.y;
}

static inline bool collide(transform *a, transform *b){
    vector2 sizea = a->collision_size.x && a->collision_size.y ? a->collision_size : a->size;
    vector2 loca = vector2_add(a->location,a->collision_offset);
    vector2 sizeb = b->collision_size.x && b->collision_size.y ? b->collision_size : b->size;
    vector2 locb = vector2_add(b->location,b->collision_offset);
    return aabb2_check_collision((aabb2){loca,vector2_add(loca, sizea)}, (aabb2){locb,vector2_add(locb, sizeb)});
}