#ifndef _STATE_H_
#define _STATE_H_

#include <stdint.h>

typedef struct State State;

typedef void (*state_update_fn)(State*, uint64_t);
typedef State* (*next_state_fn)(State*);
typedef void (*destroy_state_fn)(State*);
typedef void (*state_draw_fn)(State*);

struct State {
    state_update_fn update_fn;
    next_state_fn next_state_fn;
    destroy_state_fn destroy_state_fn;
    state_draw_fn state_draw_fn;
    void* extra_data;
};

/// @brief Runs an update cycle on a state.
/// @brief Time delta since last frame.
/// @param state The state to run the update cycle.
void update_state(State* state, uint64_t delta);

/// @brief Determines the next state. The function manages the destruction of
/// the previous state if needed.
/// @param state The managed state.
/// @return The same state or the next state on success, NULL on failure.
State* next_state(State* state);

/// @brief Destroys a state.
/// @param state The state to be destroyed.
void destroy_state(State* state);

/// @brief Draws a state.
/// @param state The state to be drawn.
void draw_state(State* state);

#endif