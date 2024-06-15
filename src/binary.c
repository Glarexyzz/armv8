//
// Created by arthurallilaire on 15/06/24.
//

#include "process_instr.h"
#include "binary.h"


// Converts a single data transfer instruction into a binary number
uint32_t sdt_to_binary(sdt instr, sdt_type type){
    switch(type) {
        case SDT:
            return (instr.sdt.sdt_start << 31) |
                (instr.sdt.sf << 30) |
                (instr.sdt.sdt_mid1 << 25) |
                (instr.sdt.U << 24) |
                (instr.sdt.sdt_mid2 << 23) |
                (instr.sdt.L << 22) |
                (instr.sdt.offset << 10) |
                (instr.sdt.xn << 5) |
                instr.sdt.rt;
        break;

        case LL:
            return (instr.ll.ll_start << 31) |
                (instr.ll.sf << 30) |
                (instr.ll.ll_mid1 << 24) |
                (instr.ll.simm19 << 5) |
                instr.ll.rt;
        break;

        default:
            return EXIT_FAILURE;
    }
}