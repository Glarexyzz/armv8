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

uint32_t arithmetic_instr(struct instruction);
uint32_t logical_instr(struct instruction);
uint32_t wide_move_instr(struct instruction);
uint32_t multiply_instr(struct instruction);
uint32_t branch_instr(struct instruction);
uint32_t sdt_instr(struct instruction);
uint32_t directive_instr(struct instruction);



#endif //PROCESS_INSTR_H
