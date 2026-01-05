#ifndef _GAME_PLAY_STATE_H_
#define _GAME_PLAY_STATE_H_

#include <stdint.h>

#include "inc/state.h"

/// @brief Creates a game play state
/// @param How many blocks should be in a side. The playable area will be side *
/// side.
/// @return A game start state on success, NULL on failure
State* create_game_play_state(uint32_t gameplay_area_side);

#endif