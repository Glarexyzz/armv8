//
// Created by arthurallilaire on 13/06/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "utils.h"

// Return true for no errors, false otherwise
// Checks right number and no oversized, prints errors to error using file_context
// Frees reg_strs if errors found
bool split_string_error_checking(char *s, char **token_array, int max_size, int num_tokens, context file_context){
  int num_oversized = split_string(s, token_array, max_size, num_tokens);
  if (num_oversized > 0){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "%d oversized register labels (Max length for registers: %d)", num_oversized, max_size);
    error(error_message, file_context);
    for (int i = 0; i < num_tokens; i++) free(token_array[i]);
    return false;
  }
//  Check right number of registers
  for (int i = 0; i < num_tokens; i++){
    if (token_array[i] == NULL){
        char error_message[MAXERRORLEN];
        snprintf(error_message, MAXERRORLEN, "Not enough registers defined (Expected: %d, Found: %d)", num_tokens, i);
        error(error_message, file_context);
        for (int i = 0; i < num_tokens; i++) free(token_array[i]);
        return false;
    }
  }
  return true;
}

//  Max-size has to include the null value
//  Char s will be modified!
//  Each token will be null terminated
//  Token array will be null terminated if less values
//  Returns number of oversized tokens and modifies s, NULL if nothing after num_reg otherwise line that was left
int split_string(char *s, char **token_array, int max_size, int max_tokens) {
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
    *s = *saveptr;
    return num_oversized;
}


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