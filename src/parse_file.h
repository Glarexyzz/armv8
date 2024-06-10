//
// Created by arthurallilaire on 10/06/24.
//

#ifndef PARSE_FILE_H
#define PARSE_FILE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>


typedef bool (*line_processor)(char *line, int lineno, FILE *outputFile);


// Prototypes
void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile);
uint32_t parseInstruction(char *line);
bool process_line(char *line, int lineno, FILE *outputFile);
#endif //PARSE_FILE_H
