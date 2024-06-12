//
// Created by arthurallilaire on 08/06/24.
//
#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3
#define MAXERRORLEN 128

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Max-size has to include the null value
//  Char s will be modified!
//  Each token will be null terminated
//  Token array will be null terminated if less values
//  Returns number of oversized tokens and modifies s, NULL if nothing after num_reg otherwise line that was left
static int split_string(char *s, char **token_array, int max_size, int max_tokens) {
  char *str1 = s;
  char *saveptr;
  int num_oversized = 0;

  for (int j = 0; j < max_tokens; j++, str1 = NULL) {
    char *token = strtok_r(str1, ", ", &saveptr);
    if (token == NULL) {
      // No more tokens
      token_array[j] = NULL; // Indicate end of tokens
      return num_oversized;
    }

    // Allocate memory for the token
    token_array[j] = (char *)malloc(max_size);
    if (token_array[j] == NULL) {
      // Handle memory allocation failure
      fprintf(stderr, "Internal malloc error!!");
      return -1;
    }

    // Copy the token and ensure it's null-terminated
    strncpy(token_array[j], token, max_size);
    token_array[j][max_size - 1] = '\0'; // Ensure string is null-terminated

    // Check if the token was oversized
    if (strlen(token) > max_size - 1) {
      num_oversized++;
    }
  }
  // Add remains of line to s
  s = saveptr;
  return num_oversized;
}

uint32_t arithmetic_instr(char *opc, char * rest_instr, context file_context){
    return 0;
}
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

//typedef struct m_instr *minstr;
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
