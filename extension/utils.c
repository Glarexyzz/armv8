#include "utils.h"

void delay(int milliseconds) {
    int prev_time = clock();

    while (clock() - prev_time < milliseconds);
}

vector * new_vector(int x, int y) {
    vector * new_vector;
    new_vector = malloc(sizeof(vector));
    new_vector->x = x;
    new_vector->x = y;
    return new_vector;
}

void free_vector(vector free_vec) {
    free(free_vec);
}
bool vec_comp(vector v1, vector v2) {
    return v1.x == v2.x && v1.y == v2.y;
}