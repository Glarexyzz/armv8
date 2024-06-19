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
#include "arith_instr.h"

#define PRINTONFAIL(c) {}
int main(int argc, char **argv) {
  if (argc != 3){
    fprintf(stderr, "Usage: %s <file_in> <file_out>\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Define input file
  const char *input_file_name = argv[1];
  FILE *input_file = fopen(input_file_name, "r");

  if (input_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", input_file_name);
    return EXIT_FAILURE;
  }

  const char *output_location = argv[2];

  //  Append .bin to file
  const char *extension = ""; // no .bin for tests
  char *output_file_name = (char *)malloc(strlen(output_location) +  strlen(extension) + 1);
  strcpy(output_file_name, output_location); strcat(output_file_name, extension);
  FILE *output_file = fopen(output_file_name, "wb");

  // TODO - turn into MACRO
  if (output_file == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", output_file_name);
    fclose(input_file);
    free(output_file_name);
    return EXIT_FAILURE;
  }

  // Should make output_file optional as symtab doesn't need it
  context file_context = create_context();
  parse_file(input_file, build_symtab, output_file, file_context);
  fseek(input_file, 0, SEEK_SET);
  parse_file(input_file, process_line, output_file, file_context);

  fclose(input_file);
  if( file_context->nerrors == 0 )		// no errors? output the file
	{
          free(file_context);
          free(output_file_name);
          return EXIT_SUCCESS;
	}
  fprintf( stderr, "%d errors\n", file_context->nerrors );
  unlink(output_file_name);
  free(file_context);
  free(output_file_name);
  return EXIT_FAILURE;
}
