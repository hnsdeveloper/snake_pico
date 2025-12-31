#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "inc/io.h"

#define I2C_PIN_1 (uint)(16)
#define I2C_PIN_2 (uint)(17)

#define DISPLAY_WIDTH (uint16_t)(128)
#define DISPLAY_HEIGHT (uint16_t)(64)

ssd1306_t* init_display() {
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_PIN_1, GPIO_FUNC_I2C);
    gpio_set_function(I2C_PIN_2, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_PIN_1);
    gpio_pull_up(I2C_PIN_2);

    ssd1306_t* disp = malloc(sizeof(ssd1306_t));

    if(disp != NULL) {
        memset(disp, 0, sizeof(ssd1306_t));
        if(ssd1306_init(disp, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x3c, i2c0) != true) {
            free(disp);
            disp = NULL;
        } else {
            ssd1306_clear(disp);
        }
        return disp;
    }

    return NULL;
}

int main()
{
    stdio_init_all();
    init_button_io();
    
    ssd1306_t* display = init_display();

    if(display == NULL) {
        printf("Failed to initialize display. Check the connections.\n");
        while(true);
    }

    
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
