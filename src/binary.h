//
// Created by arthurallilaire on 15/06/24.
//

#ifndef BINARY_H
#define BINARY_H

#include "process_instr.h"
#include "arith_instr.h"


uint32_t sdt_to_binary(sdt instr, sdt_type type);
//  Assumes well formatted dp reg
uint32_t dp_reg_to_binary(dp_reg_instr instr);
uint32_t dp_imm_to_binary(dp_imm_instr instr);
uint32_t arith_instr_to_binary(arith_instr instr);
#endif //BINARY_H
