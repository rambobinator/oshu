/**
 * \file game/modes.h
 * \ingroup game-modes
 *
 * Define the game module.
 */

#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>

/**
 * \defgroup game-modes Modes
 * \ingroup game
 *
 * \brief
 * Generic interface to all game modes.
 *
 * \{
 */

struct oshu_game;

/**
 * A generic mode is defined by a structure of callbacks.
 *
 * All these callbacks take the game state, and return 0 on success, -1 on
 * error. An error is likely to stop the game in the future, so make sure it's
 * worth it.
 *
 * When a callback is null, it means there's nothing to do, and that's
 * perfectly fine. For example, the taiko or mania modes have no reason to use
 * the mouse.
 */
struct oshu_game_mode {
	/**
	 * Called one time during game initialization.
	 */
	int (*init)(struct oshu_game *game);
	/**
	 * Called at every game iteration, unless the game is paused.
	 *
	 * The job of this function is to check the game clock and see if notes
	 * were missed, or other things of the same kind.
	 *
	 * There's no guarantee this callback is called at regular intervals.
	 */
	int (*check)(struct oshu_game *game);
	/**
	 * Draw the game on the display.
	 */
	int (*draw)(struct oshu_game *game);
	/**
	 * Handle a key press keyboard event.
	 *
	 * Key repeats are filtered out by the parent module, along with any
	 * key used by the game module itself, like escape or space to pause, q
	 * to quit, &c.
	 *
	 * This callback isn't called when the game is paused or on autoplay.
	 *
	 * \sa key_released
	 */
	int (*key_pressed)(struct oshu_game *game, SDL_Keysym *key);
	/**
	 * See #key_released.
	 */
	int (*key_released)(struct oshu_game *game, SDL_Keysym *key);
	/**
	 * Handle a mouse button press event.
	 *
	 * Mouse click events may be filtered by the parent game module in the
	 * future to implement mode-agnotistic features, but you should not
	 * worry about it.
	 *
	 * If you need the mouse position, use #oshu_get_mouse to have it in
	 * game coordinates.
	 *
	 * This callback isn't called when the game is paused or on autoplay.
	 *
	 * \sa mouse_released.
	 */
	int (*mouse_pressed)(struct oshu_game *game, Uint8 button);
	/**
	 * See #mouse_released.
	 */
	int (*mouse_released)(struct oshu_game *game, Uint8 button);
};

/**
 * Parts of the game state specific to the Osu! standard mode.
 */
struct oshu_osu_state {
	/**
	 * Slider hit object the user is holding.
	 *
	 * NULL most of the time.
	 */
	struct oshu_hit *current_slider;
};

/**
 * Parts of the game state specific to the Osu! catch the beat mode.
 */
struct oshu_catch_the_beat_state {
	/**
	 * Dash state.
	 */
	bool *is_dash;
};

/**
 * Implementation of the standard osu! game mode.
 */
extern struct oshu_game_mode oshu_osu_mode;

/**
 * Implementation of the catch the beat osu! game mode.
 */
extern struct oshu_game_mode oshu_catch_the_beat_mode;

/** \} */
