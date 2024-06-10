//
// Created by arthurallilaire on 08/06/24.
//

#ifndef SYM_TAB_H
#define SYM_TAB_H
#include <stdbool.h>
#include <stdio.h>


// Prototypes
int get_sym( char *s );
void add_sym( char *s, unsigned v );
void show_symtab( FILE *out );
bool build_symtab(char *line, int lineno, FILE *outputFile);

#endif //SYM_TAB_H
