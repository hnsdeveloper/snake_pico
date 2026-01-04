#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "inc/game_start_state.h"
#include "inc/game_play_state.h"
#include "inc/io.h"

#define PLAYABLE_SIDE 16

typedef struct GameStartData {
    bool button_state[4];
    State* next_state;
} GameStartData;

void update_game_start(State* state, uint64_t delta) {
    (void)(delta);
    if(state == NULL)
        return;
    
    Event* events = read_events();
    GameStartData* data = (GameStartData*)(state->extra_data);

    Event* e = events;
    while(e != NULL) {
        if(e->type == BUTTON_PRESS)
            data->button_state[e->event_data.button] = true;
        if(e->type == BUTTON_RELEASE)
            data->button_state[e->event_data.button] = false;
        e = e->next_event;
    }

    size_t pressed_count = 0;
    for(size_t i = 0; i < sizeof(data->button_state)/data->button_state[0]; ++i) {
        if(data->button_state[i] == true)
            ++pressed_count;
    }

    if(pressed_count > 1) {
        data->next_state = create_game_play_state(PLAYABLE_SIDE);
    }

    // Destroys all events
    Event* i = events;
    while(i != NULL) {
        Event* prev = i;
        i = prev->next_event;
        destroy_event(i);
    }
}

State* next_state_start(State* state) {
    if(state == NULL)
        return NULL;

    GameStartData* data = (GameStartData*)(state->extra_data);

    // This doesn't sound right, this invariant should never hold.
    if(data == NULL)
        return NULL; // We return null just to point it out then
    
    if(data->next_state) {
        State* s = data->next_state;
        data->next_state = NULL;
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
    display_draw_text_justified_left(0,0, "Press more than 1 button to play, and when playing, more than 1 button to pause.");
}

State* create_game_start_state() {
    State* s = malloc(sizeof(State));
    if(s == NULL)
        return NULL;

    GameStartData* data = malloc(sizeof(GameStartData));
    
    if(data != NULL) {
        memset(data, 0, sizeof(GameStartData));
        s->extra_data = data;
    } else {
        free(s);
        return NULL;
    }
    
    s->update_fn = &update_game_start;
    s->next_state_fn = &next_state_start;
    s->destroy_state_fn = &destroy_state_start;
    s->state_draw_fn = &draw_state_start;
    
    return s;
}