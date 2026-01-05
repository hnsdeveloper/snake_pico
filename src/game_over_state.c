#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "inc/game_over_state.h"
#include "inc/game_start_state.h"
#include "inc/io.h"

#define GAME_OVER_SHOW_TIME_US 3000000

typedef void (*state_draw_fn)(State*);

typedef struct ExtraData {
    uint64_t time_accumulator;
    char* text;
} ExtraData;

void game_over_draw(State* state) {
    if(state == NULL)
        return;
    ExtraData* data = (ExtraData*)(state->extra_data);
    if(data == NULL) {
        return;
    }

    display_draw_text_justified_left(0, 0, data->text);
}

void game_over_destroy(State* state) {
    if(state == NULL)
        return;
    ExtraData* data = (ExtraData*)(state->extra_data);
    if(data != NULL) {
        if(data->text != NULL)
            free(data->text);
        free(data);
    }
    free(state);
}

void game_over_update(State* state, uint64_t delta) {
    if(state == NULL)
        return;
    
    ExtraData* data = (ExtraData*)(state->extra_data);
    
    if(data == NULL)
        return;
    
    data->time_accumulator += delta;
}

State* game_over_next_state(State* state) {
    if(state == NULL)
        return NULL;
    ExtraData* data = (ExtraData*)(state->extra_data);
    if(data == NULL)
        return NULL;
    
    if(data->time_accumulator >= GAME_OVER_SHOW_TIME_US) {
        return create_game_start_state();
    }

    return state;
}

State* create_game_over_state(uint32_t score) {
    State* state = malloc(sizeof(State));
    if(state == NULL)
        return NULL;

    ExtraData* data = malloc(sizeof(ExtraData));
    if(data == NULL) {
        free(state);
        return NULL;
    }
    
    int string_size = snprintf(NULL, 0, "Score: " PRIu32 "\n", score);
    char* str = malloc(string_size + 1);
    
    if(str == NULL) {
        free(state);
        free(data);
        return NULL;
    }

    snprintf(str, string_size + 1, "Score: " PRIu32 "\n", score);
    data->text = str;
    data->time_accumulator = 0;
    state->extra_data = data;

    state->update_fn = &game_over_update;
    state->next_state_fn = &game_over_next_state;
    state->destroy_state_fn = &game_over_destroy;
    state->state_draw_fn = &game_over_draw;

    return state;
}