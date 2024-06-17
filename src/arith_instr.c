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

// True if no-errors (as always) - modifies instr_operand * sets to the right values
// The ", " bit not included - or about shifts for now
// Either #imm, lsl #[0 or 12] or #imm, shift_type #value
// Check nothing after instruction
// Checks shift value is within bounds based on sf

bool imm_to_int(char *imm_str, uint16_t *imm_res, int max, context file_context){
  // Check valid number
  if (!isfunc_str(imm_str, &isdigit)){
    error("Immediate value must be a digit!!", file_context);
    return false;
  }
  *imm_res = atoi(imm_str);
  //  Check less than 2^12
  if (*imm_res > max){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "Immediate value must be less than %d", max);
    error(error_message, file_context);
    return false;
  }
  return true;
}

bool get_shift(const char *shift, shift_enum *shift_type, context file_context){
  if (strcmp(shift, "lsl") == 0) *shift_type = LSL;
  else if (strcmp(shift, "lsr") == 0) *shift_type = LSR;
  else if (strcmp(shift, "asr") == 0) *shift_type = ASR;
  else if (strcmp(shift, "ror") == 0) *shift_type = ROR;
  else {
    error("Unidetifiable shift type.", file_context);
    return false;
    }
  return true;
}
bool parse_operand(char *str_operand, instr_operand *instr, bool sf, context file_context){
//  char *saveptr;
//  char *token =
  #define MAXOPTOKENS 3
  #define MAXLENTOKEN 16
  #define MAXIMMVAL (1 << 12) - 1 // 2^12 -1 is max
  char *op_tokens[MAXOPTOKENS];
  char *rest_instr; // Will be NULL as checks nothing after
  bool no_errors = split_string_error_checking(str_operand, op_tokens, MAXLENTOKEN, MAXOPTOKENS, &rest_instr, file_context, true, false, true);
  if (!no_errors) return no_errors; // propogate error state
  //  Check to see if immediate
  if (op_tokens[0][0] == '#'){
    instr->imm = true;
    no_errors = imm_to_int(op_tokens[0]+1, &instr->val.imm.imm12, MAXIMMVAL, file_context);
    FREE_ON_COND(!no_errors, op_tokens, MAXOPTOKENS);

    if (op_tokens[1] != NULL){ // Has a shift
      shift_enum shift_type;
      no_errors = get_shift(op_tokens[1],  &shift_type, file_context);
      FREE_ON_COND(!no_errors, op_tokens, MAXOPTOKENS);
      ERROR_AND_FREE_ON_COND(shift_type != LSL, "Only lsl allowed with Imm value", file_context, op_tokens, MAXOPTOKENS);

      uint16_t shift_val = 0; int max_shift_val = 12;
      // Skip #
      no_errors = imm_to_int(op_tokens[2]+1, &shift_val, max_shift_val, file_context);
      FREE_LIST(op_tokens, MAXOPTOKENS); // No longer needed

      if (shift_val == 12) instr->val.imm.lsl12 = true;
      else if (shift_val == 0) instr->val.imm.lsl12 = false;
      else {error("Left shift value must be 0 or 12", file_context); no_errors = false;}
      return no_errors;
    } else { instr->val.imm.lsl12 = false; FREE_LIST(op_tokens, MAXOPTOKENS); return true;}
  }
//  Parse as register
  bool new_sf;
  no_errors = parse_regs(&op_tokens[0], 1, &new_sf, &instr->val.reg.rm, file_context);
  FREE_ON_COND(!no_errors, op_tokens, MAXOPTOKENS);
  ERROR_AND_FREE_ON_COND(new_sf != sf, "All registers must either be 32(w) or 64 (x)", file_context, op_tokens, MAXOPTOKENS);

  if (op_tokens[1] != NULL){ // Has a shift
    no_errors = get_shift(op_tokens[1],  &instr->val.reg.shift_type, file_context);
    FREE_ON_COND(!no_errors, op_tokens, MAXOPTOKENS);

    uint16_t temp_shift_val; int max_shift_val = (1 << (5 + (int) sf)) - 1; // 2^6 - 1 if sf true otherwise 2^5 - 1
    no_errors = imm_to_int(op_tokens[2] + 1, &temp_shift_val, max_shift_val, file_context);
    instr->val.reg.shift_amount = (uint8_t) temp_shift_val;
  } else {
    instr->val.reg.shift_amount = 0;
    instr->val.reg.shift_type = LSL;
  }
  FREE_LIST(op_tokens, MAXOPTOKENS);
  return true;
}

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