//
// Created by arthurallilaire on 08/06/24.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "opc_table.h"

#define MAXINSTRTYPES 7
opc_row opc_table[MAXINSTRTYPES];
int num_opc_types = 0; // Variable to keep track of actual number of types

void add_opc_row(opc_row *table, int index, const char **opcodes, int num_opcodes, uint32_t (*process_opc)(struct instruction)) {
    table[index].pos_opcs = (opc_str *)malloc(num_opcodes * sizeof(opc_str));
    for (int i = 0; i < num_opcodes; i++) {
        table[index].pos_opcs[i] = opcodes[i];
    }
    table[index].num_opcs = num_opcodes;
    table[index].process_opc = process_opc;
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void opc_table_gen(opc_row *opc_table, int *num_types) {
    const char *arithmetic_opcodes[] = {"add", "adds", "sub", "subs", "cmp", "cmn", "neg", "negs"};
    add_opc_row(opc_table, 0, arithmetic_opcodes, ARRAY_SIZE(arithmetic_opcodes), &arithmetic_instr);

    const char *logical_opcodes[] = {"and", "ands", "bic", "bics", "eor", "eon", "orr", "orn", "tst", "mvn", "mov"};
    add_opc_row(opc_table, 1, logical_opcodes, ARRAY_SIZE(logical_opcodes), &logical_instr);

    const char *wide_move_opcodes[] = {"movn", "movk", "movz"};
    add_opc_row(opc_table, 2, wide_move_opcodes, ARRAY_SIZE(wide_move_opcodes), &wide_move_instr);

    const char *multiply_opcodes[] = {"madd", "msub", "mul", "mneg"};
    add_opc_row(opc_table, 3, multiply_opcodes, ARRAY_SIZE(multiply_opcodes), &multiply_instr);

    const char *branch_opcodes[] = {"b", "br", "b.cond"};
    add_opc_row(opc_table, 4, branch_opcodes, ARRAY_SIZE(branch_opcodes), &branch_instr);

    const char *sdt_opcodes[] = {"ldr", "str"};
    add_opc_row(opc_table, 5, sdt_opcodes, ARRAY_SIZE(sdt_opcodes), &sdt_instr);

    const char *directive_opcodes[] = {".int"};
    add_opc_row(opc_table, 6, directive_opcodes, ARRAY_SIZE(directive_opcodes), &directive_instr);

    *num_types = MAXINSTRTYPES; // Set the number of actual types initialized
}

opcode get_opcode(const char *opc, opc_row *opc_table, int num_types) {
    for (int i = 0; i < num_types; i++) {
        for (int j = 0; j < opc_table[i].num_opcs; j++) {
            if (strcmp(opc_table[i].pos_opcs[j], opc) == 0) {
                return (opcode){.opc = opc, .process_opc = opc_table[i].process_opc};
            }
        }
    }
    printf("Opcode: %s not found\n", opc);
    exit(EXIT_FAILURE);
}

void free_opc_table(opc_row *opc_table, int num_types) {
    for (int i = 0; i < num_types; i++) {
        free(opc_table[i].pos_opcs);
    }
}

int main(void) {
    opc_table_gen(opc_table, &num_opc_types);

    const char *op_str1 = "add";
    opcode op1 = get_opcode(op_str1, opc_table, num_opc_types);
    printf("Opcode: %s\n", op1.opc);

    const char *op_str2 = "movn";
    opcode op2 = get_opcode(op_str2, opc_table, num_opc_types);
    printf("Opcode: %s\n", op2.opc);

    free_opc_table(opc_table, num_opc_types);
    return EXIT_SUCCESS;
}

