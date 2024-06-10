//
// Created by arthurallilaire on 08/06/24.
//

#ifndef OPC_TABLE_H
#define OPC_TABLE_H

#include <stdint.h>
#include "parse_file.h"
#include "process_instr.h"

struct instruction;
typedef const char *opc_str;
typedef struct {
    opc_str opc;
    uint32_t (*process_opc)(struct instruction);
} opcode;
opcode get_opcode(const char *opc);
typedef struct {
    opcode opc;
    char rest[MAXLINELEN];
} instruction;

typedef struct {
    opc_str *pos_opcs; // Dynamically allocated array of opcode strings
    int num_opcs; // Number of opcodes
    uint32_t (*process_opc)(struct instruction);
} opc_row;
#endif //OPC_TABLE_H
