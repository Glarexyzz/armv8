#ifndef SNAKE
#define SNAKE

#include "utils.h"

//Forward declaration because it is nested
typedef struct snake_segment snake_segment;

struct snake_segment {
    vector pos;
    snake_segment * next;
} ;

typedef struct snake {
    snake_segment * head; 
    direction direction;
    int length;
} snake;

void free_snake(snake * snake);

void grow_snake_from_head(snake * snake, snake_segment * new_segment);
void remove_tail(snake * snake);

snake_segment * new_snake_segment(vector * pos);
snake * new_snake (vector * head_pos);

#endif /* SNAKE */