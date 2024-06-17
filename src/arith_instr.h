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
#include "process_instr.h"


typedef struct {
    uint16_t imm12;
    bool lsl12; //Only LSL 12 as option or not
} imm_operand;

typedef struct {
    uint8_t rm;
    shift_enum shift_type;
    uint8_t shift_amount;
} reg_operand;

typedef union {
    imm_operand imm;
    reg_operand reg;
} operand_union;

typedef struct {
    operand_union val;
    bool imm; // True if imm_operand, false for reg
} instr_operand;


typedef struct {
    bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
    bool sub; // true for add, false for sub
    bool flags; // true to set flags false no set
    uint8_t rd;
    uint8_t rn;
    bool imm; // if true then dp_imm_instr (no rm just imm12) - false then dp_reg_instr
    instr_operand operand;
} arith_instr;

uint32_t arithmetic_instr(char *opc, char * rest_instr, context file_context);

#endif //ARITH_INSTR_H
