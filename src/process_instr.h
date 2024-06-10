//
// Created by arthurallilaire on 08/06/24.
//

#ifndef PROCESS_INSTR_H
#define PROCESS_INSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "opc_table.h"

typedef uint32_t (*instr_processor)(char *opc, char * rest_instr);

uint32_t arithmetic_instr(char *opc, char * rest_instr);
uint32_t logical_instr(char *opc, char * rest_instr);
uint32_t wide_move_instr(char *opc, char * rest_instr);
uint32_t multiply_instr(char *opc, char * rest_instr);
uint32_t branch_instr(char *opc, char * rest_instr);
uint32_t sdt_instr(char *opc, char * rest_instr);
uint32_t directive_instr(char *opc, char * rest_instr);



#endif //PROCESS_INSTR_H
