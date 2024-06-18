//
// Created by arthurallilaire on 08/06/24.
//

#ifndef PROCESS_INSTR_H
#define PROCESS_INSTR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "globals.h"

typedef uint32_t (*instr_processor)(char *opc, char * rest_instr, context file_context);

uint32_t logical_instr(char *opc, char * rest_instr, context file_context);
uint32_t wide_move_instr(char *opc, char * rest_instr, context file_context);
uint32_t multiply_instr(char *opc, char * rest_instr, context file_context);
uint32_t branch_instr(char *opc, char * rest_instr, context file_context);
uint32_t sdt_instr(char *opc, char * rest_instr, context file_context);
uint32_t directive_instr(char *opc, char * rest_instr, context file_context);

//  Using uint8_t as no uint4_t but only bottom 4 bits will be used
typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  uint8_t opc;
  uint8_t op0; // 3 bits ALWAYS 100
  uint8_t opi;
  uint32_t operand; // 5 - 22 bits (18 bits)
  uint8_t rd;
} dp_imm_instr;

//  Using uint8_t as no uint4_t but only bottom 4 bits will be used
typedef struct {
  bool sf; //bit-width of registers - 0=32 (w), 1=64 (x-registers)
  uint8_t opc;
  bool M; //1 for Multiply - 0 for arithmetic, bit-logic
  uint8_t op0; // ALWAYS 101
  uint8_t opr;
  uint8_t rm;
  uint8_t operand;
  uint8_t rn;
  uint8_t rd;
} dp_reg_instr;

typedef enum {LSL = 0, LSR = 1, ASR = 2, ROR = 3} shift_enum;

/*
    Add & Logic data structures
 */
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

/*
  Loads and Stores structure - Single Data Transfer, Load Literal
*/

typedef enum SdtType {
    SDT,
    LL
} sdt_type;

typedef struct single_data {
  bool sdt_start;
  bool sf;
  uint8_t sdt_mid1;
  bool U;
  uint8_t sdt_mid2;
  bool L;
  int16_t offset;
  uint8_t xn;
  uint8_t rt;
} single_data;

typedef struct load_lit {
  bool ll_start;
  bool sf;
  uint8_t ll_mid1;
  int32_t simm19;
  uint8_t rt;
} load_lit;

typedef union sdt{
  single_data sdt;
  load_lit ll;
} sdt;

#endif //PROCESS_INSTR_H
