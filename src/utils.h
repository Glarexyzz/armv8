//
// Created by arthurallilaire on 13/06/24.
//

#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include <stdint.h>

int split_string(char *s, char **token_array, int max_size, int max_tokens);

// Return true for no errors, false otherwise
// Checks right number and no oversized, prints errors to error using file_context
bool split_string_error_checking(char *s, char **token_array, int max_size, int num_tokens, context file_context);
//Reg helpers
int get_reg_num(char *reg_str, context file_context);
int get_sf(char **reg_strs, int max_num_regs, context file_context);
bool parse_regs(char **reg_strs, int num_regs, bool *sf, uint8_t *regs, context file_context);

#endif //UTILS_H
