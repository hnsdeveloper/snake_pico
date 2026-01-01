#include <stdlib.h>
#include "inc/game_start_state.h"
#include "inc/game_play_state.h"
#include "inc/io.h"

void update_game_start(State* state) {
    ButtonState* buttons = get_current_frame_button_states();

    // If all buttons are pressed, then we start the game
    if(is_button_pressed(buttons, BUTTON_UP) && is_button_pressed(buttons, BUTTON_RIGHT) && 
       is_button_pressed(buttons, BUTTON_DOWN) && is_button_pressed(buttons, BUTTON_LEFT)) {
        State* s = create_game_play_state();
        state->extra_data = s;
    }
}

State* next_state_start(State* state) {
    State* s = (State*)(state->extra_data);
    if(s != NULL) {
        state->extra_data = NULL;
        return s;
    }
    return state;
}

void destroy_state_start(State* state) {
    if(state != NULL) {
        if(state->extra_data != NULL) 
            free(state->extra_data);
        free(state);
    }
}

void draw_state_start(State* state) {
    display_draw_text_justified_left(0,0, "Press 4 buttons to play, 4 buttons to pause when playing.");
}

State* create_game_start_state() {
    State* s = malloc(sizeof(State));
    s->update_fn = &update_game_start;
    s->next_state_fn = &next_state_start;
    s->destroy_state_fn = &destroy_state_start;
    s->state_draw_fn = &draw_state_start;
    s->extra_data = NULL;

    return s;
}