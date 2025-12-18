#include "ecs_core.h"
#include "math/aabb2.h"
#include "math/vector.h"
#include "mouse_input.h"
#include "input_keycodes.h"
#include "syscalls/syscalls.h"
#include "ui/graphic_types.h"

float camera_size = 0;
vector2 camera_pos = {};
extern float camera_scale;
extern bool zoom_changed;

extern vector2 room_size;

COMP_IMPL(transform)
COMP_IMPL(movement)
COMP_IMPL(solid)
COMP_IMPL(sprite)
COMP_IMPL(collider)
COMP_IMPL(player)
COMP_IMPL(possessed)
COMP_IMPL(camera_follow)

#define camera_mult (camera_size * camera_scale)

vector2 screentoworld(gpu_point p){
    return (vector2){((float)p.x/camera_mult)+camera_pos.x,((float)p.y/camera_mult)+camera_pos.y};
}

gpu_point worldtoscreen(vector2 p){
    return (gpu_point){(p.x-camera_pos.x)*camera_mult,(p.y-camera_pos.y)*camera_mult};
}

#define render_adjust(obj_width,obj_height)\
float lx = t->location.x - camera_pos.x;\
float ly = t->location.y - camera_pos.y;\
float locx = lx > 0 ? lx : 0;\
float locy = ly > 0 ? ly : 0;\
float offx = lx < 0 ? absf(lx) * camera_mult : 0;\
float offy = ly < 0 ? absf(ly) * camera_mult : 0;\
uint32_t w = max(0, (obj_width) - offx);\
uint32_t h = max(0, (obj_height) - offy);\
locx *= camera_mult;\
locy *= camera_mult;\
if (locx+w >= ctx->width){\
    w -= (locx+w) - ctx->width;\
    w = max(w,0);\
}\
if (locy+h >= ctx->height){\
    h -= (locy+h) - ctx->height;\
    h = max(h,0);\
}\

void map_zoom(mouse_input mouse, float dt){
    if (mouse.scroll != 0){
        float newScale = (float)mouse.scroll * dt;
        camera_scale = clampf(camera_scale + newScale, 0.5f, 2);
        zoom_changed = true;
    } else zoom_changed = false;
}

void render_solid(entity eid, transform *t, solid *s, draw_ctx *ctx, float dt){
    render_adjust(t->size.x * camera_mult, t->size.y * camera_mult)
    if (w > 0 && h > 0)
        fb_fill_rect(ctx, locx, locy, w, h, s->color.color);
}

void render_sprite(entity eid, transform *t, sprite *s, draw_ctx *ctx, float dt){
    render_adjust(s->scaled_info.width, s->scaled_info.height)

    if (w > 0 && h > 0)
    if (s->visible) fb_draw_partial_img(ctx, s->scaled_img, locx, locy, s->scaled_info.width, s->scaled_info.height, (image_transform){w, h, offx, offy, s->transform.flip_x, s->transform.flip_y});
}

void render_system(draw_ctx *ctx, float dt){
    for (int l = 0; l < 255; l++){
        for (int i = 1; i < MAX_ENTITIES; i++){
            transform *t = &GET_COMPONENT(transform,i);
            renderable *r = &GET_COMPONENT(renderable,i);
            if (t->active && r->active && r->layer == l){
                if (GET_COMPONENT(solid,i).active) render_solid(i, t, &GET_COMPONENT(solid,i), ctx, dt);
                if (GET_COMPONENT(sprite,i).active) render_sprite(i, t, &GET_COMPONENT(sprite,i), ctx, dt);
            }
        }
    }
}

void cam_follow(entity eid, transform *t, possessed *c, float dt){
    camera_pos = vector2_sub(vector2_add(t->location, vector2_scale(t->size,0.5f)), vector2_scale(room_size, 0.5f));
}

void get_render_ctx(draw_ctx *new_ctx){
    ctx = new_ctx;
    camera_size = min(ctx->width/room_size.x,ctx->height/room_size.y);
    resize_draw_ctx(ctx, room_size.x * camera_mult, room_size.y * camera_mult);
}

void draw_debug_line(vector2 start, vector2 end, color color){
    fb_draw_line(ctx, (start.x - camera_pos.x) * camera_mult, (start.y - camera_pos.y) * camera_mult, (end.x - camera_pos.x) * camera_mult, (end.y - camera_pos.y) * camera_mult, color);
}

void draw_debug_bar(entity eid, vector2 start, vector2 size, float fill, color color){
    fill = clampf(fill, 0, 1);

    fb_fill_rect(ctx, (start.x - camera_pos.x) * camera_mult, (start.y - camera_pos.y) * camera_mult, size.x * camera_mult, size.y * camera_mult, 0xFFFFFFFF);
    fb_fill_rect(ctx, (start.x - camera_pos.x) * camera_mult, (start.y - camera_pos.y) * camera_mult, (size.x * fill) * camera_mult, size.y * camera_mult, color);
}

bool find_movement_collision(entity eid, vector2 direction, float *f, entity *coll){
    if (!has_component(eid, collider)) return false;
    transform *t = &GET_COMPONENT(transform,eid);
    vector2 sizea = t->collision_size.x && t->collision_size.y ? t->collision_size : t->size;
    vector2 loca = vector2_add(t->location,t->collision_offset);
    aabb2 a = (aabb2){vector2_add(direction,loca), vector2_add(direction,vector2_add(loca, sizea))};
    uint64_t mask = GET_COMPONENT(collider,eid).mask;
    for (int i = 0; i < MAX_ENTITIES; i++){
        if (i != eid && GET_COMPONENT(collider,i).active && (mask & (1 << GET_COMPONENT(collider,i).layer))){
            transform *t2 = &GET_COMPONENT(transform,i);
            vector2 locb = vector2_add(t2->location,t2->collision_offset);
            vector2 sizeb = t2->collision_size.x && t2->collision_size.y ? t2->collision_size : t2->size;
            aabb2 b = (aabb2){locb, vector2_add(locb, sizeb)};
            if (aabb2_check_collision(a, b)){
                *coll = i;
                return true;
            }
        }
    }
    return false;
}

bool zoom_changed;

void recalculate_collision(entity eid, sprite *s, transform *t){
    uint32_t min_x = s->scaled_info.width; 
    uint32_t min_y = s->scaled_info.height;
    uint32_t max_x = 0; 
    uint32_t max_y = 0;
    uint32_t *img = s->scaled_img;
    for (uint32_t y = 0; y < s->scaled_info.height; y++){
        uint32_t *row = img + (y * s->scaled_info.width);
        for (uint32_t x = 0; x < s->scaled_info.width; x++){
            if (row[x] >> 24){
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }
    t->collision_offset = (vector2){min_x/camera_mult, min_y/camera_mult};
    t->collision_size = (vector2){(max_x - min_x)/camera_mult, (max_y - min_y)/camera_mult};
    FOCUS_PRINT("%fx%f %fx%f",t->collision_offset.x,t->collision_offset.y,t->collision_size.x,t->collision_size.y);
}

void resize_sprites(entity eid, sprite *s, transform *t, float dt){
    if (!zoom_changed && s->scaled_img) return;
    if (s->scaled_img) free_sized(s->scaled_img, s->scaled_img_size);
    s->scaled_img_size = t->size.x * camera_mult * t->size.y * camera_mult * sizeof(uint32_t);
    s->scaled_info.width = t->size.x * camera_mult;
    s->scaled_info.height = t->size.y * camera_mult;
    s->scaled_img = malloc(s->scaled_img_size);
    if (!s->base_img || !s->scaled_img) return;
    rescale_image(s->info.width, s->info.height, t->size.x * camera_mult, t->size.y * camera_mult, s->base_img, s->scaled_img);
    if (t->collision_size.x == 0 || t->collision_size.y == 0){
        recalculate_collision(eid, s, t);
    }
}

bool find_collision(entity eid, entity *coll){
    if (!has_component(eid, collider)) return false;
    transform *t = &GET_COMPONENT(transform,eid);
    vector2 sizea = t->collision_size.x && t->collision_size.y ? t->collision_size : t->size;
    vector2 loca = vector2_add(t->location,t->collision_offset);
    aabb2 a = (aabb2){vector2_add(loca, t->collision_offset), vector2_add(loca, sizea)};
    uint64_t mask = GET_COMPONENT(collider,eid).mask;
    for (int i = 0; i < MAX_ENTITIES; i++){
        transform *t2 = &GET_COMPONENT(transform,i);
        vector2 sizeb = t2->collision_size.x && t2->collision_size.y ? t2->collision_size : t2->size;
        vector2 locb = vector2_add(t2->location,t2->collision_offset);
        aabb2 b = (aabb2){vector2_add(locb, t2->collision_offset), vector2_add(locb, sizeb)};
        if (i != eid && has_component(eid, collider) && (mask & (1 << GET_COMPONENT(collider,i).layer)) && aabb2_check_collision(a, b)){
            *coll = i;
            return true;
        }
    }
    return false;
}

void apply_movement(entity eid, transform *t, movement *m, float dt){
    if (m->acceleration.x)
        m->velocity.x = clampf(m->velocity.x + (m->acceleration.x * dt), -m->max_speed.x, m->max_speed.x);
    else if (m->velocity.x != 0)
        m->velocity.x = lerpf(m->velocity.x, 0, dt);

    if (m->acceleration.y)
        m->velocity.y = clampf(m->velocity.y + (m->acceleration.y * dt), -m->max_speed.y, m->max_speed.y);
    else if (m->velocity.y != 0)
        m->velocity.y = lerpf(m->velocity.y, 0, dt);

    vector2 dir = (vector2){m->velocity.x * dt,m->velocity.y * dt};

    float hit = 0;
    entity e = 0;
    if (!vector2_zero(dir)){
        if (has_component(eid, collider)){
            if (!find_movement_collision(eid, dir, &hit, &e)){
                t->location.x += dir.x;
                t->location.y += dir.y;
            } else m->velocity = (vector2){0,0};
        } else {
            t->location.x += dir.x;
            t->location.y += dir.y;
        }
    }
}

bool raycast(vector2 start, vector2 end, raycast_result *result){
    draw_debug_line(start, end, 0xFF0000FF);
    vector2 dir = (vector2){end.x-start.x,end.y-start.y};
    result->distance = vector2_magnitude(dir);
    result->direction = (vector2){dir.x/result->distance,dir.y/result->distance};
    return true;
}