#pragma once

#include <SDL2/SDL.h>

/** @defgroup log Log
 *
 * This module provides a set of macros to avoid mentionning the verbosely
 * named `SDL_LOG_CATEGORY_APPLICATION` parameter when using SDL's logging
 * facility.
 *
 * @{
 */

#define oshu_log_verbose(...)  SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define oshu_log_debug(...)    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define oshu_log_info(...)     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define oshu_log_warn(...)     SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define oshu_log_error(...)    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)
#define oshu_log_critical(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__)

/** @} */
