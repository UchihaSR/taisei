/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "taisei.h"

#include "stage4.h"
#include "stage4_events.h"

#include "global.h"
#include "stage.h"
#include "stageutils.h"
#include "util/glm.h"
#include "resource/model.h"
#include "portrait.h"

PRAGMA(message "Remove when this stage is modernized")
DIAGNOSTIC(ignored "-Wdeprecated-declarations")

/*
 *  See the definition of AttackInfo in boss.h for information on how to set up the idmaps.
 *  To add, remove, or reorder spells, see this stage's header file.
 */







static void stage4_update(void) {
}

static void stage4_spellpractice_start(void) {
	stage4_start();
	skip_background_anim(stage4_update, 3200, &global.frames, NULL);

	global.boss = stage4_spawn_kurumi(BOSS_DEFAULT_SPAWN_POS);
	boss_add_attack_from_info(global.boss, global.stage->spell, true);
	boss_start_attack(global.boss, global.boss->attacks);

	stage_start_bgm("stage4boss");
}

static void stage4_spellpractice_events(void) {
}

void stage4_skip(int t) {
	skip_background_anim(stage4_update, t, &global.timer, &global.frames);
	audio_bgm_seek_realtime(global.timer / (double)FPS);
}

ShaderRule stage4_shaders[] = { stage4_fog, NULL };

StageProcs stage4_procs = {
	.begin = stage4_start,
	.preload = stage4_preload,
	.end = stage4_end,
	.draw = stage4_draw,
	.update = stage4_update,
	.event = stage4_events,
	.shader_rules = stage4_shaders,
	.spellpractice_procs = &stage4_spell_procs,
};

StageProcs stage4_spell_procs = {
	.begin = stage4_spellpractice_start,
	.preload = stage4_preload,
	.end = stage4_end,
	.draw = stage4_draw,
	.update = stage4_update,
	.event = stage4_spellpractice_events,
	.shader_rules = stage4_shaders,
};
