/**
 * \file game/osu.c
 * \ingroup game-modes
 *
 * Implement the osu! standard mode.
 */

#include "log.h"
#include "game/game.h"
#include "game/modes.h"
#include "graphics/draw.h"

/**
 * Find the first clickable hit object that contains the given x/y coordinates.
 *
 * A hit object is clickable if it is close enough in time and not already
 * clicked.
 *
 * If two hit objects overlap, yield the oldest unclicked one.
 */
static struct oshu_hit* find_hit(struct oshu_game *game, struct oshu_point p)
{
	double now = game->audio->current_timestamp;
	for (struct oshu_hit *hit = game->beatmap->hit_cursor; hit; hit = hit->next) {
		if (hit->time > now + game->beatmap->difficulty.approach_time)
			break;
		if (hit->time < now - game->beatmap->difficulty.approach_time)
			continue;
		if (hit->state != OSHU_HIT_INITIAL)
			continue;
		double dist = oshu_distance(p, hit->p);
		if (dist <= game->beatmap->difficulty.circle_radius)
			return hit;
	}
	return NULL;
}

/**
 * Get the current mouse position, get the hit object, and change its state.
 *
 * Play a sample depending on what was clicked, and when.
 */
static void hit(struct oshu_game *game)
{
	struct oshu_point mouse = oshu_get_mouse(game->display);
	double now = game->audio->current_timestamp;
	struct oshu_hit *hit = find_hit(game, mouse);
	if (hit) {
		if (fabs(hit->time - now) < game->beatmap->difficulty.leniency) {
			if (hit->type & OSHU_HIT_SLIDER && hit->slider.path.type) {
				hit->state = OSHU_HIT_SLIDING;
				game->osu.current_slider = hit;
			} else {
				hit->state = OSHU_HIT_GOOD;
			}
			oshu_sample_play(game->audio, game->hit_sound);
		} else {
			hit->state = OSHU_HIT_MISSED;
		}
	}
}

/**
 * When the user is holding a slider or a hold note in mania mode, do
 * something.
 */
static void release_hit(struct oshu_game *game)
{
	struct oshu_hit *hit = game->osu.current_slider;
	if (!hit)
		return;
	if (!(hit->type & OSHU_HIT_SLIDER))
		return;
	double now = game->audio->current_timestamp;
	if (now < oshu_hit_end_time(hit) - game->beatmap->difficulty.leniency) {
		hit->state = OSHU_HIT_MISSED;
	} else {
		hit->state = OSHU_HIT_GOOD;
		oshu_sample_play(game->audio, game->hit_sound);
	}
	game->osu.current_slider = NULL;
}

/**
 * Check the state of the current slider.
 *
 * Make it end if it's done.
 */
static void check_slider(struct oshu_game *game)
{
	struct oshu_hit *hit = game->osu.current_slider;
	if (!hit)
		return;
	if (!(hit->type & OSHU_HIT_SLIDER))
		return;
	double now = game->audio->current_timestamp;
	double t = (now - hit->time) / hit->slider.duration;
	double prev_t = (game->previous_time - hit->time) / hit->slider.duration;
	if (now > oshu_hit_end_time(hit)) {
		game->osu.current_slider = NULL;
		hit->state = OSHU_HIT_GOOD;
		oshu_sample_play(game->audio, game->hit_sound);
	} else if ((int) t > (int) prev_t && prev_t > 0) {
		oshu_sample_play(game->audio, game->hit_sound);
	}
	if (game->autoplay)
		return;
	struct oshu_point ball = oshu_path_at(&hit->slider.path, t);
	struct oshu_point mouse = oshu_get_mouse(game->display);
	double dist = oshu_distance(ball, mouse);
	if (dist > game->beatmap->difficulty.slider_tolerance) {
		game->osu.current_slider = NULL;
		hit->state = OSHU_HIT_MISSED;
	}
}

/**
 * Get the audio position and deduce events from it.
 *
 * For example, when the audio is past a hit object and beyond the threshold of
 * tolerance, mark that hit as missed.
 *
 * Also move the beatmap's hit cursor to optimize the beatmap manipulation
 * routines.
 */
static void check_audio(struct oshu_game *game)
{
	double now = game->audio->current_timestamp;
	if (game->autoplay) {
		for (struct oshu_hit *hit = game->beatmap->hit_cursor; hit; hit = hit->next) {
			if (hit->time > now) {
				break;
			} else if (hit->state == OSHU_HIT_INITIAL) {
				if (hit->type & OSHU_HIT_SLIDER && hit->slider.path.type) {
					hit->state = OSHU_HIT_SLIDING;
					game->osu.current_slider = hit;
				} else {
					hit->state = OSHU_HIT_GOOD;
				}
				oshu_sample_play(game->audio, game->hit_sound);
			}
		}
	} else {
		/* Mark dead notes as missed. */
		for (struct oshu_hit *hit = game->beatmap->hit_cursor; hit; hit = hit->next) {
			if (hit->time > now - game->beatmap->difficulty.leniency)
				break;
			if (hit->state == OSHU_HIT_INITIAL)
				hit->state = OSHU_HIT_MISSED;
		}
	}
	for (
		struct oshu_hit **hit = &game->beatmap->hit_cursor;
		*hit && oshu_hit_end_time(*hit) < now - game->beatmap->difficulty.approach_time;
		*hit = (*hit)->next
	);
}

static int init(struct oshu_game *game)
{
	(void)game;
	oshu_log_info("osu mode");
	return 0;
}

static int check(struct oshu_game *game)
{
	check_slider(game);
	check_audio(game);
	return 0;
}

static int draw(struct oshu_game *game)
{
	double now = game->audio->current_timestamp;
	oshu_draw_beatmap(game->display, game->beatmap, now);
	return 0;
}

static int key_pressed(struct oshu_game *game, SDL_Keysym *key)
{
	switch (key->sym) {
	case SDLK_w:
	case SDLK_x:
	case SDLK_z:
		hit(game);
		break;
	}
	return 0;
}

static int key_released(struct oshu_game *game, SDL_Keysym *key)
{
	switch (key->sym) {
	case SDLK_w:
	case SDLK_x:
	case SDLK_z:
		release_hit(game);
		break;
	}
	return 0;
}

static int mouse_pressed(struct oshu_game *game, Uint8 button)
{
	hit(game);
	return 0;
}

static int mouse_released(struct oshu_game *game, Uint8 button)
{
	release_hit(game);
	return 0;
}

struct oshu_game_mode oshu_osu_mode = {
	.init = init,
	.check = check,
	.draw = draw,
	.key_pressed = key_pressed,
	.key_released = key_released,
	.mouse_pressed = mouse_pressed,
	.mouse_released = mouse_released,
};
