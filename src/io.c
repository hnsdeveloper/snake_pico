#include "inc/io.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include "ssd1306.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define UP_BUTTON_PIN (uint)(12)
#define RIGHT_BUTTON_PIN (uint)(13)
#define DOWN_BUTTON_PIN (uint)(14)
#define LEFT_BUTTON_PIN (uint)(15)

#define I2C_PIN_1 (uint)(16)
#define I2C_PIN_2 (uint)(17)

ssd1306_t* display_instance;
Event* volatile events;

/// @brief Callback for button presses.
/// @param gpio The gpio pin that triggered the callback.
/// @param events The event of the callback.
void button_callback(uint gpio, uint32_t event) {
    Event* e = malloc(sizeof(Event));
    e->next_event = NULL;
    
    if(event & GPIO_IRQ_EDGE_RISE) {
        printf("Rise edge detected.\n");
        e->type = BUTTON_PRESS;
    } else if (event & GPIO_IRQ_EDGE_FALL) { // GPIO_IRQ_EDGE_FALL
        printf("Rise edge fall.\n");
        e->type = BUTTON_RELEASE;
    }  
        

    if(gpio == UP_BUTTON_PIN) 
        e->event_data.button = BUTTON_UP;
    if(gpio == RIGHT_BUTTON_PIN)
        e->event_data.button = BUTTON_RIGHT;
    if(gpio == DOWN_BUTTON_PIN)
        e->event_data.button = BUTTON_DOWN;
    if(gpio == LEFT_BUTTON_PIN)
        e->event_data.button = BUTTON_LEFT;

    if(events == NULL) {
        events = e;
    } else {
        Event* i = events;
        while(i->next_event != NULL)
            i = i->next_event;
        i->next_event = e;
    }
}

Event* read_events() {
    uint32_t status = save_and_disable_interrupts();
    Event* e = events;
    events = NULL;
    restore_interrupts(status);
    return e;
}

/// @brief Destroys an event, freeing any used memory. DOES NOT DESTROY THE EVENTS POINTED BY next_event. Each event
/// should be destroyed individually.
/// @param event The event to be destroyed.
void destroy_event(Event* event) {
    if(event != NULL) {
        free(event);
    }
}

ssd1306_t* init_display(uint32_t width, uint32_t height) {
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_PIN_1, GPIO_FUNC_I2C);
    gpio_set_function(I2C_PIN_2, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_PIN_1);
    gpio_pull_up(I2C_PIN_2);

    ssd1306_t* disp = malloc(sizeof(ssd1306_t));

    if(disp != NULL) {
        memset(disp, 0, sizeof(ssd1306_t));
        if(ssd1306_init(disp, width, height, 0x3c, i2c0) != true) {
            free(disp);
            disp = NULL;
        } else {
            ssd1306_clear(disp);
        }
        return disp;
    }

    return NULL;
}

int init_io(uint32_t width, uint32_t height, uint32_t pixels_block_per_shortest_side) {
    if(pixels_block_per_shortest_side & (pixels_block_per_shortest_side - 1))
        return 1;
    
    printf("Initializing button pins.\n");

    events = NULL;

    uint arr[] = {UP_BUTTON_PIN, RIGHT_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN};

    for(size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
        gpio_init(arr[i]);
        gpio_set_dir(arr[i], GPIO_IN);
        gpio_pull_down(arr[i]);
        gpio_set_irq_enabled_with_callback(arr[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);
    }

    printf("Initializing display.\n");
    
    ssd1306_t* disp = init_display(width, height);

    if(disp == NULL)
        return 1;

    display_instance = disp;

    return 0;
}

uint32_t display_width() {
    return display_instance->width;
}

uint32_t display_height() {
    return display_instance->height;
}

void display_draw_block(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    ssd1306_draw_square(display_instance, x, y, width, height);
}
 
void display_draw_checkerboard_block(uint32_t x, uint32_t y, uint32_t x_size, uint32_t y_size) {
    for(uint32_t i = 0; i <  y_size; ++i) {
        bool draw = !(i & 1);
        for(size_t j = 0; j < x_size; ++j) {
            if(draw)
                ssd1306_draw_pixel(display_instance, x + j, y + i);
            draw = !draw;
        }
    }
}

const char* fill_buffer_with_string_no_word_truncate(const char* src, char* buffer, size_t buffer_size) {
    if(src == NULL || buffer == NULL || *src == 0)
        return NULL;

    //Skip all whitespaces at the beginning.
    while((isspace(*src) || *src == '\n') && *src != 0)
        ++src;
    
    // If we got a string only with whitespaces, it is an invalid string, and thus an error.
    if(*src == 0)
        return NULL;

    size_t i = 0;
    // The last byte is reserved for the null character
    while(i < buffer_size - 1 && src[i]) {
        buffer[i] = src[i];
        ++i;
    }

    // Now we need to check if we truncated a word.
    // First we test if we achieved the end of the src.
    // If we did so, no need to do anything, just zero the remaning of the buffer.
    if(src[i] != 0) {
        // Okay, now we check if we are in the beginning/middle of a word.
        // If we are, we need to backtrack.

        // If the next character is the null character or a space character, we are good as we are not
        // truncating anything. So lets check for it.
        if(!(src[i + 1] == 0 || isspace(src[i + 1]) || src[i + 1] == '\n')) {
            // It could be that we are in the middle of a lot of spaces. If that is the case, we want
            // to ignore it.

            if(isspace(src[i]) || src[i] == '\n') {
                // i cannot reach zero as we already checked for a string with zero size
                // or only with space characters.
                while((isspace(src[i]) || src[i] == '\n')) {
                    --i;
                }
            } else { // Okay, we are in the middle of a word
                while(!(isspace(src[i]) || src[i] == '\n')) {
                    // If there is a word that is too big to fit in the buffer, we zero the buffer 
                    // and return NULL.
                    if(i == 0) {
                        for(size_t j = 0; j < buffer_size; ++j) {
                            buffer[j] = 0;
                        }
                        return NULL;
                    }
                    --i;
                }
                // We want to start at least from the space character on the next call with the 
                // return value.
                i += 1;
            }
        }
    }

    // Set the remaining characters to 0
    for(size_t j = 0; j + i < buffer_size; ++j) {
        buffer[j + i] = 0;
    }
    
    return src + i;
}

void display_draw_text_justified_left(uint32_t x, uint32_t y, const char* string) {
    if(string == NULL)
        return;
    
    // Ok, that is not ideal, but the developer of the library did it in such a manner by defining it on a header
    // that I get multiple definitions if I include fonts.h. For such, I will hardcode it, but ideally I would edit the
    // library and maybe have a function to return a pointer to the font. Heck, ideally it would be a struct with metadata
    // about the font, and not raw bytes.
    uint8_t font_width = 5;
    uint8_t font_height = 8;
    uint8_t font_spacing = 1;

    uint32_t characters_per_line = display_width() / (font_width + font_spacing);
    size_t buffer_size = characters_per_line + 1;
    char* buffer = malloc(buffer_size + 1);
    const char* str = string;
    do {
        str = fill_buffer_with_string_no_word_truncate(str, buffer, buffer_size);
        if(str == NULL) {
            printf("Error while adjusting string to be printed on screen. Check the string.\n");
            break;
        }
        ssd1306_draw_string(display_instance, x, y, 1, buffer);
        y += font_height + 1;
    } while (*str != 0);
    
    free(buffer);
}

void display_show() {
    ssd1306_show(display_instance);
}

void display_clear() {
    ssd1306_clear(display_instance);
}