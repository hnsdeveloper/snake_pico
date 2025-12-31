#include "inc/io.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UP_BUTTON_PIN (uint)(12)
#define RIGHT_BUTTON_PIN (uint)(13)
#define DOWN_BUTTON_PIN (uint)(14)
#define LEFT_BUTTON_PIN (uint)(15)

struct ButtonState {
    bool pressed[4];
};

ButtonState* volatile current_frame_state;
ButtonState* volatile next_frame_state;

/// @brief Callback for button presses.
/// @param gpio The gpio pin that triggered the callback.
/// @param events The event of the callback.
void button_callback(uint gpio, uint32_t events) {
    // No problem if we start syncing from the first button press
    static uint64_t last_execution = 0;
    uint64_t now = time_us_64();

    uint64_t diff_us;

    if (now > last_execution) 
        diff_us = now - last_execution;
    else 
        diff_us = UINT64_MAX - last_execution + now;
    
    last_execution = now;

    // 1000 us in a MS
    if(diff_us/1000 >= FRAMETIME_MS) {
        // If the input didn't get consumed, we don't care
        if(current_frame_state != NULL)
            destroy_button_state(current_frame_state);
        current_frame_state = next_frame_state;
        next_frame_state = NULL;
    }

    if(next_frame_state == NULL) {
        next_frame_state = malloc(sizeof(ButtonState));
        memset(next_frame_state, 0, sizeof(ButtonState));
    }

    if(events && GPIO_IRQ_EDGE_RISE) {
        next_frame_state->pressed[BUTTON_UP] = (gpio == UP_BUTTON_PIN) | next_frame_state->pressed[BUTTON_UP];
        next_frame_state->pressed[BUTTON_RIGHT] = (gpio == RIGHT_BUTTON_PIN) | next_frame_state->pressed[BUTTON_RIGHT];
        next_frame_state->pressed[BUTTON_DOWN] = (gpio == DOWN_BUTTON_PIN) | next_frame_state->pressed[BUTTON_DOWN];
        next_frame_state->pressed[BUTTON_LEFT] = (gpio == LEFT_BUTTON_PIN) | next_frame_state->pressed[BUTTON_LEFT];
    }
}

ButtonState* get_current_frame_button_states() {
    // We need to disable interrupts as if a button press happens while here,
    // we could end up using an invalid pointer.
    uint32_t status = save_and_disable_interrupts();
    ButtonState* ret = current_frame_state;
    current_frame_state = NULL;
    restore_interrupts(status);
    return ret;
}

void destroy_button_state(ButtonState* buttonstate) {
    free(buttonstate);
}

bool is_button_pressed(ButtonState* state, Button b) {
    if (b < INVALID && b >= 0)
        return state->pressed[b];
    return false;
}

void init_button_io() {
    printf("Initializing button pins.\n");

    current_frame_state = NULL;
    next_frame_state = NULL;

    uint arr[] = {UP_BUTTON_PIN, RIGHT_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN};

    for(size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        gpio_init(arr[i]);
        gpio_set_dir(arr[i], GPIO_IN);
        gpio_pull_down(arr[i]);
        gpio_set_irq_enabled_with_callback(arr[i], GPIO_IRQ_EDGE_RISE, true, &button_callback);
    }
}
