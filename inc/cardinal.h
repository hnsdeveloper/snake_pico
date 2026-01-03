#ifndef _POINT_H_
#define _POINT_H_

#include <stdint.h>

typedef enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    INVALID_DIRECTION
} Direction;

typedef struct Point {
    int32_t x;
    int32_t y;
} Point;

#endif