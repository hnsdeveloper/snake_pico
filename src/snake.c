#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/io.h"
#include "inc/snake.h"
#include "inc/doublell.h"
#include "pico/rand.h"

struct Snake {
    DoubleLinkedList* parts_list;
    Point tail_last_position;
    Direction last_direction;
};

bool is_valid_width_and_height(uint32_t width, uint32_t height) {
    if(width <= 1 || height <= 1 || width > INT32_MAX || height > INT32_MAX)
        return false;
    
    uint32_t r = width * height;

    if(r / width != height)
        return false;
    
    return r <= INT32_MAX;
}

Snake* spawn_snake(uint32_t width, uint32_t height) {
    if(!is_valid_width_and_height(width,height))
        return NULL;
    
    uint32_t x = get_rand_32() % width;
    uint32_t y = get_rand_32() % height;

    Snake* instance = malloc(sizeof(Snake));
    if(instance == NULL)
        return NULL;

    instance->last_direction = INVALID_DIRECTION;
    DoubleLinkedList* snake_linked_list = create_linked_list();
    if(snake_linked_list == NULL) {
        free(instance);
        return NULL;
    }

    Point p;
    p.x = (int32_t)x;
    p.y = (int32_t)y;

    prepend_data(snake_linked_list, &p, sizeof(Point));

    instance->parts_list = snake_linked_list;

    return instance;
}

void check_apple_overlap_function(const void* position,void* acc, void* extra_data) {
    if(position == NULL || acc == NULL || extra_data == NULL)
        return;
    
    const Point* current_part_position = (const Point*)(position);
    bool* result = (bool*)(acc);
    Apple* apple = (Apple*)(extra_data);

    if(apple->x == current_part_position->x && apple->y == current_part_position->y)
        *result = true;

}

Apple spawn_apple(Snake* snake, uint32_t width, uint32_t height) {
    Apple a;

    a.x = -1;
    a.y = -1;
    
    if(snake != NULL && width > 1 && height > 1 && snake_parts_count(snake) < width * height) { 
        bool overlaps;
        do {
            overlaps = false;
            a.x = get_rand_32() % width;
            a.y = get_rand_32() % height;
            reduce_from_tail(snake->parts_list, check_apple_overlap_function, &overlaps, &a);
        } while(overlaps);
    }
    return a;
}

int has_snake_overlapped_apple(Snake* snake, Apple* apple) {
    if(snake == NULL || apple == NULL)
        return -1;
    
    
    Point* snake_head = (Point*)(get_tail(snake->parts_list));

    if(snake_head->x == apple->x && snake_head->y == apple->y)
        return 1;

    return 0;
}

typedef struct ExtraData {
    Point point;
    Direction direction; // Only used when updating the snake position
    bool is_first;
} ExtraData;

void update_position_function(void* point, void* extra_data) {
    if(point == NULL || extra_data == NULL)
        return;
    
    Point* current_position = (Point*)(point);
    ExtraData* data = (ExtraData*)(extra_data);

    Point old_position = *current_position;

    if(data->is_first) {
        if(data->direction == UP)
            current_position->y -= 1;
        else if(data->direction == RIGHT)
            current_position->x += 1;
        else if(data->direction == DOWN)
            current_position->y += 1;
        else if(data->direction == LEFT)
            current_position->x -=1;
        data->is_first = false;
    } else {
        *current_position = data->point;
    }

    data->point = old_position;
}

void move_snake(Snake* snake, Direction direction) {
    if(snake == NULL)
        return;

    if(direction == INVALID_DIRECTION && snake->last_direction == INVALID_DIRECTION)
        return;

    ExtraData update_data;
    update_data.is_first = true;

    if(snake_parts_count(snake) == 1) {
        update_data.direction = direction;
        snake->last_direction = direction;
    } else if(direction == UP && snake->last_direction == DOWN) {
        update_data.direction = DOWN;
    } else if (direction == RIGHT && snake->last_direction == LEFT) {
        update_data.direction = LEFT;
    } else if (direction == DOWN && snake->last_direction == UP) {
        update_data.direction = UP;
    } else if (direction == LEFT && snake->last_direction == RIGHT) {
        update_data.direction = RIGHT;
    } else {
        update_data.direction = direction;
        snake->last_direction = direction;
    }

    snake->tail_last_position = *(Point*)(get_head(snake->parts_list));
    in_place_map_from_tail(snake->parts_list, &update_position_function, &update_data);
}

void check_collision_function(const void* position,void* acc, void* extra_data) {
    if(position == NULL || acc == NULL || extra_data == NULL)
        return;
    
    const Point* current_part_position = (const Point*)(position);
    bool* result = (bool*)(acc);
    ExtraData* data = (ExtraData*)(extra_data);

    if(data->is_first) {
        data->is_first = false;
        return;
    }

    if(current_part_position->x == data->point.x && current_part_position->y == data->point.y)
        *result = true;
}

int has_snake_collided(Snake* snake, uint32_t width, uint32_t height) {
    if(snake == NULL)
        return -1;
    if(!is_valid_width_and_height(width,height))
        return -1;

    Point* snake_head_position = (Point*)(get_tail(snake->parts_list));

    if(snake_head_position == NULL)
        return -1;

    bool result = 0;
    result |= snake_head_position->x >= width;
    result |= snake_head_position->y >= height;

    ExtraData extra_data;
    extra_data.is_first = true;
    extra_data.point = *snake_head_position;

    reduce_from_tail(snake->parts_list, &check_collision_function, &result, &extra_data);
    
    return result;
}

bool grow_snake(Snake* snake) {
    if(snake == NULL)
        return false;
    return prepend_data(snake->parts_list, &(snake->tail_last_position), sizeof(Point));
}

void destroy_snake(Snake* snake) {
    if(snake != NULL) {
        destroy_linked_list(snake->parts_list);
        free(snake);
    }
}

void counter_function(const void* data, void* accumulator, void* extra_data) {
    uint32_t* acc = (uint32_t*)(accumulator);
    *acc += 1;
}

uint32_t snake_parts_count(Snake* snake) {
    uint32_t acc = 0;
    if(snake != NULL) {
        reduce_from_tail(snake->parts_list, &counter_function, &acc, NULL);
    }
    return acc;
}


void draw_fn(const void* data, void* extra_data) {
    const Point* snake_part = (const Point*)(data);
    Point* dimensions = (Point*)(extra_data);
    display_draw_block(snake_part->x * dimensions->x, snake_part->y * dimensions->y, dimensions->x, dimensions->y);
}

void draw_game(Snake* snake, Apple* apple, uint32_t side_size) {
    // It will only support drawing integer ratios, as anything else is out of scope.

    // It is fine if apple is NULL, as it simply means it is on the blinking part of the cycle.
    if(snake == NULL)
        return;
    Point p;

    if(display_width() > display_height()) {
        p.x = display_height()/side_size * display_width()/display_height();
        p.y = display_height()/side_size;
    } else {
        p.x = display_width()/side_size;
        p.y = display_width()/side_size * display_height()/display_width();
    }

    for_each(snake->parts_list, &draw_fn, &p);

    if(apple != NULL) {
        display_draw_checkerboard_block(apple->x * p.x, apple->y * p.y, p.x, p.y);
    }
}