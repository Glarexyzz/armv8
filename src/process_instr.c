//
// Created by arthurallilaire on 08/06/24.
//

#include "process_instr.h"

static void split_string(char *s, char **reg_array, int max_reg){
//  Char s will be modified!
	char *str1 = s; char *saveptr;

	for (int j = 0; j < max_reg; j++, str1=NULL){
          reg_array[j] = strtok_r(str1, ", ", &saveptr);
          if (reg_array[j] == NULL){
            break;
          }
    }
    return ;
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
uint32_t multiply_instr(char *opc, char * rest_instr, context file_context){
//    Applies to madd, msub, mul, mneg
char *saveptr;
	char *rd = strtok_r(rest_instr, ", ", &saveptr);
	char *rn = strtok_r(NULL, ", ", &saveptr);
	char *rm = strtok_r(NULL, ", ", &saveptr);
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