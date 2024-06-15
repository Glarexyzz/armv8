//
// Created by arthurallilaire on 08/06/24.
//
#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#include "utils.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint32_t logical_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}
uint32_t wide_move_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}

//  Using uint8_t as no uint4_t but only bottom 4 bits will be used
typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  uint8_t opc;
  bool M; //1 for Multiply - 0 for arithmetic, bit-logic
  uint8_t op0; // ALWAYS 101
  uint8_t opr;
  uint8_t rm;
  uint8_t operand;
  uint8_t rn;
  uint8_t rd;
} dp_reg_instr;


//Returns true if no errors parsed - false for errors
//bool parse_regs(char **reg_strs, int num_regs, dp_reg_instr *dp_instr, context file_context){
//  int res = get_sf(reg_strs, num_regs, file_context);
//  if (res < 0) return false;
//  dp_instr->sf = (bool) res;
//  for (int i = 0; i < num_regs; i++){
//    res = get_reg_num(reg_strs[i], file_context);
//    if (res < 0) return false;
//    switch (i) {
//      case 0: dp_instr->rm = (uint8_t) res; break;
//      case 1: dp_instr->operand |= (uint8_t) res; break;
//      case 2: dp_instr->rn = (uint8_t) res; break;
//      case 3: dp_instr->rd = (uint8_t) res; break;
//      default: return false; // Unexpected number of registers
//    }
//  }
//  return true;
//}

//  Assumes well formatted dp reg
uint32_t dp_reg_to_binary(dp_reg_instr instr){
  return (instr.sf << 31) |
    (instr.opc << 29) |
    (instr.M << 28) |
    (instr.op0 << 25) |
    (instr.opr << 21) |
    (instr.rm << 16) |
    (instr.operand << 10) |
    (instr.rn << 5) |
    (instr.rd << 0);
}

// Applies to madd, msub, mul, mneg
uint32_t multiply_instr(char *opc, char * rest_instr, context file_context){
  // Initialise registers
  int NUMREGS = 4;
  dp_reg_instr instr;
  if (strcmp(opc, "mul") == 0 || strcmp(opc, "mneg") == 0) {instr.rd = 31; NUMREGS--;}
  //x is top bit or operand (ls 6) - 0 for m-add, 1 for m-sub - (do you negate the multiplication)
  if (strcmp(opc, "msub") == 0 || strcmp(opc, "mneg") == 0) instr.operand |= (1 << 6);
  char *reg_strs[NUMREGS];
//  +1 for the null byte
  bool no_errors = split_string_error_checking(rest_instr, reg_strs, MAXREGSTRLEN + 1, NUMREGS, file_context);
  if (!no_errors) return 0; // reg_strs freed by error_checking func

//  Check nothing after registers - TODO - test (what happens with whitespace?)
  if (rest_instr[0] == '\0'){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "Too many registers defined (Expected: %d). Make sure there are no trailing characters", NUMREGS);
    error(error_message, file_context);
    for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
    return 0; //No point continuing
  }

  uint8_t reg_pointers[NUMREGS];

  no_errors = parse_regs(reg_strs, NUMREGS, &instr.sf, reg_pointers, file_context);

  for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
  if (!no_errors) return 0;

  for (int i = 0; i < NUMREGS; i++){
      switch (i) {
        case 0: instr.rm = reg_pointers[i]; break;
        case 1: instr.operand |= reg_pointers[i]; break;
        case 2: instr.rn = reg_pointers[i]; break;
        case 3: instr.rd = reg_pointers[i]; break;
        default: error("Unexpected number of registers", file_context); return 0; // Unexpected number of registers
      }
    }
//  dp_instr finally correctly formatted and valid
  instr.M = 1; instr.op0 = 5;
//  Return binary representation as uint32_t
  return dp_reg_to_binary(instr);
}


uint32_t branch_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}
uint32_t sdt_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}
uint32_t directive_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}
