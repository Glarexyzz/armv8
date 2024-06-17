//
// Created by arthurallilaire on 13/06/24.
//

#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include <stdint.h>
// *rest_instr == NULL if nothing left
// Ignores whitespace before and after string
int split_string(const char *s, char **token_array, int max_size, int max_tokens,char **rest_instr);

// Return true for no errors, false otherwise
// Can check: no_oversized, right_num, nothing_after, prints errors to error using file_context
bool split_string_error_checking(const char *s, char **token_array, int max_size, int num_tokens, char **rest_instr, context file_context, bool no_oversized, bool right_num, bool nothing_after);
//Reg helpers
int get_reg_num(char *reg_str, context file_context);
int get_sf(char **reg_strs, int max_num_regs, context file_context);
bool parse_reg(char *reg_str, bool *sf, uint8_t *reg_num, context file_context);
bool parse_regs(char **reg_strs, int num_regs, bool *sf, uint8_t *regs, context file_context);
bool isfunc_str(const char *str, int (*isfunc)(int) );
char* trim_whitespace(const char *str);
#endif //UTILS_H
