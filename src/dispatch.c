//
// Created by aa8123 on 10/06/24.
//
#include "dispatch.h"
#define MAXOPCLEN 7
typedef struct {
    char opcode[MAXOPCLEN + 1]; //+ null terminator
    instr_processor opc_func;
} opc_to_func;
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
// Initialize the array of opcode-function pairs
opc_to_func opc_func_pairs[] = {
        // Arithmetic opcodes
        {"add", arithmetic_instr},
        {"adds", arithmetic_instr},
        {"sub", arithmetic_instr},
        {"subs", arithmetic_instr},
        {"cmp", arithmetic_instr},
        {"cmn", arithmetic_instr},
        {"neg", arithmetic_instr},
        {"negs", arithmetic_instr},

        // Logical opcodes
        {"and", logical_instr},
        {"ands", logical_instr},
        {"bic", logical_instr},
        {"bics", logical_instr},
        {"eor", logical_instr},
        {"eon", logical_instr},
        {"orr", logical_instr},
        {"orn", logical_instr},
        {"tst", logical_instr},
        {"mvn", logical_instr},
        {"mov", logical_instr},

        // Wide move opcodes
        {"movn", wide_move_instr},
        {"movk", wide_move_instr},
        {"movz", wide_move_instr},

        // Multiply opcodes
        {"madd", multiply_instr},
        {"msub", multiply_instr},
        {"mul", multiply_instr},
        {"mneg", multiply_instr},

        // Branch opcodes
        {"b", branch_instr},
        {"br", branch_instr},
        {"b.cond", branch_instr},

        // SDT opcodes
        {"ldr", sdt_instr},
        {"str", sdt_instr},

        // Directive opcodes
        {".int", directive_instr},
};
//Return the function to process the instruction with that opcode
//Returns NULL if not a valid OPCODE - you must check!!
instr_processor get_instr_processor(char *opc_str){
    for (int i = 0; i < ARRAY_SIZE(opc_func_pairs); i++){
        if (strcmp(opc_str, opc_func_pairs[i].opcode) == 0){
            return opc_func_pairs[i].opc_func;
        }
    }
//  No opcode matches
    return NULL;
}