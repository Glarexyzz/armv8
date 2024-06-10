//
// Created by arthurallilaire on 10/06/24.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>

#include "sym_tab.h"
#include "process_instr.h"
#include "opc_table.h"
#include "parse_file.h"


// typedef for line processor
typedef bool (*line_processor)(char *line, int lineno, FILE *outputFile);

// Could be its own file + header
void parse_file(FILE *, line_processor, FILE *);

// Could be its own file but too small for now
void error( char *line, int lineno, char *msg );
int nerrors = 0;		// number of assembly time errors
// error( line, lineno, msg );
//	Produce a standard-formatted error
//
void error( char *line, int lineno, char *msg )
{
	fprintf( stderr, "Line %d: %s\n\t\t%s\n", lineno, line, msg );
	nerrors++;
}


bool isalpha_str(const char *str) {
    while (*str) {
        if (!isalpha(*str)) {
            return false; // Return 0 (false) if any character is not alphabetic
        }
        str++;
    }
    return true; // Return 1 (true) if all characters are alphabetic
}

//returns an array of strings
instruction parseInstruction(char *line) {
  //use strtok to tokenise and then maybe realloc?
  char *opc_str;
  char *saveptr;
  char *rest_instr;
  opc = strtok_r(line, " ", &saveptr);

  rest_instr = strtok_r(NULL, "", &saveptr);
  *rest_instr = (*rest_instr == NULL) ? "" : *rest_instr;

  instruction ins = {.opc = opc, .rest = str};

}

int main(int argc, char **argv) {
  if (argc != 3){
    fprintf(stderr, "Usage: %s <file_in> <file_out>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Define input file
  const char *inputFileName = argv[1];
  FILE *inputFile = fopen(inputFileName, "r");

  if (inputFile == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", inputFileName);
    return EXIT_FAILURE;
  }

  const char *outputFileName = argv[2];
  FILE *outputFile = fopen(outputFileName, "wb");

  if (outputFile == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", outputFileName);
    return EXIT_FAILURE;
  }

  // Should make outputFile optional
  parse_file(inputFile, build_symtab, outputFile);
  fseek(inputFile, 0, SEEK_SET);
  parse_file(inputFile, process_line, outputFile);

  fclose(inputFile);

	if( nerrors == 0 )		// no errors? output the file
	{
		fputs( "\n",  outputFile);
    show_symtab(outputFile);
    return EXIT_SUCCESS;
	} else {
    fprintf( stderr, "%d errors\n", nerrors );
    unlink(outputFileName);
    return EXIT_FAILURE;
  }



  return EXIT_SUCCESS;
}