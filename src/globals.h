//
// Created by aa8123 on 10/06/24.
//

#ifndef ARMV8_2_GLOBALS_H
#define ARMV8_2_GLOBALS_H

#include <stdbool.h>

#define MAXLINELEN 1024
#define MAXERRORLEN 128
#define ZR 31

struct context {
    int nerrors;
    char cur_line[MAXLINELEN];
    int prog_lineno; // Only program instructions
    int file_lineno; // Includes labels, new lines and comments
};
typedef struct context *context;
//extern context GLOBAL_CONTEXT; - decided to define in assemble
context create_context(void);
void reset_linenos(context context);
void free_context(context context);
void error( char *msg, context context );
bool isalpha_str(const char *str);

#endif //ARMV8_2_GLOBALS_H
