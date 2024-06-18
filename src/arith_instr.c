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
#include "parse_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int initialise_arith_instr(char *opc, arith_instr *instr){
  // Set bool: add, flags, rd/rn if needed (for neg/cmp(n))
//  Returns numregs expected (not including operand)
  int NUMREGS = 2;
  if (strncmp(opc, "add", 3) == 0 || strncmp(opc, "neg", 3) == 0) instr->sub = false;
  else instr->sub = true;

  // check if opc is add, sub or neg for flags
  if (strstr("add sub neg", opc) != NULL) instr->flags = false;
  else instr->flags = true;

  if (strncmp(opc, "neg", 3) == 0 || strncmp(opc, "cm", 2) == 0){
    NUMREGS--;
  }
  return NUMREGS;
}

uint32_t arithmetic_instr(char *opc, char *str_instr, context file_context){
// Applies to: add(s), sub(s), cmp/n - set flags rd is zr, neg(s) - rn is zr
//  Two operand: <opcode> rd, rn, <operand> Applies to arithmetic and bit-logic operations.
//  Operand can be either rm or #<imm> with optional{, <shift> #<amount> }
//  If add or sub - rd, rn, <op2>, if cmp
  arith_instr instr;
  int NUMREGS = initialise_arith_instr(opc, &instr);
  char *reg_strs[NUMREGS];
  char *rest_instr;
//  Don't check nothing_after, do check right num and not_oversized
  bool no_errors =
      split_string_error_checking(str_instr, reg_strs, MAXREGSTRLEN + 1, NUMREGS, &rest_instr, file_context, true, true, false);
  if (!no_errors) return 0;
//  Parse the registers
  uint8_t reg_pointers[NUMREGS];

  no_errors = parse_regs(reg_strs, NUMREGS, &instr.sf, reg_pointers, file_context);

  for (int i = 0; i < NUMREGS; i++) free(reg_strs[i]);
  if (!no_errors) return 0;

// Any writes to 11111 with |= no effect
  if (NUMREGS == 1) {
// to set cmp/n 2nd register (first 11111)
    if (strncmp(opc, "neg", 3) == 0){
      instr.rn = ZR; // 11111
      instr.rd = reg_pointers[0];
    } else if (strncmp(opc, "cm", 2) == 0){
      instr.rd = ZR;
      instr.rn = reg_pointers[0];
    }

  } else {
    for (int i = 0; i < NUMREGS; i++){
        switch (i) {
          case 0: instr.rd = reg_pointers[i]; break;
          case 1: instr.rn = reg_pointers[i]; break;
          default: error("Unexpected number of registers", file_context); return 0; // Unexpected number of registers
        }
      }
  }
//  First two registers are right - just operand left
//  Set the operand correctly - will check nothing after instruction
  no_errors = parse_operand(rest_instr, &instr.operand, instr.sf, file_context);
  if (!no_errors) return 0;

  return arith_instr_to_binary(instr);
}