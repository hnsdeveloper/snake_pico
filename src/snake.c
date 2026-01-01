#include <string.h>
#include <stdint.h>
#include "inc/snake.h"
#include "inc/doublell.h"
#include "pico/rand.h"
#include <stdlib.h>

struct Snake {
    DoubleLinkedList* parts_list;
};

bool is_valid_width_and_height(uint32_t width, uint32_t height) {
    if(width == 0 || height == 0 || width > INT32_MAX || height > INT32_MAX)
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

    DoubleLinkedList* snake_linked_list = create_linked_list();
    if(snake_linked_list == NULL) {
        free(instance);
        return NULL;
    }

    Point p;
    p.x = (int32_t)x;
    p.y = (int32_t)y;

    prepend_data(instance->parts_list, &p, sizeof(Point));

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

Apple* spawn_apple(Snake* snake, uint32_t width, uint32_t height) {
    if(snake == NULL)
        return NULL;
    if(!is_valid_width_and_height(width,height))
        return NULL;
    
    Apple* apple = malloc(sizeof(Apple));
    
    if(apple == NULL)
        return NULL;

    if(snake_parts_count(snake) >= width * height)
        return NULL;

    bool overlaps;
    do {
        overlaps = false;
        apple->x = get_rand_32() % width;
        apple->y = get_rand_32() % height;
        reduce_from_tail(snake->parts_list, check_apple_overlap_function, &overlaps, apple);
    } while(overlaps);

    return apple;
}

int has_snake_overlapped_apple(Snake* snake, Apple* apple) {
    if(snake == NULL || apple == NULL)
        return -1;
    
    bool overlaps = false;
    reduce_from_tail(snake->parts_list, check_apple_overlap_function, &overlaps, apple);
    return overlaps;    
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
    
    ExtraData update_data;
    update_data.direction = direction;
    update_data.is_first = true;

    in_place_map_from_tail(snake->parts_list, &update_position_function, &update_data);
}

void check_collision_function(const void* position,void* acc, void* extra_data) {
    if(position == NULL || acc == NULL || extra_data == NULL)
        return;
    
    const Point* current_part_position = (const Point*)(position);
    bool* result = (bool*)(acc);
    ExtraData* data = (ExtraData*)(extra_data);

    
    if(data->is_first) {
        // On the first iteration, the point represents the width and height.
        if(data->point.x == current_part_position->x || data->point.y == current_part_position->y)
            *result = true;
        // On remaining iterations, it represents the position of the head.
        data->point = *current_part_position;
        data->is_first = false;
    } else {
        if(current_part_position->x == data->point.x && current_part_position->y == data->point.y)
            *result = true;
    }

}

int has_snake_collided(Snake* snake, uint32_t width, uint32_t height) {
    if(snake == NULL)
        return -1;
    if(!is_valid_width_and_height(width,height))
        return -1;

    Point* head_position = (Point*)(get_head(snake->parts_list));

    if(head_position == NULL)
        return -1;

    bool result = false;
    ExtraData extra_data;
    extra_data.is_first = true;
    extra_data.point = *head_position;

    reduce_from_tail(snake->parts_list, &check_collision_function, &result, &extra_data);
    
    return result;
}

bool grow_snake(Snake* snake) {
    if(snake == NULL)
        return false;
    Point p;
    return prepend_data(snake->parts_list, &p, sizeof(Point));
}

void destroy_snake(Snake* snake) {
    if(snake != NULL) {
        destroy_linked_list(snake->parts_list);
        free(snake);
    }
}

void destroy_apple(Apple* apple) {
    if(apple != NULL)
        free(apple);
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