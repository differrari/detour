#pragma once

#include "types.h"
#include "ui/draw/draw.h"
#include "syscalls/syscalls.h"

draw_ctx * prepare_graphics();
void commit_graphics(draw_ctx *ctx);
