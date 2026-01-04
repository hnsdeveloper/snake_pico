#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "inc/snake.h"
#include "inc/game_play_state.h"
#include "inc/io.h"

// I could make it such that there is a difficulty selector... maybe I do it if I feel like.
#define MIN_MOVE_ACC 500000

typedef struct GamePlayStateData {
    uint64_t acc_delta_time; // Accumulator for time delta.
    uint64_t min_acc_delta_time; // The minimum the accumulator should be to process the game such as moving the snake. Changes over time.
    Apple apple;
    State* pause_state;
    Snake* snake;
    uint32_t side_size;
    Button last_button;
    bool button_state[4]; // For checking if it should pause

} GamePlayStateData;


void update_game_play(State* state, uint64_t delta) {
    if(state == NULL || state->extra_data == NULL)
        return;
        
    GamePlayStateData* data = (GamePlayStateData*)(state->extra_data);

    Event* events = read_events();

    // Do nothing until the player provides the first input. I could randomize it as well, but it could cause such
    // that the player dies instantly. Another alternative would be to never spawn less than n blocks to wall...
    // I prefer it like this to be honest, so I will leave as it is.
    if(data->last_button == INVALID && events == NULL)
        return;
    
    data->acc_delta_time += delta;

    if(events != NULL) {
        Event* i = events;
        while(i != NULL) {
            // For the user input to the game, we care only about the last one, so we can overwrite it
            if(i->type == BUTTON_PRESS) {
                data->last_button = i->event_data.button;
                data->button_state[i->event_data.button] = true;
            } else if (i->type == BUTTON_RELEASE) {
                data->button_state[i->event_data.button] = false;
            }
            i = i->next_event;
        }
    }

    if(data->last_button == INVALID)
        return;
    
    bool all_pressed = true;
    for(size_t i = 0; i < sizeof(data->button_state)/sizeof(data->button_state[0]); ++i) {
        all_pressed = all_pressed && data->button_state[i];
    }

    if(all_pressed) {
        // Switch to pause state...
    }

    if(data->acc_delta_time >= data->min_acc_delta_time) {
        data->acc_delta_time = 0;
        switch (data->last_button) {
            case BUTTON_UP:
                move_snake(data->snake, UP);
                break;
            case BUTTON_RIGHT:
                move_snake(data->snake, RIGHT);
                break;
            case BUTTON_DOWN:
                move_snake(data->snake, DOWN);
                break;
            case BUTTON_LEFT:
                move_snake(data->snake, LEFT);
                break;
            default: 
                break;
        }
    }

    if(has_snake_collided(data->snake, data->side_size, data->side_size)) {
        // TODO: Game over.
    } else if(has_snake_overlapped_apple(data->snake, &(data->apple)) > 0) {
        grow_snake(data->snake);
        if(snake_parts_count(data->snake) == data->side_size * data->side_size) {
            // TODO: Game over.
        } else {
            data->apple = spawn_apple(data->snake, data->side_size, data->side_size);
        }
    }

    // Destroys all events
    Event* i = events;
    while(i != NULL) {
        Event* prev = i;
        i = prev->next_event;
        destroy_event(prev);
    }
}

State* next_state_game_play(State* state) {
    if(state == NULL || state->extra_data == NULL)
        return NULL;

    GamePlayStateData* data = (GamePlayStateData*)(state->extra_data);

    if(data->pause_state != NULL) {
        State* s = data->pause_state;
        data->pause_state = NULL;
        return s;
    }

    return state;
}

void destroy_state_game_play(State* state) {
    if(state != NULL) {
        if(state->extra_data != NULL) {
            GamePlayStateData* data = (GamePlayStateData*)(state->extra_data);
            if(data->snake != NULL)
                destroy_snake(data->snake);
            if(data->pause_state != NULL)
                destroy_state(data->pause_state);
            free(data);
        }
        free(state);
    }
}

void draw_game_play(State* state) {
    if(state != NULL && state->extra_data != NULL) {
        GamePlayStateData* data = (GamePlayStateData*)(state->extra_data);
        draw_game(data->snake, &(data->apple), data->side_size);
    }
}

State* create_game_play_state(uint32_t gameplay_area_side) {
    if(gameplay_area_side <= 1 || gameplay_area_side > INT32_MAX)
        return NULL;
    
    State* s = malloc(sizeof(State));

    if(s == NULL)
        return NULL;

    GamePlayStateData* data = malloc(sizeof(GamePlayStateData));
    if(data == NULL) {
        free(s);
        return NULL;
    }    

    memset(data, 0, sizeof(GamePlayStateData));
    data->pause_state = NULL;
    data->last_button = INVALID;
    data->side_size = gameplay_area_side;
    data->min_acc_delta_time = MIN_MOVE_ACC;
    Snake* snake = spawn_snake(data->side_size, data->side_size);
    
    if(snake == NULL) {
        free(data);
        free(s);
        return NULL;
    }
    
    data->snake = snake;
    data->apple = spawn_apple(snake, data->side_size, data->side_size);
    
    if(data->apple.x == -1) {
        free(snake);
        free(data);
        free(s);
        return NULL;
    }

    s->extra_data = data; 
    s->update_fn = &update_game_play;
    s->next_state_fn = &next_state_game_play;
    s->state_draw_fn = &draw_game_play;
    s->destroy_state_fn = &destroy_state_game_play;

    return s;
}