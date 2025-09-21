#include "redactedos.h"

draw_ctx* prepare_graphics(){
    draw_ctx *ctx = malloc(sizeof(draw_ctx));
    request_draw_ctx(ctx);
    return ctx;
}

void commit_graphics(draw_ctx *ctx){
    commit_draw_ctx(ctx);
}