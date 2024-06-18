//
// Created by arthurallilaire on 18/06/24.
//

#include "parse_utils.h"
#include "utils.h"
#include <ctype.h>


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
  instr->imm = false;
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

//-1 if error raised, 0 for w (32), 1 for x (64)
int get_sf(char **reg_strs, int max_num_regs, context file_context){
  char old_sf_str = reg_strs[0][0];
  for (int i = 0; i < max_num_regs; i++){
    char sf_str = reg_strs[i][0];
    if (sf_str != 'x' && sf_str != 'w'){
      error("Register names must start with 'x' or 'w'", file_context);
      return -1;
    }
    if (sf_str != old_sf_str){
      error("Conflicting register bit sizes", file_context);
      return -1;
    }
  }
  return old_sf_str=='x' ? 1 : 0; //1 for 64, o
}

//Num or zr should be
//-1 if error raised (invalide reg_num - catches NULL registers and returns -1)
int get_reg_num(char *reg_str, context file_context){
  if(reg_str == NULL){ //Should have already been checked
    error("Missing register value", file_context);
    return -1;
  }
  char reg_num_str[2]; //either NN or zr
  strncpy(reg_num_str, reg_str+1, 2); //skip sf
  if (strcmp(reg_num_str, "zr") == 0){
    return 31;
  }
  //  Could be single digit
  if (isdigit(reg_num_str[0]) && (isdigit(reg_num_str[1]) || reg_num_str[1] == '\0')){
    int reg_num = atoi(reg_num_str);
    if (reg_num > 30){
      error("Register index must be 0<=n<= 30 or zr", file_context);
      return -1;
    }
    return reg_num;
  }
  error("Register index must be an Int or zr", file_context);
  return -1;
}


//Returns true if no errors parsed - false for errors
bool parse_regs(char **reg_strs, int num_regs, bool *sf, uint8_t *regs, context file_context){
  int res = get_sf(reg_strs, num_regs, file_context);
  if (res < 0) return false;
  *sf = (bool) res;
  for (int i = 0; i < num_regs; i++){
    res = get_reg_num(reg_strs[i], file_context);
    if (res < 0) return false;
    regs[i] = res;
  }
  return true;
}

// True if no-errors (as always) - modifies instr_operand * sets to the right values
// The ", " bit not included - or about shifts for now
// Either #imm, lsl #[0 or 12] or #imm, shift_type #value
// Check nothing after instruction
// Checks shift value is within bounds based on sf

bool imm_to_int(char *imm_str, uint16_t *imm_res, int max, context file_context){
  // Check valid number
  int base = 0; // Checks both int and hex
  char *endptr;
  long val = strtol(imm_str, &endptr, base);
  *imm_res = (uint16_t) val;
  ERROR_ON_COND(*endptr != '\0', "Invalid characters found", file_context);
  ERROR_ON_COND(endptr == imm_str, "No digits found!!", file_context);
  //  Check less than max val
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