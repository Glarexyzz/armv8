//
// Created by aa8123 on 10/06/24.
//
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "globals.h"

int NERRORS = 0; // number of assembly time errors (Global)
int CUR_LINENO = 0;
char CUR_LINE[MAXLINELEN] = "";
//	Produce a standard-formatted error
//
void error_glob_vals(char *msg){
    fprintf( stderr, "Line %d: %s\n\t\t%s\n", CUR_LINENO, CUR_LINE, msg );
    NERRORS++;
}
void error( char *line, int lineno, char *msg )
{
    fprintf( stderr, "Line %d: %s\n\t\t%s\n", lineno, line, msg );
    NERRORS++;
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