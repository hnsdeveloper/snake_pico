#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "inc/cardinal.h"
#include <stdbool.h>

typedef struct Snake Snake;
typedef Point Apple;

/// @brief Will spawn a snake at a random location, respecting the x and y limits (non inclusive).
/// @param width The width. Shouldn't exceed INT32_MAX. Also, width * height shouldn't exceed INT32_MAX
/// @param height The height. Shouldn't exceed INT32_MAX.
/// @return A pointer to a Snake instance on success, NULL on failure.
Snake* spawn_snake(uint32_t width, uint32_t height);

/// @brief Spawns an Apple, respecting the x and y limits (non inclusive), while also taking care to not overlap it with 
/// a Snake instance.
/// @param snake A pointer to a Snake instance.
/// @param width The width. Shouldn't exceed INT32_MAX. Also, width * height shouldn't exceed INT32_MAX
/// @param height The height. Shouldn't exceed INT32_MAX.
/// @return An instance to an Apple on success or NULL on failure.
Apple* spawn_apple(Snake* snake, uint32_t width, uint32_t height);

/// @brief Moves a Snake to a direction.
/// @param snake The snake instance.
/// @param direction The desired direction.
void move_snake(Snake* snake, Direction direction);

/// @brief Checks if a snake has collided with walls or with itself.
/// @param snake The snake instance.
/// @param width The width. Shouldn't exceed INT32_MAX. Also, width * height shouldn't exceed INT32_MAX
/// @param height The height. Shouldn't exceed INT32_MAX.
/// @return 1 if it has collided, 0 if not and -1 on error (invalid arguments or corrupted data).
int has_snake_collided(Snake* snake, uint32_t width, uint32_t height);

/// @brief Checks if the head of the snake overlaps with the apple's position.
/// @param snake The Snake instance.
/// @param apple The Apple instance.
/// @return 1 if it has collided, 0 if not and -1 on error (invalid arguments or corrupted data).
int has_snake_overlapped_apple(Snake* snake, Apple* apple);

/// @brief Grows the snake by one unit.
/// @param snake The snake instance.
/// @return True if it successfully created a new part for the snake, otherwise false.
bool grow_snake(Snake* snake);

/// @brief Destroys a Snake instance.
/// @param snake The snake instance.
void destroy_snake(Snake* snake);

/// @brief Destroy an Apple instance.
/// @param apple The Apple instance.
void destroy_apple(Apple* apple);

#endif