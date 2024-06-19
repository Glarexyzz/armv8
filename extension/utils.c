#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

void delay(int milliseconds, update_func u_func) {
    clock_t prev_time = clock();
    if (u_func == NULL) {
        while (clock() - prev_time < milliseconds/1000.0 * CLOCKS_PER_SEC);
    } else {
        while (clock() - prev_time < milliseconds/1000.0 * CLOCKS_PER_SEC) {
            u_func();
        }
    }
}

vector * new_vector(int x, int y) {
    vector * new_vector;
    new_vector = malloc(sizeof(vector));
    new_vector->x = x;
    new_vector->y = y;
    return new_vector;
}

void free_vector(vector * free_vec) {
    free(free_vec);
}
bool vec_comp(vector v1, vector v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

int rand_in_range(int min, int max) {
    srand(time(NULL));
    //inclusive
    return rand() % (max - min + 2) + min; 
}