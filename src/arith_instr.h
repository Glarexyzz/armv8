//
// Created by arthurallilaire on 15/06/24.
//

#ifndef ARITH_INSTR_H
#define ARITH_INSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "globals.h"

uint32_t arithmetic_instr(char *opc, char * rest_instr, context file_context);

#endif //ARITH_INSTR_H
