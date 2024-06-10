//
// Created by aa8123 on 10/06/24.
//
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "globals.h"

//	Produce a standard-formatted error
void error( char *msg, context context )
{
    fprintf( stderr, "Line %d: %s\n\t\t%s\n", context->file_lineno, context->cur_line, msg );
    context->nerrors++;
}

// May become own utils package later...
bool isalpha_str(const char *str) {
    while (*str) {
        if (!isalpha(*str)) {
            return false; // Return 0 (false) if any character is not alphabetic
        }
        str++;
    }
    return true; // Return 1 (true) if all characters are alphabetic
}
//Default values - nerrors = 0, file_lineno = prog_lineno = 1
context create_context(void){
    context new = malloc(sizeof(struct context));
    assert(new != NULL);
    new->nerrors = 0; new->file_lineno = new->prog_lineno = 1;
    return new;
}
void reset_linenos(context context){
    context->file_lineno = 0; context->prog_lineno = 0;
}
void free_context(context context){
    free(context);
}