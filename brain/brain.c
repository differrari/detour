#include "brain.h" 

void render_lookat_debug(entity eid, look_at *l, transform *t, draw_ctx*ctx, float dt){
    draw_debug_line(vector2_add(t->location, l->eye), vector2_add(t->location, l->target), 0xFFFF0000);
}

void enqueue_activity(entity eid, activity act){
    if (!GET_COMPONENT(activity_queue,eid).active) GET_COMPONENT(activity_queue,eid).active = true;
    activity_queue *q = &GET_COMPONENT(activity_queue,eid);
    if (q->activity_ptr >= 15) {
        printf("Activity queue full for entity %i",eid);
        return;
    }
    q->activity_queue[q->activity_ptr++] = act;
}

activity dequeue_activity(entity eid){
    if (!GET_COMPONENT(activity_queue,eid).active) GET_COMPONENT(activity_queue,eid).active = true;
    activity_queue *q = &GET_COMPONENT(activity_queue,eid);
    if (q->activity_ptr <= 0) {
        printf("Activity queue empty for entity %i",eid);
        return (activity){};
    }
    return q->activity_queue[(q->activity_ptr--) - 1];
}

void clear_activities(entity eid){
    if (!GET_COMPONENT(activity_queue,eid).active) GET_COMPONENT(activity_queue,eid).active = true;
    activity_queue *q = &GET_COMPONENT(activity_queue,eid);
    q->activity_ptr = 0;
}

void handle_ai(entity eid, human_brain_t *ai, float dt){
    if (ai->current_activity.state == brain_none){
        // printf("Dequeueing activity %i",ai->current_activity.state);
        ai->current_activity = dequeue_activity(eid);
        if (ai->current_activity.state == brain_none) ai->current_activity.state = brain_choose_goal;
    }
    if (ai->current_activity.state != brain_choose_goal) return;
    clear_activities(eid);
    int need_count = 0;
    generic_need **needs = get_need_array(eid, &need_count);
    float max_priority = 0;
    generic_need *max_need = 0;
    for (int i = 0; i < 4; i++){
        if (needs[i]->active && needs[i]->cooldown > 0){
            needs[i]->cooldown -= dt;
            continue;
        }
        if (needs[i]->active && needs[i]->priority > max_priority){
            max_need = needs[i];
            max_priority = needs[i]->priority;
        }
        if (needs[i]->triggered == trigger_add) needs[i]->triggered = trigger_remove;
    }
    if (max_need){
        max_need->cooldown = 5.f;
        max_need->triggered = trigger_add;
    }
}