#include <stdbool.h>

typedef struct vector {
    int x;
    int y;
} vector;

typedef enum direction {up, right, down, left} direction;

void delay(int milliseconds);

vector * new_vector(int x, int y);

bool vec_comp(vector v1, vector v2);