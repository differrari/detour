#include "ai.h" 

void render_lookat_debug(entity eid, look_at *l, transform *t, draw_ctx*ctx, float dt){
    draw_debug_line(vector2_add(t->location, l->eye), vector2_add(t->location, l->target), 0xFFFF0000);
}

void enqueue_activity(entity eid, activity act){
    if (!activity_queue_list[eid].active) activity_queue_list[eid].active = true;
    activity_queue *q = &activity_queue_list[eid];
    if (q->activity_ptr >= 15) {
        printf("Activity queue full for entity %i",eid);
        return;
    }
    q->activity_queue[q->activity_ptr++] = act;
}

activity dequeue_activity(entity eid){
    if (!activity_queue_list[eid].active) activity_queue_list[eid].active = true;
    activity_queue *q = &activity_queue_list[eid];
    if (q->activity_ptr <= 0) {
        printf("Activity queue empty for entity %i",eid);
        return (activity){};
    }
    return q->activity_queue[(q->activity_ptr--) - 1];
}

void clear_activities(entity eid){
    if (!activity_queue_list[eid].active) activity_queue_list[eid].active = true;
    activity_queue *q = &activity_queue_list[eid];
    q->activity_ptr = 0;
}