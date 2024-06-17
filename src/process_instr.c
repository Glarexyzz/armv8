//
// Created by arthurallilaire on 08/06/24.
//
#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#include "utils.h"
#include "sym_tab.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"

/* SRAP FOR NOW
typedef struct {
  uint8_t hw; // top 2 bits
  uint16_t imm16;
} wide_operand;

typedef struct {
  bool sh;
  uint16_t imm12;
  uint8_t rn;
} arith_imm_operand;

typedef union {
  wide_operand w;
  arith_imm_operand a;
} dp_imm_operand;

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
*/



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

//Should be similar to/same as parse_arith_operand?
bool parse_logic_operand(char *str_operand, logic_operand_parse_result *result, context file_context) {
  // str_operand of form rm or rm{, shift #amount} - the brackets are to show it's optional not actually in code.
  result->rm = get_reg_num(strtok(str_operand, ", #"),file_context);

  char* shiftType = strtok(NULL, ", #");
  if (shiftType == NULL) {
    // operand is just rm, so shift rm by 0
    result->shType == 0;
    result->operand == 0;
  } else {
    if (strcmp(shiftType, "lsl")) {result->shType = 0;} else
    if (strcmp(shiftType, "lsr")) {result->shType = 1;} else
    if (strcmp(shiftType, "asr")) {result->shType = 2;} else
    if (strcmp(shiftType, "ror")) {result->shType = 3;} else
    result->operand = (uint8_t)atoi(strtok(NULL, ", #"));
  }
  return true;
}

uint32_t logical_instr(char *opc, char * str_instr, context file_context){
  int NUMARGS = 3; // rd, rn, opearand
  dp_reg_instr instr;
  logic_operand_parse_result parse_result;
  bool N;
  char** argArray;
  char *rest_instr;

  //Set opcode and N
  if (strcmp(opc, "and") == 0) {instr.opc = 0; N = 0;} else
  if (strcmp(opc, "bic") == 0) {instr.opc = 0; N = 1;} else
  if (strcmp(opc, "orr") == 0) {instr.opc = 1; N = 0;} else
  if (strcmp(opc, "orn") == 0) {instr.opc = 1; N = 1;} else
  if (strcmp(opc, "eor") == 0) {instr.opc = 2; N = 0;} else
  if (strcmp(opc, "eon") == 0) {instr.opc = 2; N = 1;} else
  if (strcmp(opc, "ands") == 0) {instr.opc = 3; N = 0;} else
  if (strcmp(opc, "bics") == 0) {instr.opc = 3; N = 1;} else
  {return 0;}

  // +1 for the null byte
  int num_oversized = split_string(str_instr, argArray, MAXREGSTRLEN + 1, NUMARGS, &rest_instr);
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

// Applies to madd, msub, mul, mneg
uint32_t multiply_instr(char *opc, char * instr_str, context file_context){
  // Initialise registers
  int NUMREGS = 4;
  dp_reg_instr instr;
  if (strcmp(opc, "mul") == 0 || strcmp(opc, "mneg") == 0) {instr.rd = 31; NUMREGS--;}
  //x is top bit or operand (ls 6) - 0 for m-add, 1 for m-sub - (do you negate the multiplication)
  if (strcmp(opc, "msub") == 0 || strcmp(opc, "mneg") == 0) instr.operand |= (1 << 6);
  char *reg_strs[NUMREGS]; char *rest_instr;
//  +1 for the null byte
  bool no_errors = split_string_error_checking(instr_str, reg_strs, MAXREGSTRLEN + 1, NUMREGS, &rest_instr, file_context, true, true, true);
  if (!no_errors) return 0; // reg_strs freed by error_checking func

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
/*
  Branch structure
*/
typedef enum BranchType {
    UNCONDITIONAL,
    REGISTER,
    CONDITIONAL
} branch_type;

typedef struct uncond {
    uint8_t uncond_start; // 6 bits
    int32_t simm26; // 26 bits
  } uncond;

typedef struct reg {
  uint32_t reg_start; // 22 bits
  uint8_t xn; // 5 bits
  uint8_t reg_finish; // 5 bits
} reg;

typedef struct cond {
  uint8_t cond_start; // 8 bits
  int32_t simm19; // 19 bits
  bool cond_mid; // 1 bit
  uint8_t cond; // 4 bits
} cond;

typedef union {
  uncond uncond;
  reg reg;
  cond cond;
} branch;

// Converts a branch instruction into a binary number
uint32_t branch_to_binary(branch instr, branch_type type){
  switch(type) {
    case UNCONDITIONAL:
      return (instr.uncond.uncond_start << 26) |
              instr.uncond.simm26;
      break;

    case REGISTER:
      return (instr.reg.reg_start << 10) |
          (instr.reg.xn << 5) |
          instr.reg.reg_finish;
      break;

    case CONDITIONAL:
      return (instr.cond.cond_start << 24 ) |
              (instr.cond.simm19 << 5) |
              (instr.cond.cond_mid << 4) |
              instr.cond.cond;
      break;
    default:
      return EXIT_FAILURE;
  }
}

// Breaks down a branch instruction
uint32_t branch_instr(char *opc, char *rest_instr, context file_context) {
  // Find branch type and work on each type individually
  branch instr;
  branch_type b_type;

  // Unconditional branch
  if(strcmp(opc, "b") == 0) {
    b_type = UNCONDITIONAL;
    instr.uncond.uncond_start = 5;
    instr.uncond.simm26 = get_sym(rest_instr) - file_context->prog_lineno;
  }

  // Register branch
  else if(strcmp(opc, "br") == 0) {
    b_type = REGISTER;
    instr.reg.reg_start = 3508160;
    instr.reg.xn = atoi(rest_instr);
    instr.reg.reg_finish = 0;
  }

  // Conditional branch
  else {
    b_type = CONDITIONAL;

    instr.cond.cond_start = 84;
    instr.cond.simm19 = get_sym(rest_instr)  - file_context->prog_lineno;
    instr.cond.cond_mid = 0;

    if(strcmp(opc, "b.eq") == 0) instr.cond.cond = 0;
    else if(strcmp(opc, "b.ne") == 0) instr.cond.cond = 1;
    else if(strcmp(opc, "b.ge") == 0) instr.cond.cond = 10;
    else if(strcmp(opc, "b.lt") == 0) instr.cond.cond = 11;
    else if(strcmp(opc, "b.gt") == 0) instr.cond.cond = 12;
    else if(strcmp(opc, "b.le") == 0) instr.cond.cond = 13;
    else if(strcmp(opc, "b.al") == 0) instr.cond.cond = 14;
  }

  // Return binary representation as uint32_t
  return branch_to_binary(instr, b_type);
}

// Breaks down a Single Data Transfer instruction
uint32_t sdt_instr(char *opc, char * rest_instr, context file_context){
  // Find branch type and work on each type individually
  sdt instr;
  sdt_type sdt_type;

  // Setup rt & sf
  bool sf;
  uint8_t rt;

  // Setup tokens
  const char delim[] = " ,x[";
  char *saveptr;
  
  // First token contains RT
  char * fsttoken = strtok_r(rest_instr, delim, saveptr); 
  sf = (fsttoken[0] == 'w') ? 0 : 1; 
  rt = (sf == 0) ? atoi(++fsttoken) : atoi(fsttoken); //remove prefix "w"

  // Second token records xn or literal
  char *sndtoken = strtok_r(NULL, delim, saveptr);
  int sndlen = strlen(sndtoken);
  sndtoken[sndlen - 1] = (sndtoken[sndlen - 1] == ']') ? "\0" : sndtoken[sndlen - 1];

  // Check if the second token is literal or xn
  if (sndtoken != '0' && atoi(sndtoken) == 0) {
    // Load Literal
    sdt_type = LL;
    int32_t simm19;

    // True -> Immediate Address Literal ; False -> Label Literal
    int32_t literaladdress = (sndtoken[0] == '#') ? atoi(++sndtoken) : get_sym(sndtoken);
    simm19 = literaladdress - file_context->prog_lineno;
    assert(abs(simm19) < (1 << 20)); //address of literal within 1MB of the current address

    // Store values into the instruction structure
    instr.ll.ll_start = 0;
    instr.ll.sf = sf;
    instr.ll.ll_mid1 = 24;
    instr.ll.simm19 = simm19;
    instr.ll.rt = rt;
  }

  // SDT
  else {
    sdt_type = SDT;
    bool u;
    int16_t offset;

    // Third token records the offset value
    char *trdtoken = strtok_r(NULL, delim, saveptr);
    int trdlen = strlen(trdtoken);

    // Zero Unsigned Offset
    if (trdtoken == NULL) {
      u = 1;
      offset = 0;
    }

    else if (trdtoken[0] == '#') {
      u = 0;
      trdtoken++;

      switch (trdtoken[trdlen - 1]) {
        // Unsigned Offset
        case ']':
          u = 1; // change it back
          trdtoken[trdlen - 1] = "\0"; // get rid of "]" so only the immValue left
          offset = (sf == 1) ? (atoi(trdtoken))/8 : (atoi(trdtoken))/4 ;
          break;

        // Pre-Indexed
        case '!':
          trdtoken[trdlen - 2] = "\0"; // get rid of "]!" so only the simmValue left
          offset = (0 << 11) |
            (atoi(trdtoken) << 2) |
            3;
          break;

        // Post-Indexed
        default:
          offset = (0 << 11) |
            (atoi(trdtoken) << 2) |
            1;
      }
    }

    // Register Offset
    else {
      u = 0;

      // Add Register into the offset
      trdtoken[trdlen - 1] = "\0"; // get rid of "]" so only the register number left
      offset = (1 << 11) |
        (atoi(trdtoken) << 6) |
        26;
    }

    // Store values into the instruction structure
    instr.sdt.sdt_start = 1;
    instr.sdt.sf = sf;
    instr.sdt.sdt_mid1 = 28;
    instr.sdt.U = u;
    instr.sdt.sdt_mid2 = 0;
    instr.sdt.L = (strcmp(opc, "ldr") == 0) ? 1 : 0;
    instr.sdt.offset = offset;
    instr.sdt.xn = atoi(sndtoken);
    instr.sdt.rt = rt;
  }

  // Return binary representation as uint32_t
  return sdt_to_binary(instr, sdt_type);
}

/*
  Directive instruction
*/
uint32_t directive_instr(char *opc, char * rest_instr, context file_context){

  // True -> Hexadecimal Value; False -> Decimal Value
  int32_t dirvalue = (rest_instr[1] == 'x') ? strtol(rest_instr, NULL, 16) : atoi(rest_instr);
  
  
  int *currentaddress = &(file_context->prog_lineno); // is this the current address?
  // Set the address of the current instruction to 4 bytes
  currentaddress = malloc(sizeof(int32_t)); 

  // update the assembly file of the value at the current address
  sprintf(file_context->cur_line, "%ld", dirvalue); 
  return dirvalue;
}
