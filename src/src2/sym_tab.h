//
// Created by arthurallilaire on 08/06/24.
//

#ifndef SYM_TAB_H
#define SYM_TAB_H
#include <stdbool.h>
#include <stdio.h>
// Symbol table:

#define MAXSYMBOLLEN 64
typedef char symbol[MAXSYMBOLLEN];

typedef struct {
    symbol s; //Up to 64 characters
    unsigned v;
} symval;

#define MAXSYMBOLS 1024

symval symtab[MAXSYMBOLS];
int nsymbols = 0;


// init_symtab();
//	Initialise the symbol table.
//
void init_symtab( void )
{
    nsymbols = 0;
}


// Prototypes
int get_sym( char *s );
void add_sym( char *s, unsigned v );
void show_symtab( FILE *out );
bool build_symtab(char *line, int lineno, FILE *outputFile);

#endif //SYM_TAB_H
