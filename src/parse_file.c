//
// Created by arthurallilaire on 10/06/24.
//
#include "globals.h"
#include "parse_file.h"
#include "dispatch.h"

void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile, context file_context) {
    // Read input file line by line
    assert(file_context->prog_lineno == 1);
    for ( ; fgets( file_context->cur_line, MAXLINELEN, inputFile ) != NULL; file_context->prog_lineno++ )
    {
        // Remove newline character
        int len = strlen(file_context->cur_line);
        if( file_context->cur_line[len-1] == '\n' )
        {
            file_context->cur_line[len-1] = '\0';
        } else {
            fprintf( stderr, "Line %d too long\n", file_context->prog_lineno );
            exit( EXIT_FAILURE );
        }
        // Always increment file line number as includes labels as well
        file_context->file_lineno++;
        // Skip empty lines and comments and build_symbol table if label then skip line for continue
        if (file_context->cur_line[0] == '#' || file_context->cur_line[0] == '\0' || processor(file_context, outputFile)) {
            file_context->prog_lineno--; // Ignore blank lines
            continue;
        }
    }
}
//returns an instruction
uint32_t parseInstruction(context file_context) {
    char *opc_str;
    char *saveptr;
    char *rest_instr;
    opc_str = strtok_r(file_context->cur_line, " ", &saveptr);
    instr_processor opc_fun = get_instr_processor(opc_str);
    if (opc_fun == NULL) error("Invalid op-code!", file_context);

    rest_instr = strtok_r(NULL, "", &saveptr);
    rest_instr = (rest_instr == NULL) ? "" : rest_instr;

    return opc_fun(opc_str, rest_instr);
}

bool process_line(context file_context, FILE *outputFile) {
    // Check if line is a label
    if (file_context->cur_line[strlen(file_context->cur_line)-1] == ':') {
        return true; // Skip all labels
    }
    uint32_t binary_instr = parseInstruction(file_context);
    int num_instr = 1;
    int num_instr_w = fwrite(&binary_instr, sizeof(binary_instr), num_instr, outputFile);
    if (num_instr != num_instr_w){
        error("Unable to write instruction to output file.", file_context);
    }
    return false;
}