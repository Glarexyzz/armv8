#include <stdlib.h>
#include <stdio.h>
#include "snake.h"

void move(direction dir) {

};

void free_snake (snake * snake) {
    snake_segment * curr_segment = snake->head;
    while (curr_segment->next != NULL)
    {
        snake_segment * prev_segment = curr_segment;
        curr_segment = curr_segment->next;
        free(prev_segment);
    }
    free(snake);
}

snake * new_snake (vector head_pos) {
    snake * n_snake;
    n_snake = malloc(sizeof(snake));
    n_snake->head = new_snake_segment(head_pos);
}

snake_segment * new_snake_segment(vector pos) {
    snake_segment * new_segment;
    new_segment = malloc(sizeof(snake_segment));
    new_segment->pos = pos;
    new_segment->next = NULL;
    return new_segment;
}

void grow_snake_from_head(snake * snake, snake_segment * new_segment);

//Should probably check if there is at least one segment
void remove_tail(snake * snake) {

    snake_segment * last = snake->head;
    snake_segment * prev_last = last;
    while (last->next != NULL) {
        prev_last = last;
        last = last->next;
    }
    free(last);
    prev_last->next = NULL;
}