#include "inc/state.h"

#include <stdlib.h>

void update_state(State* state, uint64_t delta) {
    if (state != NULL) state->update_fn(state, delta);
}

State* next_state(State* state) {
    State* ret = NULL;
    if (state != NULL) {
        ret = state->next_state_fn(state);
        if (ret != state) destroy_state(state);
    }
    return ret;
}

void destroy_state(State* state) {
    if (state != NULL) state->destroy_state_fn(state);
}

void draw_state(State* state) {
    if (state != NULL) state->state_draw_fn(state);
}
