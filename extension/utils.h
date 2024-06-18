#ifndef UTILS
#define UTILS

#include <stdbool.h>

typedef struct vector {
    int x;
    int y;
} vector;

typedef enum direction {up, right, down, left} direction;

typedef void (*update_func)();

//Optional update_func which will run every step while delaying 
void delay(int milliseconds, update_func u_func);

vector * new_vector(int x, int y);

void free_vector(vector * free_vec);

bool vec_comp(vector v1, vector v2);

int rand_in_range(int min, int max);

#endif /* UTILS */