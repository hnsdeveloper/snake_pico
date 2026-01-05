#include <stdio.h>

#include "inc/game_play_state.h"
#include "inc/game_start_state.h"
#include "inc/io.h"
#include "inc/state.h"
#include "pico/stdlib.h"

void error_message_loop(const char* message) {
    while (true) {
        printf("%s\n", message);
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();

    if (init_io(DISPLAY_WIDTH, DISPLAY_HEIGHT, 4) != 0) {
        error_message_loop(
            "Failed to initialize IO. Check buttons and display cabling.");
    }

    State* s = create_game_start_state();

    if (s == NULL) {
        error_message_loop("Failed to create game start state.");
    }

    uint64_t last = time_us_64();
    while (true) {
        uint64_t now = time_us_64();
        update_state(s, now - last);
        display_clear();
        draw_state(s);
        display_show();
        s = next_state(s);
        last = now;
    }

    return 0;
}
