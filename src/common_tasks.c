/*
 * This software is licensed under the terms of the MIT License.
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2019, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2019, Andrei Alexeyev <akari@taisei-project.org>.
 */

#include "taisei.h"

#include "common_tasks.h"
#include "random.h"
#include "util/glm.h"

DEFINE_EXTERN_TASK(common_drop_items) {
	cmplx p = *ARGS.pos;

	for(int i = 0; i < ITEM_LAST - ITEM_FIRST; ++i) {
		for(int j = ARGS.items.as_array[i]; j; --j) {
			spawn_item(p, i + ITEM_FIRST);
			WAIT(2);
		}
	}
}

void common_move_loop(cmplx *restrict pos, MoveParams *restrict mp) {
	for(;;) {
		move_update(pos, mp);
		YIELD;
	}
}

DEFINE_EXTERN_TASK(common_move) {
	if(ARGS.ent.ent) {
		TASK_BIND(ARGS.ent);
	}

	MoveParams p = ARGS.move_params;
	common_move_loop(ARGS.pos, &p);
}

DEFINE_EXTERN_TASK(common_move_ext) {
	if(ARGS.ent.ent) {
		TASK_BIND(ARGS.ent);
	}

	common_move_loop(ARGS.pos, ARGS.move_params);
}

DEFINE_EXTERN_TASK(common_call_func) {
	ARGS.func();
}

DEFINE_EXTERN_TASK(common_start_bgm) {
	stage_start_bgm(ARGS.bgm);
}

static Projectile *spawn_charge_particle(cmplx target, real dist, const Color *clr) {
	cmplx pos = target + rng_dir() * dist;

	return PARTICLE(
		.sprite = "graze",
		.color = clr,
		.pos = pos,
		.draw_rule = pdraw_timeout_scalefade(2, 0.1*(1+I), 0, 1),
		.move = move_towards(target, rng_range(0.12, 0.16)),
		.timeout = 30,
		// .scale = 0.5+1.5*I,
		.flags = PFLAG_NOREFLECT,
		.layer = LAYER_PARTICLE_HIGH,
	);
}

static void randomize_hue(Color *clr, float r) {
	float h, s, l, a = clr->a;
	color_get_hsl(clr, &h, &s, &l);
	h += rng_sreal() * r;
	*clr = *HSLA(h, s, l, a);
}

static int common_charge_impl(
	int time,
	const cmplx *anchor,
	cmplx offset,
	const Color *color,
	const char *snd_charge,
	const char *snd_discharge
) {
	real dist = 256;
	int delay = 3;
	real rayfactor = 1.0 / time;
	float hue_rand = 0.02;
	SFXPlayID charge_snd_id = snd_charge ? play_sfx(snd_charge) : 0;
	DECLARE_ENT_ARRAY(Projectile, particles, 256);

	// This is just about below LAYER_PARTICLE_HIGH
	// We want it on a separate layer for better sprite batching efficiency,
	// because these sprites are on a different texture than most.
	drawlayer_t blast_layer = LAYER_PARTICLE_PETAL | 0x80;

	int wait_time = 0;

	for(int i = 0; i < time; ++i, wait_time += WAIT(1)) {
		cmplx pos = *anchor + offset;

		ENT_ARRAY_COMPACT(&particles);
		ENT_ARRAY_FOREACH(&particles, Projectile *p, {
			p->move.attraction_point = pos;
		});

		if(i % delay) {
			continue;
		}

		int stage = (5 * i) / time;
		real sdist = dist * glm_ease_quad_out((stage + 1) / 6.0);

		for(int j = 0; j < stage + 1; ++j) {
			Color c = *color;
			randomize_hue(&c, hue_rand);
			color_lerp(&c, RGBA(1, 1, 1, 0), rng_real() * 0.2);
			Projectile *p = spawn_charge_particle(pos, sdist * (1 + 0.1 * rng_sreal()), &c);
			ENT_ARRAY_ADD(&particles, p);
			color_mul_scalar(&c, 0.2);
		}

		Color c = *color;
		randomize_hue(&c, hue_rand);
		color_lerp(&c, RGBA(1, 1, 1, 0), rng_real() * 0.2);
		color_mul_scalar(&c, 0.5);

		ENT_ARRAY_ADD(&particles, PARTICLE(
			.sprite = "blast_huge_rays",
			.color = &c,
			.pos = pos,
			.draw_rule = pdraw_timeout_scalefade(0, 1, 1, 0),
			.move = move_towards(pos, 0.1),
			.timeout = 30,
			.flags = PFLAG_NOREFLECT | PFLAG_MANUALANGLE,
			.scale = glm_ease_bounce_out(rayfactor * (i + 1)),
			.angle = rng_angle(),
			.layer = blast_layer,
		));
	}

	if(snd_discharge) {
		replace_sfx(charge_snd_id, snd_discharge);
	} else {
		stop_sound(charge_snd_id);
	}

	Color c = *color;
	randomize_hue(&c, hue_rand);
	color_mul_scalar(&c, 2.0);

	cmplx pos = *anchor + offset;

	PARTICLE(
		.sprite = "blast_huge_halo",
		.color = &c,
		.pos = pos,
		.draw_rule = pdraw_timeout_scalefade(0, 2, 1, 0),
		.timeout = 30,
		.flags = PFLAG_NOREFLECT,
		.angle = rng_angle(),
		.layer = blast_layer,
	);

	ENT_ARRAY_FOREACH(&particles, Projectile *p, {
		if(!(p->flags & PFLAG_MANUALANGLE)) {
			p->move.attraction = 0;
			p->move.acceleration += cnormalize(p->pos - pos);
			p->move.retention = 0.8;
		}
	});

	assert(time == wait_time);
	return wait_time;
}

int common_charge(int time, const cmplx *anchor, cmplx offset, const Color *color) {
	return common_charge_impl(time, anchor, offset, color, COMMON_CHARGE_SOUND_CHARGE, COMMON_CHARGE_SOUND_DISCHARGE);
}

int common_charge_static(int time, cmplx pos, const Color *color) {
	cmplx anchor = 0;
	return common_charge_impl(time, &anchor, pos, color, COMMON_CHARGE_SOUND_CHARGE, COMMON_CHARGE_SOUND_DISCHARGE);
}

int common_charge_custom(
	int time,
	const cmplx *anchor,
	cmplx offset,
	const Color *color,
	const char *snd_charge,
	const char *snd_discharge
) {
	cmplx local_anchor = 0;
	anchor = anchor ? anchor : &local_anchor;
	return common_charge_impl(
		time,
		anchor,
		offset,
		color,
		snd_charge,
		snd_discharge
	);
}

DEFINE_EXTERN_TASK(common_charge) {
	Color local_color;
	const Color *p_color = ARGS.color_ref;
	if(!p_color) {
		local_color = *NOT_NULL(ARGS.color);
		p_color = &local_color;
	}

	if(ARGS.bind_to_entity.ent) {
		TASK_BIND(ARGS.bind_to_entity);
	}

	common_charge_custom(
		ARGS.time,
		ARGS.anchor,
		ARGS.pos,
		p_color,
		ARGS.sound.charge,
		ARGS.sound.discharge
	);
}

DEFINE_EXTERN_TASK(common_set_bitflags) {
	assume(ARGS.pflags != NULL);
	*ARGS.pflags = ((*ARGS.pflags & ARGS.mask) | ARGS.set);
}

cmplx common_wander(cmplx origin, double dist, Rect bounds) {
	int attempts = 32;
	double angle;
	cmplx dest;
	cmplx dir;

	// assert(point_in_rect(origin, bounds));

	while(attempts--) {
		angle = rng_angle();
		dir = cdir(angle);
		dest = origin + dist * dir;

		if(point_in_rect(dest, bounds)) {
			return dest;
		}
	}

	log_warn("Clipping fallback  origin = %f%+fi  dist = %f  bounds.top_left = %f%+fi  bounds.bottom_right = %f%+fi",
		creal(origin), cimag(origin),
		dist,
		creal(bounds.top_left), cimag(bounds.top_left),
		creal(bounds.bottom_right), cimag(bounds.bottom_right)
	);

	// TODO: implement proper line-clipping here?

	double step = cabs(bounds.bottom_right - bounds.top_left) / 16;
	dir *= step;
	dest = origin;

	while(point_in_rect(dest + dir, bounds)) {
		dest += dir;
	}

	return dest;
}
