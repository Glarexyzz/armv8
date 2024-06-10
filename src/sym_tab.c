//
// Created by arthurallilaire on 08/06/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "sym_tab.h"
#include "globals.h"

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

//
// int v = get_sym( char *s );
//	Get (s,v) from the symbol table and return v.
//	Returns -1 if s is NOT already present.
//
int get_sym( char *s )
{
    for( int i=0; i<nsymbols; i++ )
    {
        if( strcmp( symtab[i].s, s ) == 0 )
        {
            return symtab[i].v;
        }
    }
    return -1;
}


//
// add_sym( char *s, unsigned v );
//	Add (s,e) to the symbol table.
//	Aborts if s is already present.
//
void add_sym( char *s, unsigned v )
{
    assert( get_sym( s ) == -1 );
    assert( nsymbols < MAXSYMBOLS );
    assert( strlen(s) < MAXSYMBOLLEN );
    strcpy( symtab[nsymbols].s, s );
    symtab[nsymbols].v        = v;
    nsymbols++;
}


// show_symtab( out );
//	Display every (name,value) pair in the symbol table.
//
void show_symtab( FILE *out )
{
    for( int i=0; i<nsymbols; i++ )
    {
        // Print the symbol table (4-digit hex value (2-bytes), symbol name)
        fprintf( out, "%04x  %s\n", symtab[i].v, symtab[i].s );
    }
}

// Return true if line is a label (so line should be skipped), false otherwise
// Logs an error if a label is duplicated, label is not only a-zA-Z
bool build_symtab(context file_context, FILE *_) {
    // Check if line is a label
    if (file_context->cur_line[strlen(file_context->cur_line)-1] == ':') {
        // Remove colon
        file_context->cur_line[strlen(file_context->cur_line)-1] = '\0';
        if (get_sym(file_context->cur_line) != -1) {
            error("Error: Duplicate label", file_context);
            return true;
        }
        if (!isalpha_str(file_context->cur_line)){
            error("Error: Label must only contain a-zA-Z", file_context);
            return true;
        }
        // Add symbol to symbol table
        add_sym(file_context->cur_line, file_context->prog_lineno);
        return true;
    }
    return false;
}