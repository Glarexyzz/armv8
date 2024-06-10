#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>

#include "globals.h"
#include "sym_tab.h"
#include "process_instr.h"
#include "parse_file.h"

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
  context file_context = create_context();
  parse_file(inputFile, build_symtab, outputFile, file_context);
  fseek(inputFile, 0, SEEK_SET);
  reset_linenos(file_context);
  parse_file(inputFile, process_line, outputFile, file_context);

  fclose(inputFile);

  if( file_context->nerrors == 0 )		// no errors? output the file
	{
        return EXIT_SUCCESS;
	}
  fprintf( stderr, "%d errors\n", file_context->nerrors );
  unlink(outputFileName);
  return EXIT_FAILURE;
}
