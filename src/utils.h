//
// Created by arthurallilaire on 13/06/24.
//

#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

int split_string(char *s, char **token_array, int max_size, int max_tokens);

//Reg helpers
int get_reg_num(char *reg_str, context file_context);
int get_sf(char **reg_strs, int max_num_regs, context file_context);

#endif //UTILS_H
