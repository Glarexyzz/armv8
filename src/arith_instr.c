//
// Created by arthurallilaire on 15/06/24.
//

#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#include "utils.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3
#include "arith_instr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t hw; // top 2 bits
  uint16_t imm16;
} wide_operand;

typedef struct {
  bool sh;
  uint16_t imm12;
  uint8_t rn;
} arith_operand;

typedef union {
  wide_operand w;
  arith_operand a;
} dp_imm_operand;

//  Using uint8_t as no uint4_t but only bottom 4 bits will be used
typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  uint8_t opc;
  uint8_t op0; // ALWAYS 100
  uint8_t opi;
  dp_imm_operand operand; // 5 - 22 bits (18 bits) - opi = 010 then arith, 101 wide move
  uint8_t rd;
} dp_imm_instr;

typedef struct {
  bool add; // true for add, false for sub
  uint8_t rd;
  uint8_t rn;
  bool imm; // if true then dp_imm_instr (no rm just imm12) - false then dp_reg_instr
  uint16_t imm12


} arith_instr;

//  Assumes well formatted dp reg
uint32_t dp_imm_to_binary(dp_imm_instr instr){
//  Based on opi return different
    uint32_t operand = 0;
    if (instr.opi == 2){
      operand |= (instr.operand.a.sh << 17);
      operand |= (instr.operand.a.imm12 << 5);
      operand |= (instr.operand.a.rn);
    } else {
      operand |= (instr.operand.w.hw << 17);
      operand |= (instr.operand.w.imm16 << 0);
    }
  return (instr.sf << 31) |
    (instr.opc << 29) |
    (instr.op0 << 25) |
    (operand << 5) |
    (instr.rd << 0);
}

// True if no-errors (as always)
// Not sure ", " bit is included??
bool parse_arith_operand(char *str_operand, dp_imm_instr *instr, arith_operand *operand, context file_context){
  printf("%s", str_operand);
  return true;
}

// Sets op0, opc and opi
// Returns number of registers to count (Not including operand)
// Will set rd (for cmp/n) to zr, rn (for neg) to zr
int set_arith_ops(char *opc, dp_imm_instr *instr){
  //  If add, sub, neg -> no set flags otherwise do
  TODO();
  if (strcmp(opc, "cmp") == 0 || strcmp(opc, "cmn") == 0);
  return 2;
}

uint32_t arithmetic_instr(char *opc, char * rest_instr, context file_context){
// Applies to: add(s), sub(s), cmp/n - set flags rd is zr, neg(s) - rn is zr
//  Two operand: <opcode> rd, rn, <operand> Applies to arithmetic and bit-logic operations.
//  Operand can be either rm or #<imm> with optional{, <shift> #<amount> }
//  If add or sub - rd, rn, <op2>, if cmp
  dp_imm_instr instr;
  int NUMREGS = set_arith_ops(opc, &instr);
  char *reg_strs[NUMREGS];
  bool no_errors =
      split_string_error_checking(rest_instr, reg_strs, MAXREGSTRLEN + 1, NUMREGS, file_context);
  if (!no_errors) return 0;
//  Parse the registers
  uint8_t reg_pointers[NUMREGS];

  no_errors = parse_regs(reg_strs, NUMREGS, &instr.sf, reg_pointers, file_context);

  for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
  if (!no_errors) return 0;

  if
  for (int i = 0; i < NUMREGS; i++){
      switch (i) {
        case 0: instr.rd = reg_pointers[i]; break;
        case 1: instr.operand.a.rn = reg_pointers[i]; break;
        default: error("Unexpected number of registers", file_context); return 0; // Unexpected number of registers
      }
    }

//  Set the right registers...
  for (int i = 0; i < NUMREGS; i++){
    switch(i){
      case 1: instr.rd = re
  if (NUMREGS == 1){
    } else {

      switch (i) {

      }
//  Set the operand correctly
  arith_operand operand;
  no_errors = parse_arith_operand(rest_instr, &instr, &operand, file_context);
  if (!no_errors) return 0;

  return dp_imm_to_binary(instr);
}