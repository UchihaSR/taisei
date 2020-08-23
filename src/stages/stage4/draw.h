/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#ifndef IGUARD_stages_stage4_draw_h
#define IGUARD_stages_stage4_draw_h

#include "taisei.h

#include "color.h"

extern struct Stage4DrawData {
	Color ambient_color;
	vec3 midboss_light_pos;
} stage4_draw_data;

void stage4_drawsys_init(void);

void stage4_draw(void);

#endif
