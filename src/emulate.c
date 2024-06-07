#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Define constants for memory, halt instruction
#define MEMORY_SIZE (2 * 1024 * 1024) // 2MB of memory
#define HALT_INSTRUCTION 0x8a000000   // Halt instruction
#define NO_REGISTERS 31
#define ZR_SP 31     // 11111

#define INSTR_BYTES 4
#define BYTE_SIZE 8
#define MASK32 0xFFFFFFFFLL
#define MASK8 0xFFLL


// Prototypes for functions - code structure
void binaryFileLoad(const char *inputFileName);
uint32_t fetch();
void decode(uint32_t instruction);
void executeDataProcessImmediate(uint32_t instruction);
void executeDataProcessRegister(uint32_t instruction);
void executeLoadsAndStores(uint32_t instruction);
void executeBranches(uint32_t instruction);
int64_t add(int64_t int1, int64_t int2, bool sf, bool setFlags);
int64_t sub(int64_t int1, int64_t int2, bool sf, bool setFlags);
int64_t loadMemory(int32_t targetAddress, bool sf);
void storeMemory(int32_t targetAddress, uint64_t value, bool sf);
uint32_t extractBits(uint32_t n, int start, int end);
int32_t signExtend(int32_t value, int nbits);
void printState(const char *outputFileName);

/*
    1.1 An Armv8 Emulator
        - define structure for the ARMv8 machine state
        - initialize memory, registers and flags
*/

// Define structure for the ARMv8 machine state
uint8_t memory[MEMORY_SIZE] = {0}; // Emulated memory
typedef struct ARMv8_State
{
    int64_t R[NO_REGISTERS]; // General Purpose Registers
    uint64_t ZR;   // Zero Register
    uint64_t PC;   // Program Counter
    uint64_t SP;   // Stack Pointer
    struct PSTATE  // Processor State
    {
        bool N;
        bool Z;
        bool C;
        bool V;
    } pstate;
} ARMv8_State;

// Initialize memory, registers and flags
ARMv8_State state = {
    .R = {0}, .ZR = 0, .PC = 0, .SP = 0, .pstate = {false, true, false, false}};

/*
    Load binary file
*/
void binaryFileLoad(const char *inputFileName)
{
    // Open input file and read words into memory
    FILE *inputFile = fopen(inputFileName, "rb");
    if (inputFile == NULL)
    {
        fprintf(stderr, "Error: Can't open %s\n", inputFileName);
        exit(1);
    }
    fread(memory, sizeof(uint32_t), MEMORY_SIZE, inputFile);
    fclose(inputFile);
}

/*
    1.2 Execution Pipeline
        1. Fetch
        2. Decode
        3. Execute
*/

/*
    1.2.1 Fetch instruction
*/
uint32_t fetch()
{
    // Get bytes from address in little-endian style
    return (((uint32_t)memory[state.PC + 3]) << 24) |
           (((uint32_t)memory[state.PC + 2]) << 16) |
           (((uint32_t)memory[state.PC + 1]) << 8) |
           ((uint32_t)memory[state.PC + 0]);
}

/*
    1.2.2 Decode instruction
*/
void decode(uint32_t instruction)
{
    // Check op0 to determine instruction type
    uint8_t op0 = extractBits(instruction, 25, 28);

    // 1.3 A64: The AArch64 Instruction Set
    switch (op0)
    {
    // 1.4 Data Processing (Immediate)
    case 8:
    case 9:
        executeDataProcessImmediate(instruction);
        break;
    // 1.5 Data Processing (Register)
    case 5:
    case 13:
        executeDataProcessRegister(instruction);
        break;
    // 1.7 Loads and Stores
    case 4:
    case 6:
    case 12:
    case 14:
        executeLoadsAndStores(instruction);
        break;
    // 1.8 Branches
    case 10:
    case 11:
        executeBranches(instruction);
        break;
    }
}

/*
    1.2.3 Execute instruction:
      - Data Processing Instruction (Immediate)
      - Data Processing Instruction (Register)
      - Single Data Transfer Instructions (Single Data Transfer & Load Literal)
      - Branch Instructions (Unconditional & Register & Conditional)
*/

// 1.4 Data Processing Instruction (Immediate)
void executeDataProcessImmediate(uint32_t instruction)
{
    bool sf = extractBits(instruction, 31, 31);  // sf
    uint8_t opc = extractBits(instruction, 29, 30);  // opc
    uint8_t opi = extractBits(instruction, 23, 25);  // opi
    uint8_t rd = extractBits(instruction, 0, 4);  // rd

    switch (opi)
    {

    // Arithmetic instruction
    case 2:  // 010
    {
        bool sh = extractBits(instruction, 22, 22);  // sh
        uint32_t imm12 = extractBits(instruction, 10, 21);  // imm12
        uint8_t rn = extractBits(instruction, 5, 9);  // rn

        imm12 = (sh == 1) ? imm12 << 12 : imm12;  // possible shift

        int64_t result;

        switch (opc) {
            case 0: // add - add
                result = (rn != ZR_SP) ? add(state.R[rn], imm12, sf, false) : (int64_t)imm12;
                break;
            case 1: // adds - add and set flags
                result = (rn != ZR_SP) ? add(state.R[rn], imm12, sf, true) : add(0, imm12, sf, true);
                break;
            case 2: // sub - subtract
                result = (rn != ZR_SP) ? sub(state.R[rn], imm12, sf, false) : -(int64_t)imm12;
                break;
            case 3: // subs - subtract and set flags
                result = (rn != ZR_SP) ? sub(state.R[rn], imm12, sf, true) : sub(0, imm12, sf, true);
                break;
        }

        if(rd != ZR_SP) {
            state.R[rd] = sf ? result : (result & MASK32);
        }
        
        break;
    }

    // Wide move instruction
    case 5:  // 101
    {
        uint8_t hw = extractBits(instruction, 21, 22);  // hw
        uint32_t imm16 = extractBits(instruction, 5, 20);  // imm16
        uint64_t value = (uint64_t)imm16 << (hw * 16);  // value

        uint64_t mask = (sf == 0) ? MASK32 : ~0ULL;  // mask for 32 or 64 bits

        switch (opc) {
            case 0:  // movn - move wide with NOT
                state.R[rd] = ~value & mask;
                break;
            case 2:  // movz - move wide with zero
                state.R[rd] = value & mask;
                break;
            case 3:  // movk - move wide with keep
                state.R[rd] = (state.R[rd] & ~(0xFFFFULL << (hw * 16))) | value;
                state.R[rd] &= mask;
                break;
        }
        break;
    }
    }

    state.PC += INSTR_BYTES;
}

// 1.5 Data Processing Instruction (Register)
void executeDataProcessRegister(uint32_t instruction)
{
    bool sf = extractBits(instruction, 31, 31);  // sf
    uint8_t opc = extractBits(instruction, 29, 30);  // opc
    bool M = extractBits(instruction, 28, 28);  // M
    uint8_t opr = extractBits(instruction, 21, 24);  // opr
    uint8_t rm = extractBits(instruction, 16, 20);  // rm
    uint8_t operand = extractBits(instruction, 10, 15);  // operand
    uint8_t rn = extractBits(instruction, 5, 9);  // rn
    uint8_t rd = extractBits(instruction, 0, 4);  // rd

    int64_t op1 = (rn != ZR_SP) ? state.R[rn] : state.ZR; 
    int64_t op2 = (rm != ZR_SP) ? state.R[rm] : state.ZR; 
    int64_t result; 

    // Arithmetic and Bit-Logic
    if (M == 0) {

        // Calculate shift - 1.6 Bitwise Shifts
        uint8_t shift = extractBits(instruction, 22, 23);

        switch (shift) {
            case 0: // lsl (left shift)
                op2 = sf ? op2 << operand : (int32_t)op2 << operand;
                break;
            case 1: // lsr (logical right shift)
                op2 = sf ? (uint64_t)op2 >> operand : (uint32_t)op2 >> operand;
                break;
            case 2: // asr (arithmetic right shift)
                op2 = sf ? op2 >> operand : (int32_t)op2 >> operand;
                break;
            case 3: // ror (rotate right)
                if (opr < 8) {
                    op2 = sf ? (((uint64_t)op2 >> operand) | ((uint64_t)op2 << (64 - operand))) 
                                : (((uint32_t)op2 >> operand) | (uint32_t)op2 << (32 - operand));
                }
                break;
        }

        // Arithmetic
        if (opr >= 8 && opr % 2 == 0)
        {
            switch (opc)
            {
            case 0:  // add
                result = sf ? add(op1, op2, sf, false) : (int32_t)add(op1, op2, sf, false);
                break;
            case 1:  // adds
                result = sf ? add(op1, op2, sf, true) : (int32_t)add(op1, op2, sf, true);
                break;
            case 2:  // sub
                result = sf ? sub(op1, op2, sf, false) : (int32_t)sub(op1, op2, sf, false);
                break;
            case 3:  // subs
                result = sf ? sub(op1, op2, sf, true) : (int32_t)sub(op1, op2, sf, true);
                break;
            }
        }

        // Bit-Logic
        else if (opr < 8)
        {
            bool N = extractBits(instruction, 21, 21);  // N

            switch (opc)
            {
            case 0:  // bitwise bit clear | bitwise and
                result = N ? op1 & ~op2 : op1 & op2;
                break;
            case 1:  // bitwise inclusive or not | bitwise inclusive or
                result = N ? op1 | ~op2 : op1 | op2;
                break;
            case 2:  // bitwise exclusive or not | bitwise exclusive or
                result = N ? op1 ^ ~op2 : op1 ^ op2;
                break;
            case 3:  // bitwise bit clear, set flags| bitwise and, set flags
                result = N ? op1 & ~op2 : op1 & op2;
                state.pstate.N = sf ? result < 0 : ((int32_t)result) < 0;
                state.pstate.Z = (result == 0);
                state.pstate.C = 0;
                state.pstate.V = 0;
                break;
            }
        }
    }

    // Multiply
    else if (M == 1 && opr == 8) 
    {
        uint8_t ra = extractBits(instruction, 10, 14);  // ra
        bool x = extractBits(instruction, 15, 15);  // x

        int64_t Ra = (ra != ZR_SP) ? state.R[ra] : state.ZR;
        
        if(sf == 0)  // 32-bit
        {
            if (x == 0) { // madd - Multiply-Add
                result = (int32_t)add(Ra, op1 * op2, sf, false);
            } else { // msub - Multiply-Sub
                result = (int32_t)sub(Ra, op1 * op2, sf, false);
            }
        }
        else  // 64-bit
        {
            if (x == 0) { // madd - Multiply-Add
                result = add(Ra, op1 * op2, sf, false);
            } else { // msub - Multiply-Sub
                 result = sub(Ra, op1 * op2, sf, false);
            }
        }
    }

    state.R[rd] = sf ? result : (result & MASK32);

    state.PC += INSTR_BYTES;
}

// 1.7 Single Data Transfer Instructions (Single Data Transfer & Load Literal)
void executeLoadsAndStores(uint32_t instruction)
{
    bool sf = extractBits(instruction, 30, 30);  // sf
    uint8_t rt = extractBits(instruction, 0, 4);  // rt

    uint32_t targetAddress;  // target address

    // Single Data Transfer
    if (extractBits(instruction, 23, 23) == 0 && extractBits(instruction, 25, 29) == 28 && extractBits(instruction, 31, 31) == 1)
    {
        bool U = extractBits(instruction, 24, 24);  // U
        bool L = extractBits(instruction, 22, 22);  // L
        int16_t offset = extractBits(instruction, 10, 21);  // offset
        uint8_t xn = extractBits(instruction, 5, 9);  // xn
        uint8_t index = extractBits(instruction, 10, 11);  // index
        uint8_t xm = extractBits(instruction, 16, 20);  // xm
        int32_t simm9 = extractBits(instruction, 12, 20);
        simm9 = signExtend(simm9, 9);  // simm9

        // Unsigned Immediate Offset
        if (U == 1)
        {
            targetAddress = state.R[xn] + (uint16_t)offset * (sf ? 8 : 4);
        }
        else
        {
            switch (index)
            {
            case 3: // Pre-Indexed
                targetAddress = state.R[xn] + simm9;
                state.R[xn] = (int64_t)targetAddress;
                break;
            case 1:  // Post-Indexed
                targetAddress = state.R[xn];
                state.R[xn] += (int64_t)simm9;
                break;
            case 2:  // Register Offset
                targetAddress = state.R[xn] + state.R[xm];
                break;
            }
        }
        // Load Operation
        if (L == 1)
        {
            state.R[rt] = sf ? loadMemory(targetAddress, sf) : (int32_t)loadMemory(targetAddress, sf);
        }
        // Store Operation
        else
        {
            if (sf == 0) {
                storeMemory(targetAddress, (uint32_t)state.R[rt], sf);
            } else {
                storeMemory(targetAddress, state.R[rt], sf);
            }
        }
    }

    // Load Literal
    if (extractBits(instruction, 24, 29) == 24 && extractBits(instruction, 31, 31) == 0)
    {
        int32_t simm19 = extractBits(instruction, 5, 23);
        simm19 = signExtend(simm19, 19);  // simm19

        targetAddress = state.PC + simm19 * 4;

        state.R[rt] = sf ? loadMemory(targetAddress, sf) : (int32_t)loadMemory(targetAddress, sf);
    }

    state.PC += INSTR_BYTES;
}

// 1.8 Branch Instructions (Unconditional & Register & Conditional)
void executeBranches(uint32_t instruction)
{
    // Unconditional branch
    if (extractBits(instruction, 26, 31) == 5)
    {
        int32_t simm26 = extractBits(instruction, 0, 25);
        simm26 = signExtend(simm26, 26);  // simm26
        int64_t offset = (int64_t)(simm26) * 4;   // offset

        state.PC += offset;
    }

    // Register branch
    else if (extractBits(instruction, 0, 4) == 0 && extractBits(instruction, 10, 31) == 3508160)
    {
        uint8_t xn = extractBits(instruction, 5, 9); // xn

        state.PC = (xn != ZR_SP) ? state.R[xn] : state.ZR;
    }

    // Conditional branch
    else if (extractBits(instruction, 4, 4) == 0 && extractBits(instruction, 24, 31) == 84)
    {
        int32_t simm19 = extractBits(instruction, 5, 23);
        simm19 = signExtend(simm19, 19);  // simm19
        int64_t offset = (int64_t)(simm19) * 4;  // offset
        uint8_t cond = extractBits(instruction, 0, 3);  // cond

        bool condition = false;

        switch (cond) // Encoding
        {
            case 0: condition = state.pstate.Z == 1; break;  // 0000 - EQ
            case 1: condition = state.pstate.Z == 0; break;  // 0001 - NE  
            case 10: condition = state.pstate.N == state.pstate.V; break;  // 1010 - GE
            case 11: condition = state.pstate.N != state.pstate.V; break;  // 1011 - LT
            case 12: condition = state.pstate.Z == 0 && state.pstate.N == state.pstate.V; break;  // 1100 - GT
            case 13: condition = !(state.pstate.Z == 0 && state.pstate.N == state.pstate.V); break;  // 1101 - LE
            case 14: condition = true; break;  // 1110 - AL
        }
        state.PC += (condition == true) ? offset : INSTR_BYTES;
    }
}

/*
    Helper functions and bit operations
*/

// Add function - with possible set flags
int64_t add(int64_t int1, int64_t int2, bool sf, bool setFlags)
{
    int64_t output = int1 + int2;
    if (setFlags)
    {
        state.pstate.N = sf ? output < 0 : (int32_t)output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = sf ? (uint64_t)int1 + (uint64_t)int2 > UINT64_MAX
                            : (uint32_t)int1 > UINT32_MAX - (uint32_t)int2;
        state.pstate.V = sf ? ((int1 > 0) && (int2 > 0) && (output < 0)) || ((int1 < 0) && (int2 < 0) && (output > 0))
                            : (((int32_t)int1 > 0) && ((int32_t)int2 > 0) && ((int32_t)output < 0)) || (((int32_t)int1 < 0) && ((int32_t)int2 < 0) && ((int32_t)output > 0));
    }
    return output;
}
// Sub function - with possible set flags
int64_t sub(int64_t int1, int64_t int2, bool sf, bool setFlags)
{
    int64_t output = int1 - int2;
    if (setFlags)
    {
        state.pstate.N = sf ? output < 0 : (int32_t)output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = sf ? (uint64_t)int1 >= (uint64_t)int2
                            : (uint32_t)int1 >= (uint32_t)int2;
        state.pstate.V = sf ? ((int1 > 0) && (int2 < 0) && (output < 0)) || ((int1 < 0) && (int2 > 0) && (output > 0))
                            : (((int32_t)int1 > 0) && ((int32_t)int2 < 0) && ((int32_t)output < 0)) || (((int32_t)int1 < 0) && ((int32_t)int2 > 0) && ((int32_t)output > 0));
    }
    return output;
}

// Load memory into register
int64_t loadMemory(int32_t targetAddress, bool sf) {
    int64_t value = 0;
    int bytes = sf ? 8 : 4;
    for(int i = 0; i < bytes; i++) {
        value |= sf ? (uint64_t)memory[targetAddress + i] << (BYTE_SIZE * i)
                    : (uint32_t)memory[targetAddress + i] << (BYTE_SIZE * i);
    }
    return value;
}

// Store register into memory
void storeMemory(int32_t targetAddress, uint64_t value, bool sf) {
    int bytes = sf ? 8 : 4;
    for(int i = 0; i < bytes; i++) {
        memory[targetAddress + i] = (value >> (BYTE_SIZE * i)) & MASK8;
    }
}

// Extract bits from instruction
uint32_t extractBits(uint32_t value, int start, int end) {
    uint32_t mask = (1 << (end - start + 1)) - 1;
    return (value >> start) & mask;
}

// Sign-extend to 32 bits
int32_t signExtend(int32_t value, int nbits)
{
    if (value & (1 << (nbits - 1)))
    {
        // The number is negative
        int mask = MASK32 ^ ((1 << nbits) - 1);
        value |= mask;
    }
    return value;
}

/*
    Print the state of the machine
*/
void printState(const char *outputFileName)
{
    // Determine output file / stdout
    if (outputFileName)
    {
        const char *dot = strrchr(outputFileName, '.');
        if (dot && strcmp(dot, ".out") != 0) // check to be of the form "__.out"
        {
            printf("Output file should be of type .out.\n");
            exit(0);
        }
    }

    FILE *outputDest = outputFileName ? fopen(outputFileName, "wb") : stdout;
    if (outputDest == NULL)
    {
        fprintf(stderr, "Error: Can't open %s\n", outputFileName);
        exit(1);
    }

    // Print - General purpose registers
    fprintf(outputDest, "Registers:\n");
    for (int i = 0; i < NO_REGISTERS; i++)
    {
        fprintf(outputDest, "X%02d = %016lx\n", i, state.R[i]);
    }

    // Print - Program Counter (PC)
    fprintf(outputDest, "PC = %016lx\n", state.PC);

    // Print - Processor State (PSTATE)
    fprintf(outputDest, "PSTATE : ");
    fprintf(outputDest, "%c", state.pstate.N ? 'N' : '-');
    fprintf(outputDest, "%c", state.pstate.Z ? 'Z' : '-');
    fprintf(outputDest, "%c", state.pstate.C ? 'C' : '-');
    fprintf(outputDest, "%c\n", state.pstate.V ? 'V' : '-');

    // Print - Non-Zero Memory
    fprintf(outputDest, "Non-zero Memory:\n");
    for (int i = 0; i < MEMORY_SIZE; i += INSTR_BYTES)
    {
        uint32_t value = *((uint32_t *)(memory + i));
        if (value != 0)
        {
            fprintf(outputDest, "0x%08x : %08x\n", i, value);
        }
    }

    // Close output file
    if (outputDest != stdout)
    {
        fclose(outputDest);
    }
}

/*
    Main function
*/
int main(int argc, char **argv)
{
    /*
        1 The Emulator
            - read in binary object code from a binary file
            - support an optional output file
            - object code: 32-bit words (representing machine instruction / data)
    */

    // Check for correct number of arguments
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file_in> [file_out]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Define input and output files
    const char *inputFileName = argv[1];
    const char *outputFileName = (argc > 2) ? argv[2] : NULL;

    // Load binary file
    binaryFileLoad(inputFileName);

    // Execution pipeline - fetch, decode and execute instructions
    uint32_t instruction = fetch();
    while (instruction != HALT_INSTRUCTION) // 1.9 Special Instructions - Halting
    {
        decode(instruction);
        instruction = fetch();
    }

    // Print state after HALT_INSTRUCTION
    printState(outputFileName);

    return 0;
}