//
// Created by arthurallilaire on 13/06/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "utils.h"

bool isfunc_str(const char *str, int (* isfunc)(int)) {
    while (*str) {
        if (!isfunc(*str)) {
            return false; // Return 0 (false) if any character is not alphabetic
        }
        str++;
    }
    return true; // Return 1 (true) if all characters are alphabetic
}

// Helper function to trim whitespace
char* trim_whitespace(const char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return strdup("");

    // Trim trailing space
    end = (char *)(str + strlen(str) - 1);
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return strdup(str);
}



// Return true for no errors, false otherwise
// Checks right number and no oversized, prints errors to error using file_context
// Frees reg_strs if errors found
bool split_string_error_checking(const char *s, char **token_array, int max_size, int num_tokens, char **rest_instr, context file_context, bool no_oversized, bool no_over, bool nothing_after){
  int num_oversized = split_string(s, token_array, max_size, num_tokens, rest_instr);
  if (no_oversized && num_oversized > 0){
    char error_message[MAXERRORLEN];
    snprintf(error_message, MAXERRORLEN, "%d oversized register labels (Max length for registers: %d)", num_oversized, max_size);
    error(error_message, file_context);
    for (int i = 0; i < num_tokens; i++) free(token_array[i]);
    return false;
  }
//  Check right number of registers
  if (no_over){
  for (int i = 0; i < num_tokens; i++){
    if (token_array[i] == NULL){
        char error_message[MAXERRORLEN];
        snprintf(error_message, MAXERRORLEN, "Not enough registers defined (Expected: %d, Found: %d)", num_tokens, i);
        error(error_message, file_context);
        for (int i = 0; i < num_tokens; i++) free(token_array[i]);
        return false;
    }
  }
  }
  if (nothing_after){
    //  Check nothing after registers
      if (*rest_instr != NULL){
        char error_message[MAXERRORLEN];
        snprintf(error_message, MAXERRORLEN, "Too many clauses (Expected: %d). Extra: %s", num_tokens, *rest_instr);
        error(error_message, file_context);
        for (int i = 0; i < num_tokens; i++) free(token_array[i]);
        return false; //No point continuing
      }
  }
  return true;
}

//  Max-size has to include the null value
//  Char s will be modified!
//  Each token will be null terminated
//  Token array will be null terminated if less values
//  Returns number of oversized tokens and modifies rest_instr (NULL if nothing after num_reg otherwise line that was left)
int split_string(const char *s, char **token_array, int max_size, int max_tokens, char **rest_instr) {
    char *str1 = trim_whitespace(s); // Duplicate the input string to avoid modifying the original
    if (str1 == NULL) {
        fprintf(stderr, "Internal strdup error!!");
        return -1;
    }

    char *saveptr;
    int num_oversized = 0;
    int j;

    for (j = 0; j < max_tokens; j++) {
        char *token = strtok_r(j == 0 ? str1 : NULL, ", ", &saveptr); // First time str1 otherwise NULL
        if (token == NULL) {
            // No more tokens
            break;
        }

        // Allocate memory for the token
        token_array[j] = (char *)malloc(max_size);
        if (token_array[j] == NULL) {
            // Handle memory allocation failure
            fprintf(stderr, "Internal malloc error!!");
            // Free previously allocated memory
            for (int k = 0; k < j; k++) free(token_array[k]);
            free(str1); // Free the duplicated string
            return -1;
        }

        // Copy the token and ensure it's null-terminated
        strncpy(token_array[j], token, max_size);
        token_array[j][max_size - 1] = '\0'; // Ensure string is null-terminated

        // Check if the token was oversized
        if (strlen(token) >= max_size) {
            num_oversized++;
        }
    }

    // Indicate end of tokens in the array - make all of them NULL to avoid double free errors
    for (; j < max_tokens; j++) token_array[j] = NULL;

    // Point rest_instr to the rest of the input string if any
    if (saveptr != NULL && *saveptr != '\0') {
        *rest_instr = strdup(saveptr); // Duplicate the rest of the string
        if (*rest_instr == NULL) {
            fprintf(stderr, "Internal strdup error!!");
            for (int k = 0; k < j; k++) {
                free(token_array[k]);
            }
            free(str1); // Free the duplicated string
            return -1;
        }
    } else {
        *rest_instr = NULL;
    }

    free(str1); // Free the duplicated string
    return num_oversized;
}

