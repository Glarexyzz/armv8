#include "food.h"
#include <stdlib.h>

food * new_food (vector pos, int score) {
    food * n_food;
    n_food = malloc(sizeof(food));
    n_food->pos = pos;
    n_food->score = score;
    return n_food;
}