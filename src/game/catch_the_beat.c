/**
 * \file game/catch_the_beat.c
 * \ingroup game-modes
 *
 * Implement the osu! catch the beat mode also called "CTB".
 */

#include "log.h"
#include "game/game.h"
#include "game/modes.h"
#include "graphics/draw.h"

static int init(struct oshu_game *game)
{
	oshu_log_info("catch the beat mode");
	if (!game->ctb.is_dash){
		if (!(game->ctb.is_dash = malloc(sizeof(bool))))
			return -1;
		*game->ctb.is_dash = false;
	}
	return 0;
}

static int check(struct oshu_game *game)
{
	(void)game;
	return 0;
}

static int draw(struct oshu_game *game)
{
	(void)game;
	return 0;
}

static int key_pressed(struct oshu_game *game, SDL_Keysym *key)
{
	switch (key->sym) {
		case SDLK_LSHIFT:
			*game->ctb.is_dash = true;
			break;
		case SDLK_LEFT:
		case SDLK_RIGHT:
			oshu_log_debug("move");
	}
	return 0;
}

static int key_released(struct oshu_game *game, SDL_Keysym *key)
{
	switch (key->sym) {
	case SDLK_LSHIFT:
		*game->ctb.is_dash = false;
		break;
	}
	return 0;
}

static int mouse_pressed(struct oshu_game *game, Uint8 button)
{
	(void)game;
	(void)button;
	return 0;
}

static int mouse_released(struct oshu_game *game, Uint8 button)
{
	(void)game;
	(void)button;
	return 0;
}

struct oshu_game_mode oshu_catch_the_beat_mode = {
	.init = init,
	.check = check,
	.draw = draw,
	.key_pressed = key_pressed,
	.key_released = key_released,
	.mouse_pressed = mouse_pressed,
	.mouse_released = mouse_released,
};
