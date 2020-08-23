/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "taisei.h"

#include "background_anim.h"
#include "draw.h"

#include "stageutils.h"

TASK(upgrade_stage_3d, NO_ARGS) {
	for(;;) {
		YIELD;
		stage3d_update(&stage_3d_context);
	}
}

TASK(animate_bg_fullstage, NO_ARGS) {
	stage_3d_context.cam.pos[2] = -1.8;
	stage_3d_context.cam.pos[1] = -30;
	stage_3d_context.crot[0] = 80;
	
	stage_3d_context.cam.vel[1] = 0.017;
	stage_3d_context.cam.vel[2] = 0.008;

	WAIT(400);
	for(int i = 0; i < 200; i++) {
		YIELD;
		fapproach_p(&stage_3d_context.cam.vel[1], 0.013f, 0.0001f);
		fapproach_p(&stage_3d_context.cam.vel[2], 0.0f, 0.0001f);
	}
	WAIT(600);
	for(int i = 0; i < 200; i++) {
		YIELD;
		fapproach_p(&stage_3d_context.cam.vel[2], 0.003f, 0.0002f);
	}
	WAIT(1000);
	for(int i = 0; i < 400; i++) {
		YIELD;
		fapproach_p(&stage_3d_context.cam.vel[2], -0.003f, 0.0002f);
	}
	for(;;) {
		YIELD;
		fapproach_p(&stage_3d_context.cam.vel[1], 0.05f, 0.001f);
		fapproach_p(&stage_3d_context.cam.vel[2], 0.0f, 0.001f);
	}
}

void stage4_bg_init_fullstage() {
	memset(&stage4_draw_data, 0, sizeof(stage4_draw_data));
	stage4_draw_data.ambient_color = *RGB(0.1,0.1,0.1);
	
	INVOKE_TASK(upgrade_stage_3d);
	INVOKE_TASK(animate_bg_fullstage);
}
