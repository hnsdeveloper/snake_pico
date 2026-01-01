#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>
#include <stdbool.h>
#include "pico/types.h"

#define FRAMETIME_US (uint64_t)(16666)

#define DISPLAY_WIDTH (uint16_t)(128)
#define DISPLAY_HEIGHT (uint16_t)(64)

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
/// @param pixels_block_per_shortest_side How many pixels the smallest side of the screen will have when drawing a rectangle. 
/// The largest side will be adjusted such that the canvas will have a 1:1 aspect ratio, even if it looks stretched on screen.
/// Must be a power of 2 and be smaller than the display resolution.
/// @return 0 on success, non 0 on failure.
int init_io(uint32_t width, uint32_t height, uint32_t pixels_block_per_shortest_side);

/// @brief 
/// @return 
uint32_t display_block_unit_width();

/// @brief 
/// @return 
uint32_t display_block_unit_height();

/// @brief 
/// @return 
uint32_t display_width();

/// @brief 
/// @return 
uint32_t display_height();

/// @brief 
/// @param x 
/// @param y 
void display_draw_block(uint32_t x, uint32_t y);

/// @brief 
/// @param x 
/// @param y 
void display_draw_checkerboard_block(uint32_t x, uint32_t y);

/// @brief 
/// @param x 
/// @param y 
void display_draw_text_justified_left(uint32_t x, uint32_t y, const char* string);

/// @brief 
void display_clear();

/// @brief 
void display_show();



#endif