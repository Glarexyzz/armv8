//
// Created by arthurallilaire on 15/06/24.
//

#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#include "utils.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3
#include "arith_instr.h"
#include "binary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {LSL, LSR, ASR, ROR} shift_enum;

typedef struct {
  uint16_t imm12;
  uint8_t shift_amount; //Always LSL
} imm_operand;

typedef struct {
  uint8_t rm;
  shift_enum shift_type;
  uint8_t shift_amount;
} reg_operand;

typedef union {
  imm_operand imm;
  reg_operand reg;
} operand_union;

typedef struct {
  operand_union val;
  bool imm; // True if imm_operand, false for reg
} instr_operand;


typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  bool add; // true for add, false for sub
  bool flags; // true to set flags false no set
  uint8_t rd;
  uint8_t rn;
  bool imm; // if true then dp_imm_instr (no rm just imm12) - false then dp_reg_instr
  instr_operand operand;
} arith_instr;


// True if no-errors (as always) - modifies instr_operand * sets to the right values
// Not sure ", " bit is included?? - or about shifts for now
// Check nothing after instruction
bool parse_operand(char *str_operand, instr_operand *instr, context file_context){
  printf("%s", str_operand);
  return true;
}

// Sets op0, opc and opi
// Returns number of registers to count (Not including operand)
// Will set rd (for cmp/n) to zr, rn (for neg) to zr
//int set_arith_ops(char *opc, dp_imm_instr *instr){
//  //  If add, sub, neg -> no set flags otherwise do
//  TODO();
//  if (strcmp(opc, "cmp") == 0 || strcmp(opc, "cmn") == 0);
//  return 2;
//}

int initialise_arith_instr(char *opc, arith_instr *instr){
  // Set bool: add, flags, rd/rn if needed (for neg/cmp(n))
//  Returns numregs expected (outside operand)
  return 0;
}

uint32_t arith_instr_to_binary(arith_instr instr){
//  Convert to dp_imm or dp_reg then use those helper functions
  return 0;
}

uint32_t arithmetic_instr(char *opc, char *rest_instr, context file_context){
// Applies to: add(s), sub(s), cmp/n - set flags rd is zr, neg(s) - rn is zr
//  Two operand: <opcode> rd, rn, <operand> Applies to arithmetic and bit-logic operations.
//  Operand can be either rm or #<imm> with optional{, <shift> #<amount> }
//  If add or sub - rd, rn, <op2>, if cmp
  arith_instr instr;
  int NUMREGS = initialise_arith_instr(opc, &instr);
  char *reg_strs[NUMREGS];
  bool no_errors =
      split_string_error_checking(rest_instr, reg_strs, MAXREGSTRLEN + 1, NUMREGS, file_context);
  if (!no_errors) return 0;
//  Parse the registers
  uint8_t reg_pointers[NUMREGS];

  no_errors = parse_regs(reg_strs, NUMREGS, &instr.sf, reg_pointers, file_context);

  for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
  if (!no_errors) return 0;

// Any writes to 11111 with |= no effect
  if (NUMREGS == 1) {
// to set cmp/n 2nd register (first 11111)
    instr.rn |= reg_pointers[0];
  }
  for (int i = 0; i < NUMREGS; i++){
      switch (i) {
        case 0: instr.rd |= reg_pointers[i]; break;
        case 1: instr.rn |= reg_pointers[i]; break;
        default: error("Unexpected number of registers", file_context); return 0; // Unexpected number of registers
      }
    }
//  First two registers are right - just operand left
//  Set the operand correctly - will check nothing after instruction
  no_errors = parse_operand(rest_instr, &instr.operand, file_context);
  if (!no_errors) return 0;

  return arith_instr_to_binary(instr);
}