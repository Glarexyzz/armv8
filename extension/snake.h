#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct snake_segment {
    vector pos;
    snake_segment * next;
} snake_segment;

typedef struct snake {
    snake_segment * head; 
    direction direction;
    int length;
} snake;

void free_snake(snake * snake);

void grow_snake_from_head(snake * snake, snake_segment * new_segment);
void remove_tail(snake * snake);

snake_segment * new_snake_segment(vector pos);

