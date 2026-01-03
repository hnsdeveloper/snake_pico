#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
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

typedef enum {
    BUTTON_PRESS,
    BUTTON_RELEASE,
} EventType;

typedef union {
    Button button;
} EventData;

typedef struct Event {
    struct Event* next_event;
    EventType type;
    EventData event_data;
} Event;

/// @brief Reads all events that happend since the last call.
/// @return Null if there are no events, or a pointer to the first event.
Event* read_events();

/// @brief Destroys an event, freeing any used memory. DOES NOT DESTROY THE EVENTS POINTED BY next_event. Each event
/// should be destroyed individually.
/// @param event The event to be destroyed.
void destroy_event(Event* event);

/// @brief Initializes the system responsible for button io
/// @param pixels_block_per_shortest_side How many pixels the smallest side of the screen will have when drawing a rectangle. 
/// The largest side will be adjusted such that the canvas will have a 1:1 aspect ratio, even if it looks stretched on screen.
/// Must be a power of 2 and be smaller than the display resolution.
/// @return 0 on success, non 0 on failure.
int init_io(uint32_t width, uint32_t height, uint32_t pixels_block_per_shortest_side);

/// @return The display width in pixels.
uint32_t display_width();

/// @return The display height in pixels.
uint32_t display_height();

/// @brief Draws a solid rectangle with the dimensions given on position x and y (x moving to the right and y downwards).
/// @param x The x position.
/// @param y The y position.
/// @param width The width of the rectangle
/// @param height The height of the rectangle
void display_draw_block(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/// @brief Draws a checkerboard pattern rectangle with the dimensions given on position x and y (x moving to the right and
/// y downwards).
/// @param x The x position.
/// @param y The y position.
/// @param width The width of the rectangle
/// @param height The height of the rectangle
void display_draw_checkerboard_block(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

/// @brief 
/// @param x 
/// @param y 
void display_draw_text_justified_left(uint32_t x, uint32_t y, const char* string);

/// @brief Clears the display.
void display_clear();

/// @brief Display what has been drawn so far i.e the framebuffer.
void display_show();



#endif