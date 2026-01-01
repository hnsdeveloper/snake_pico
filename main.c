#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/io.h"
#include "inc/state.h"
#include "inc/game_start_state.h"

void error_message_loop(const char* message) {
    while(true) {
        printf("%s\n", message);
        sleep_ms(1000);
    }
    
}

int main()
{
    stdio_init_all();

    if(init_io(DISPLAY_WIDTH, DISPLAY_HEIGHT, 4) != 0) {
        error_message_loop("Failed to initialize IO. Check buttons and display cabling.");
    }

    State* s = create_game_start_state();

    if(s == NULL) {
        error_message_loop("Failed to create game start state.");
    }
    
    while(true) {
        update_state(s);
        display_clear();
        draw_state(s);
        display_show();
        s = next_state(s);
    }

    return 0;
}
