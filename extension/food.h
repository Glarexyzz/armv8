#ifndef FOOD
#define FOOD

#include "utils.h"
#define FOOD_SCORE 1

typedef struct food {
    vector pos;
    int score;
} food;

food * new_food (vector * pos, int score);

#endif /* FOOD */