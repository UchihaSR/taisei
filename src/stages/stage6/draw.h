/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#ifndef IGUARD_stages_stage6_draw_h
#define IGUARD_stages_stage6_draw_h

#include "taisei.h"

#include "util/fbpair.h"
#include "stagedraw.h"
#include "stageutils.h"

void stage6_drawsys_init(void);
void stage6_drawsys_shutdown(void);
void stage6_draw(void);

enum {
	NUM_STARS = 200
};

typedef struct Stage6DrawData {

	struct {
		float position[3*NUM_STARS];
	} stars;

} Stage6DrawData;

void baryon_center_draw(Enemy*, int, bool);

extern ShaderRule stage6_bg_effects[];
extern ShaderRule stage6_postprocess[];

uint stage6_towerwall_pos(Stage3D *s3d, vec3 pos, float maxrange);
void stage6_towerwall_draw(vec3 pos);

void elly_spellbg_toe(Boss*, int);
void elly_spellbg_modern_dark(Boss*, int);
void elly_spellbg_modern(Boss*, int);
void elly_spellbg_classic(Boss*, int);

// this hackery is needed for spell practice
#define STG6_SPELL_NEEDS_SCYTHE(s) ((s) >= &stage6_spells.scythe_first && ((s) - &stage6_spells.scythe_first) < sizeof(stage6_spells.scythe)/sizeof(AttackInfo))
#define STG6_SPELL_NEEDS_BARYON(s) ((s) >= &stage6_spells.baryon_first && ((s) - &stage6_spells.baryon_first) < sizeof(stage6_spells.baryon)/sizeof(AttackInfo)+1)

#endif // IGUARD_stages_stage6_stage6_h
