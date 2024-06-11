//
// Created by arthurallilaire on 08/06/24.
//
#include <ctype.h>
#include "process_instr.h"
#include "globals.h"
#define MAXREGSTRLEN 3 // xNN or wzr - so always max 3
#define MAXNUMREGS 4
#define MAXERRORLEN 128

//  Char s will be modified!
//  Each token will be null terminated
//  Token array will be null terminated if less values
//  Returns number of oversized tokens
static int split_string(char *s, char **token_array, int max_size, int max_tokens){
	char *str1 = s; char *saveptr; int num_oversized = 0;

	for (int j = 0; j < max_tokens; j++, str1=NULL){
          char *token = strtok_r(str1, ", ", &saveptr);
          if (token == NULL){
            // No more tokens
            token_array[j] = NULL; //
            return num_oversized;
          }
          strncpy(token_array[j], token, max_size);
          if (token[max_size] != '\0'){
            token_array[j][max_size] = '\0';
            num_oversized++;
          }
    }
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

typedef struct {
    uint8_t rm;
    uint8_t ra;
    uint8_t rn;
    uint8_t rd;
    bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
} registers;
//typedef struct regs *regs; - getting clion errors :(

typedef struct {
    //  Using uint8_t as no uint4_t but only bottom 4 bits will be used
    registers *r;
    bool x; //0 for m-add, 1 for m-sub - (do you negate the multiplication)
} m_instr;


//typedef struct m_instr *minstr;
//-1 if error raised, 0 for w (32), 1 for x (64)
int get_sf(char **reg_strs, int max_num_regs, context file_context){
  char old_sf_str = reg_strs[0][0];
  for (int i = 0; i < max_num_regs; i++){
      char sf_str = reg_strs[i][0];
      if (sf_str != 'x' || sf_str != 'w'){
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
//-1 if error raised (invalide reg_num)
int get_reg_num(char *reg_str, context file_context){
  char reg_num_str[2]; //either NN or zr
  int reg_num;
  strncpy(reg_num_str, reg_str+1, 2);
  if (strcmp(reg_num_str, "zr") == 0){
    return 31;
  }
  if (isdigit(reg_num_str[0]) && isdigit(reg_num_str[1])){
    reg_num = atoi(reg_num_str);
    if (reg_num > 30){
      error("Register index must be 0<=n<= 30 or zr", file_context);
      return -1;
    }
    return reg_num;
  }
  error("Register index must be an int or zr", file_context);
  return -1;
}


//Sets rd to 31 (ZR) if not defined
//Returns true if no errors parsed
bool parse_regs(char **reg_strs, int max_num_regs, registers *regs, context file_context){
  int res = get_sf(reg_strs, max_num_regs, file_context);
  if (res < 0) return false;
  regs->sf = (bool) res;
  for (int i = 0; i < max_num_regs; i++){
    res = get_reg_num(reg_strs[i], file_context);
    if (res < 0) return false;
//    Add to one register at a time??
//    TODO - Handle default rd to zr..
      switch (i) {
          case 0: regs->rm = (uint8_t) res; break;
          case 1: regs->ra = (uint8_t) res; break;
          case 2: regs->rn = (uint8_t) res; break;
          case 3: regs->rd = (uint8_t) res; break;
          default: return false; // Unexpected number of registers
      }
  }
  return true;
}



uint32_t multiply_instr(char *opc, char * rest_instr, context file_context){
//    Applies to madd, msub, mul, mneg
    char *regs[MAXNUMREGS];
    char buffer[MAXNUMREGS][MAXREGSTRLEN+1];
    //    +1 for null byte
    int num_oversized = split_string(rest_instr, regs, MAXREGSTRLEN + 1, MAXNUMREGS);
    if (num_oversized > 0){
      char error_message[MAXERRORLEN];
      snprintf(error_message, MAXERRORLEN, "%d registers were oversized (Max length for registers: %d)", num_oversized, MAXREGSTRLEN);
      error(error_message, file_context);
      return 0; //No point continuing
    }
//    Currently ignores num pointers > MAXNUMREGS - should probably fix that... (TODO)

    parse_regs(char *regs, MAXNUMREGS,
	if (strcmp(opc, "madd") || strcmp(opc, "msub")){
//          One more register
		char *ra = strtok_r(NULL, ", ", &saveptr);
        if (ra == NULL){
          error("Missing register for constant needed for madd and msub.", file_context);
    }
    if (rd == NULL || rn == NULL || rm == NULL){
      error("Missing register values", file_context);
    }
//    First letters all W or all X -
    //    Number 0 to 30 or zr

	}
	return 0;
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

//int main(void){
//  char *s1 = "R0, Rzr, W34";
//  char *regs[3];
//  split_string(s1, regs, 3);
//  for (int i = 0; i<3; i++){
//    printf("%s", *regs[i]);
//    }
//    return 1;
//}