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
#include "globals.h"

typedef bool (*line_processor)(context file_context, FILE *outputFile);


// Prototypes
void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile, context file_context);
uint32_t parseInstruction(context file_context);
bool process_line(context file_context, FILE *outputFile);
#endif //PARSE_FILE_H
