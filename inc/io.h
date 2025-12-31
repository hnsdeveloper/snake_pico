#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>
#include <stdbool.h>
#include "pico/types.h"

#define FRAMETIME_MS (uint64_t)(16)

typedef enum {
    BUTTON_UP,
    BUTTON_RIGHT,
    BUTTON_DOWN,
    BUTTON_LEFT,
    INVALID
} Button;

/// @brief Represent the states of buttons on the current frame.
typedef struct ButtonState ButtonState;

/// @brief Gets the structure with the state of the current buttons.
/// @return A pointer to a ButtonState structure, which should be destroyed using the provided function. Null 
//if no button presses have happened.
ButtonState* get_current_frame_button_states();

/// @brief Destroys a button state, freeing the allocated memory.
/// @param buttonstate The buttonstate to be destroyed.
void destroy_button_state(ButtonState* buttonstate);

/// @brief Checks if a button is pressed or not.
/// @param state The state to check.
/// @param b The button which we want to know if it is pressed or not.
/// @return True if it is pressed, false otherwise.
bool is_button_pressed(ButtonState* state, Button b);

/// @brief Initializes the system responsible for button io
void init_button_io();

#endif