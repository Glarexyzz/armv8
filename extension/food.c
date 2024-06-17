#include "food.h"

food * new_food (vector head_pos) {
    food * n_food;
    n_food = malloc(sizeof(food));
    return n_food;
}