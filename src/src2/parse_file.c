//
// Created by arthurallilaire on 10/06/24.
//

#include "parse_file.h"

void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile) {
    // Read input file line by line
    char line[MAXLINELEN];
    for ( int lineno = 0; fgets( line, MAXLINELEN, inputFile ) != NULL; lineno++ )
    {
        // Remove newline character
        int len = strlen(line);
        if( line[len-1] == '\n' )
        {
            line[len-1] = '\0';
        } else {
            fprintf( stderr, "Line %d too long\n", lineno );
            exit( EXIT_FAILURE );
        }
        // Skip empty lines and comments and build_symbol table if label then skip line for counting
        if (line[0] == '#' || line[0] == '\0' || processor(line, lineno, outputFile)) {
            lineno--; // Ignore blank lines
            continue;
        }
    }
}

bool process_line(char *line, int lineno, FILE *outputFile) {
    // Check if line is a label
    if (line[strlen(line)-1] == ':') {
        return true;
    }
    char* opc;
    opc = strtok(line, " "); // Always return something (previous checks for null line)

    else if (line[0] == '.') {

        // Otherwise line is instruction
    } else {
        char** parsedIns = parseInstruction(line);
        char* opc = parsedIns[0];

        if (*opc == "add") {

        } else if (*opc == "adds") {

        }
        //etc
    }
    fprintf(outputFile, "%d: %s", lineno, line);
    return false;
}