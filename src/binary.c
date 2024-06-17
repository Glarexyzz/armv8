//
// Created by arthurallilaire on 15/06/24.
//

#include "process_instr.h"
#include "binary.h"

//  Assumes well formatted dp reg
uint32_t dp_reg_to_binary(dp_reg_instr instr){
    return (instr.sf << 31) |
      (instr.opc << 29) |
      (instr.M << 28) |
      (instr.op0 << 25) |
      (instr.opr << 21) |
      (instr.rm << 16) |
      (instr.operand << 10) |
      (instr.rn << 5) |
      (instr.rd << 0);
}
uint32_t dp_imm_to_binary(dp_imm_instr instr){
    return (instr.sf << 31) |
        (instr.opc << 29) |
        (instr.op0 << 26) |
        (instr.opi << 23) |
        (instr.operand << 5) |
        (instr.rd << 0);
}

uint32_t arith_instr_to_binary(arith_instr instr){
    //  Convert to dp_imm or dp_reg then use those helper functions
    if (instr.imm) {
        dp_imm_instr dp_instr;
        dp_instr.sf = instr.sf;
        dp_instr.opc = (instr.sub << 1) | (instr.flags << 0); // 11 for subs
        dp_instr.opi = 2; //A = 010, WM = 101
        dp_instr.operand = (instr.operand.val.imm.lsl12 << 17) |
                           (instr.operand.val.imm.imm12 << 5) |
                           (instr.rn << 0);
        dp_instr.op0 = 4; // 100
        return dp_imm_to_binary(dp_instr);
    }
    //  Convert to dp_reg
    dp_reg_instr dp_instr;
    dp_instr.sf = instr.sf;
    dp_instr.opc = (instr.sub << 1) | (instr.flags << 0); // 11 for subs
    dp_instr.M = false; //0 for arithmetic
    dp_instr.op0 = 5; //Always = 101
    dp_instr.opr = 8 | (instr.operand.val.reg.shift_type << 1); // 1xx0 | SHIFT for Arithmetic
    dp_instr.rm = instr.operand.val.reg.rm;
    dp_instr.operand = instr.operand.val.reg.shift_amount;
    dp_instr.rn = instr.rn;
    dp_instr.rd = instr.rd;
    return dp_reg_to_binary(dp_instr);
}


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