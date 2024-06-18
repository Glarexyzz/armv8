//
// Created by arthurallilaire on 18/06/24.
//

#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H
#include "process_instr.h"
bool parse_operand(char *str_operand, instr_operand *instr, bool sf, context file_context);

//Reg helpers
int get_reg_num(char *reg_str, context file_context);
int get_sf(char **reg_strs, int max_num_regs, context file_context);
bool parse_reg(char *reg_str, bool *sf, uint8_t *reg_num, context file_context);
bool parse_regs(char **reg_strs, int num_regs, bool *sf, uint8_t *regs, context file_context);
bool imm_to_int(char *imm_str, uint16_t *imm_res, int max, context file_context);
bool get_shift(const char *shift, shift_enum *shift_type, context file_context);
#endif //PARSE_UTILS_H
