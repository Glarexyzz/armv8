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
