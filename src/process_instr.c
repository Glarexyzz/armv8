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



//  Using uint8_t as no uint4_t but only bottom 4 bits will be used
typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  uint8_t opc;
  uint8_t op0; // ALWAYS 100
  uint8_t opi;
  uint16_t operand; // 5 - 22 bits (18 bits)
  uint8_t rd;
} dp_imm_instr;

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

typedef struct {
  uint8_t hw; // top 2 bits
  uint16_t imm16;
} wide_operand;

typedef struct {
  bool sh;
  uint16_t imm12;
  uint8_t rn;
} arith_operand;

typedef struct {
  uint8_t rm;
  uint8_t shType;
  uint8_t operand; // 6 bit determining shift amount
} logic_operand_parse_result;

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

bool parse_arith_operand(char *str_operand, arith_operand *operand, context file_context){
  return true;
}

//Should be similar to/same as parse_arith_operand?
bool parse_logic_operand(char *str_operand, logic_operand_parse_result *result, context file_context) {
  // str_operand of form rm or rm{, shift #amount}
  result->rm = get_reg_num(strtok(str_operand, "{, #}"),file_context);
  
  char* shiftType = strtok(NULL, "{, #}");
  if (shiftType == NULL) {
    // operand is just rm, so shift rm by 0
    result->shType == 0;
    result->operand == 0;
  } else {
    if (strcmp(shiftType, "lsl")) {result->shType = 0;} else
    if (strcmp(shiftType, "lsr")) {result->shType = 1;} else
    if (strcmp(shiftType, "asr")) {result->shType = 2;} else
    if (strcmp(shiftType, "ror")) {result->shType = 3;} else
    result->operand = (uint8_t)atoi(strtok(NULL, "{, #}"));
  }
  return true;
}

uint32_t arithmetic_instr(char *opc, char * rest_instr, context file_context){
// Applies to: add(s), sub(s), cmp/n - set flags rd is zr, neg(s) - rn is zr
//  Two operand: <opcode> rd, rn, <operand> Applies to arithmetic and bit-logic operations.
//  Operand can be either rm or #<imm> with optional{, <shift> #<amount> }
//  If add or sub - rd, rn, <op2>, if cmp

  return 0;
}

uint32_t logical_instr(char *opc, char * rest_instr, context file_context){
  int NUMARGS = 3; // rd, rn, opearand
  dp_reg_instr instr;
  logic_operand_parse_result parse_result;
  bool N;
  char** argArray;

  //Set opcode and N
  if (strcmp(opc, "and")) {instr.opc = 0; N = 0;} else
  if (strcmp(opc, "bic")) {instr.opc = 0; N = 1;} else
  if (strcmp(opc, "orr")) {instr.opc = 1; N = 0;} else
  if (strcmp(opc, "orn")) {instr.opc = 1; N = 1;} else
  if (strcmp(opc, "eor")) {instr.opc = 2; N = 0;} else
  if (strcmp(opc, "eon")) {instr.opc = 2; N = 1;} else
  if (strcmp(opc, "ands")) {instr.opc = 3; N = 0;} else
  if (strcmp(opc, "bics")) {instr.opc = 3; N = 1;} else
  {return 0;}
  
  // +1 for the null byte
  int num_oversized = split_string(rest_instr, argArray, MAXREGSTRLEN + 1, NUMARGS);
  if (num_oversized > 0){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "%d oversized register labels (Max length for registers: %d)", num_oversized, MAXREGSTRLEN);
    error(error_message, file_context);
  }

  instr.rd = get_reg_num(argArray[0], file_context);
  instr.rn = get_reg_num(argArray[1], file_context);
  char* operand = argArray[2];

  bool parse_success = parse_logic_operand(operand, &parse_result, file_context);
  instr.opr = parse_result.shType << 1 | N;
  instr.rm = parse_result.rm;
  instr.operand = parse_result.operand;

  instr.M = 0;
  instr.op0 = 6; // ALWAYS 101
  instr.sf = argArray[0][0] == 'x' ? 1 : 0;
  
  return dp_reg_to_binary(instr);
}

uint32_t wide_move_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}

//Returns true if no errors parsed - false for errors
bool parse_regs(char **reg_strs, int num_regs, dp_reg_instr *dp_instr, context file_context){
  int res = get_sf(reg_strs, num_regs, file_context);
  if (res < 0) return false;
  dp_instr->sf = (bool) res;
  for (int i = 0; i < num_regs; i++){
    res = get_reg_num(reg_strs[i], file_context);
    if (res < 0) return false;
      switch (i) {
          case 0: dp_instr->rm = (uint8_t) res; break;
          case 1: dp_instr->operand |= (uint8_t) res; break;
          case 2: dp_instr->rn = (uint8_t) res; break;
          case 3: dp_instr->rd = (uint8_t) res; break;
          default: return false; // Unexpected number of registers
      }
  }
  return true;
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
  int num_oversized = split_string(rest_instr, reg_strs, MAXREGSTRLEN + 1, NUMREGS);
  if (num_oversized > 0){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "%d oversized register labels (Max length for registers: %d)", num_oversized, MAXREGSTRLEN);
    error(error_message, file_context);
//    Free memory
    for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
    return 0; //No point continuing
  }
//  Check right number of registers
  for (int i = 0; i < NUMREGS; i++){
    if (reg_strs[i] == NULL){
        char error_message[MAXERRORLEN];
        snprintf(error_message, MAXERRORLEN, "Not enough registers defined (Expected: %d, Found: %d)", NUMREGS, i);
        error(error_message, file_context);
        for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
        return 0; //No point continuing
    }
  }

//  Check nothing after registers - TODO - test (what happens with whitespace?)
  if (rest_instr[0] == '\0'){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "Too many registers defined (Expected: %d). Make sure there are no trailing characters", NUMREGS);
    error(error_message, file_context);
    for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
    return 0; //No point continuing
  }

  bool no_errors = parse_regs(reg_strs, NUMREGS, &instr, file_context);
  for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);

  if (no_errors == false) return 0;
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
