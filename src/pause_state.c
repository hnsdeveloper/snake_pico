#include "inc/pause_state.h"

#include <stdlib.h>
#include <string.h>

#include "inc/io.h"

#define PAUSE_MIN_TIME_US 300000

typedef struct ExtraData {
    State* gameplay_state;
    uint64_t pause_acc;
    bool button_state[4];
    bool should_switch;
} ExtraData;

void pause_state_destroy(State* state) {
    if (state == NULL) {
        return;
    }
    if (state->extra_data != NULL) {
        ExtraData* data = (ExtraData*)(state->extra_data);
        if (data->gameplay_state != NULL) {
            destroy_state(data->gameplay_state);
        }
        free(data);
    }
    free(state);
}

void pause_state_draw(State* state) {
    if (state == NULL) return;
    display_draw_text_justified_left(
        0, 0, "Pause. Press more than 1 button to unpause.");
}

State* pause_state_next_state(State* state) {
    if (state == NULL) {
        return NULL;
    }

    ExtraData* data = (ExtraData*)(state->extra_data);
    if (data->should_switch == true) {
        State* ret = data->gameplay_state;
        data->gameplay_state = NULL;
        return ret;
    }

    return state;
}

void pause_state_update(State* state, uint64_t delta) {
    (void)(delta);
    if (state == NULL) return;

    Event* events = read_events();
    ExtraData* data = (ExtraData*)(state->extra_data);

    data->pause_acc += delta;

    Event* e = events;
    while (e != NULL) {
        if (e->type == BUTTON_PRESS)
            data->button_state[e->event_data.button] = true;
        if (e->type == BUTTON_RELEASE)
            data->button_state[e->event_data.button] = false;
        e = e->next_event;
    }

    size_t pressed_count = 0;
    for (size_t i = 0; i < sizeof(data->button_state) / data->button_state[0];
         ++i) {
        if (data->button_state[i] == true) ++pressed_count;
    }

    if (pressed_count > 1 && data->pause_acc >= PAUSE_MIN_TIME_US) {
        data->should_switch = true;
    }

    Event* i = events;
    while (i != NULL) {
        Event* prev = i;
        i = prev->next_event;
        destroy_event(prev);
    }
}

State* create_pause_state(State* state) {
    if (state == NULL) return NULL;

    State* pause_state = malloc(sizeof(State));
    if (pause_state == NULL) return NULL;
    State* gameplay_state_copy = malloc(sizeof(State));
    if (gameplay_state_copy == NULL) {
        free(pause_state);
        return NULL;
    }

    ExtraData* state_data = malloc(sizeof(ExtraData));
    if (state_data == NULL) {
        free(pause_state);
        free(gameplay_state_copy);
        return NULL;
    }

    memcpy(gameplay_state_copy, state, sizeof(State));
    state_data->gameplay_state = gameplay_state_copy;
    state_data->should_switch = false;
    state_data->pause_acc = 0;

    pause_state->extra_data = state_data;
    pause_state->update_fn = &pause_state_update;
    pause_state->next_state_fn = &pause_state_next_state;
    pause_state->state_draw_fn = &pause_state_draw;
    pause_state->destroy_state_fn = &pause_state_destroy;

    return pause_state;
}