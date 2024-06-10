//
// Created by arthurallilaire on 10/06/24.
//
#include "globals.h"
#include "parse_file.h"
#include "dispatch.h"

void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile) {
    // Read input file line by line
    for ( int lineno = 0; fgets( CUR_LINE, MAXLINELEN, inputFile ) != NULL; lineno++ )
    {
        // Remove newline character
        int len = strlen(CUR_LINE);
        if( CUR_LINE[len-1] == '\n' )
        {
            CUR_LINE[len-1] = '\0';
        } else {
            fprintf( stderr, "Line %d too long\n", lineno );
            exit( EXIT_FAILURE );
        }
        // Always increment CUR_LINENO as includes labels as well
        CUR_LINENO++;
        // Skip empty lines and comments and build_symbol table if label then skip line for countine
        if (CUR_LINE[0] == '#' || CUR_LINE[0] == '\0' || processor(CUR_LINE, lineno, outputFile)) {
            lineno--; // Ignore blank lines
            continue;
        }
    }
}
//returns an instruction
uint32_t parseInstruction(char *line) {
    char *opc_str;
    char *saveptr;
    char *rest_instr;
    opc_str = strtok_r(line, " ", &saveptr);
    instr_processor opc_fun = get_instr_processor(opc_str);
    if (opc_fun == NULL) error_glob_vals("Invalid op-code!");

    rest_instr = strtok_r(NULL, "", &saveptr);
    rest_instr = (rest_instr == NULL) ? "" : rest_instr;

    return opc_fun(opc_str, rest_instr);
}

bool process_line(char *line, int lineno, FILE *outputFile) {
    // Check if line is a label
    if (line[strlen(line)-1] == ':') {
        return true; // Skip all labels
    }
    uint32_t binary_instr = parseInstruction(line);
    int num_instr = 1;
    int num_instr_w = fwrite(&binary_instr, sizeof(binary_instr), num_instr, outputFile);
    if (num_instr != num_instr_w){
        error(line, lineno, "Unable to write instruction to output file.");
    }
    return false;
}