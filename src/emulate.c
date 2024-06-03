#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Define constants for memory, halt instruction
#define MEMORY_SIZE (2 * 1024 * 1024) // 2MB of memory
#define ZERO_REGISTER_ENCODING 31     // 11111
#define HALT_INSTRUCTION 0x8a000000   // Halt instruction

// Prototypes for functions
void binaryFileLoad(const char *inputFileName);
uint32_t fetch();
void decode(uint32_t instruction);
void executeDataProcessImmediate(uint32_t instruction);
void executeDataProcessRegister(uint32_t instruction);
void executeLoadsAndStores(uint32_t instruction);
void executeBranches(uint32_t instruction);
int processDataRegisterHelper(int instruction, int op1, int op2);
uint32_t extractBits(uint32_t n, int start, int end);
int64_t signExtend(int32_t value);
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
    int64_t R[31]; // General Purpose Registers
    uint64_t ZR;   // Zero Register
    uint64_t PC;   // Program Counter
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
    .R = {0}, .ZR = 0, .PC = 0, .pstate = {false, true, false, false}};

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
    uint32_t *bytes = (uint32_t *)(memory + state.PC);
    return (bytes[3] << 24) |
           (bytes[2] << 16) |
           (bytes[1] << 8) |
           bytes[0];
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
    bool sf = extractBits(instruction, 31, 31);
    uint8_t opc = extractBits(instruction, 29, 30);
    uint8_t opi = extractBits(instruction, 23, 25);
    uint8_t rd = extractBits(instruction, 0, 4);
    switch (opi)
    {
    // Arithmetic instruction
    case 2:
        bool sh = extractBits(instruction, 22, 22);
        uint32_t imm12 = extractBits(instruction, 10, 21);
        uint8_t rn = extractBits(instruction, 5, 9);
        if (sh == 1)
        {
            imm12 = imm12 << 12;
        }

        if (sf == 0)
        {
            int32_t result;
            switch (opc)
            {
            case 0:
                // add - add
                if (rn != 31)
                {
                    result = add32(state.R[rn], imm12, false);
                }
                else
                {
                    result = imm12;
                }
            case 1:
                // adds - add and set flags
                if (rn != 31)
                {
                    result = add32(state.R[rn], imm12, true);
                }
                else
                {
                    result = add32(0, imm12, true);
                }
                break;
            // sub - subtract
            case 2:
                if (rn != 31)
                {
                    result = sub32(state.R[rn], imm12, false);
                }
                else
                {
                    result = -imm12;
                }
                break;
            // subs - subtract and set flags
            case 3:
                if (rn != 31)
                {
                    result = sub32(state.R[rn], imm12, true);
                }
                else
                {
                    result = sub32(imm12, 0, true);
                }
                break;
            }
            // clear least significant 32 bits
            state.R[rd] &= 0xFFFFFF;
            state.R[rd] = state.R[rn] | (int64_t)result;
        }
        else
        {
            switch (opc)
            {
            case 0:
                // add - add
                if (rn != 31)
                {
                    state.R[rd] = add64(state.R[rn], imm12, false);
                }
                else
                {
                    state.R[rd] = imm12;
                }
            case 1:
                // adds - add and set flags
                if (rn != 31)
                {
                    state.R[rd] = add64(state.R[rn], imm12, true);
                }
                else
                {
                    state.R[rd] = add64(0, imm12, true);
                }
                break;
            // sub - subtract
            case 2:
                if (rn != 31)
                {
                    state.R[rd] = sub64(state.R[rn], imm12, false);
                }
                else
                {
                    state.R[rd] = -imm12;
                }
                break;
            // subs - subtract and set flags
            case 3:
                if (rn != 31)
                {
                    state.R[rd] = sub64(state.R[rn], imm12, true);
                }
                else
                {
                    state.R[rd] = sub64(imm12, 0, true);
                }
                break;
            }
        }
        break;
    // Wide move instruction
    case 5:
    {
        uint8_t hw = extractBits(instruction, 21, 22);
        uint32_t imm16 = extractBits(instruction, 5, 20);
        switch (opc)
        {
        // movn - move wide with NOT
        case 0:
            state.R[rd] = ~(imm16 << (hw * 16));
            break;
        // movz - move wide with zero
        case 2:
            state.R[rd] = ~(imm16 << (hw * 16));
            break;
        // movk - move wide with keep
        case 3:
            state.R[rd] = (state.R[rd] & ~(0xFFFF << (hw * 16))) | (imm16 << (hw * 16));
            break;
        }
        break;
    }
    }
    state.PC += 4;
}

// 1.5 Data Processing Instruction (Register)
void executeDataProcessRegister(uint32_t instruction)
{
    bool sf = extractBits(instruction, 31, 31);
    // uint8_t opc = extractBits(instruction, 29, 30);
    bool M = extractBits(instruction, 28, 28);
    uint8_t opr = extractBits(instruction, 21, 24);
    uint8_t rm = extractBits(instruction, 16, 20);
    uint8_t operand = extractBits(instruction, 10, 15);
    uint8_t rn = extractBits(instruction, 5, 9);
    uint8_t rd = extractBits(instruction, 0, 4);

    switch (M)
    {
    case 0:
    {
        uint8_t shift = extractBits(instruction, 22, 23);
        int32_t op2 = (int32_t)state.R[rm];
        if (sf == 0)
        {
            // 32 bit
            switch (shift)
            {
            case 0:
                // lsl (left shift)
                op2 = op2 << operand;
                break;
            case 1:
                // lsr (right shift)
                op2 = op2 >> operand;
                break;
            case 3:
                // asr (arithmetic right shift)
                op2 = op2 >> operand;
                break;
            case 4:
                // ror (only valid for logic instructions)
                if (opr < 8)
                {
                    int bitWidth = 32;
                    op2 = op2 >> operand | op2 << (bitWidth - operand);
                }
                break;
            }
            // clear least significant 32 bits
            state.R[rm] &= 0xFFFFFF;
            state.R[rm] = state.R[rm] | (int64_t)processDataRegisterHelper(instruction, rn, op2);
        }
        else if (sf == 1)
        {
            // 64 bit
            switch (shift)
            {
            case 0:
                // lsl (left shift)
                op2 = op2 << operand;
                break;
            case 1:
                // lsr (right shift)
                op2 = op2 >> operand;
                break;
            case 3:
                // asr (arithmetic right shift)
                op2 = op2 >> operand;
                break;
            case 4:
                // ror (only valid for logic instructions)
                if (opr < 8)
                {
                    int bitWidth = 64;
                    op2 = op2 >> operand | op2 << (bitWidth - operand);
                }
                break;
            }

            state.R[rm] = (int64_t)processDataRegisterHelper(instruction, rn, op2);
        }

        break;
    }
    case 1:
        // Multiply
        if (opr == 8)
        {
            uint8_t ra = extractBits(instruction, 10, 14);
            bool x = extractBits(instruction, 15, 15);
            if (x == 0) // madd - Multiply-Add
            {
                if (ra != 31)
                {
                    state.R[rd] = state.R[ra] + state.R[rn] * state.R[rm];
                }
                else
                {
                    state.R[rd] = state.R[rn] * state.R[rm];
                }
            }
            else // msub - Multiply-Sub
            {
                if (ra != 31)
                {
                    state.R[rd] = state.R[ra] - state.R[rn] * state.R[rm];
                }
                else
                {
                    state.R[rd] = -state.R[rn] * state.R[rm];
                }
            }
        }
        break;
    }

    state.PC += 4;
}
// Helper functions for add and sub
// FLAG DEFS
// C - result not mathematically correct when interpreted as unsigned
// V - result not mathematically correct when interpreted as signed

int32_t add32(int32_t int1, int32_t int2, bool setFlags)
{
    int32_t output;
    if (int1 > 0 && int2 > INT32_MAX - int1)
    {
        // Handle overflow
        output = -2 - abs(int1 - int2);
    }
    else
    {
        output = int1 + int2;
    }
    if (setFlags)
    {
        state.pstate.N = output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = (uint32_t)int1 > UINT32_MAX - (uint32_t)int2;
        state.pstate.V = int1 > 0 && int2 > INT32_MAX - int1;
    }
    return output;
}

int32_t sub32(int32_t int1, int32_t int2, bool setFlags)
{
    int32_t output;
    if (int1 < 0 && int2 > 0 && -int2 < INT32_MIN - int1)
    {
        // Handle underflow
        output = -(int1 + int2);
    }
    else
    {
        output = int1 - int2;
    }
    if (setFlags)
    {
        state.pstate.N = output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = (uint32_t)int1 < (uint32_t)int2;
        state.pstate.V = int1 < 0 && int2 > 0 && -int2 < INT32_MIN - int1;
    }
    return output;
}
int64_t add64(int64_t int1, int64_t int2, bool setFlags)
{
    int64_t output;
    if (int1 > 0 && int2 > INT64_MAX - int1)
    {
        // Handle overflow
        output = -2 - abs(int1 - int2);
    }
    else
    {
        output = int1 + int2;
    }
    if (setFlags)
    {
        state.pstate.N = output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = (uint64_t)int1 > UINT64_MAX - (uint64_t)int2;
        state.pstate.V = int1 > 0 && int2 > INT64_MAX - int1;
    }
    return output;
}

int64_t sub64(int64_t int1, int64_t int2, bool setFlags)
{
    int64_t output;
    if (int1 < 0 && int2 > 0 && -int2 < INT64_MIN - int1)
    {
        // Handle underflow
        output = -(int1 + int2);
    }
    else
    {
        output = int1 - int2;
    }
    if (setFlags)
    {
        state.pstate.N = output < 0;
        state.pstate.Z = output == 0;
        state.pstate.C = (uint64_t)int1 < (uint64_t)int2;
        state.pstate.V = int1 < 0 && int2 > 0 && -int2 < INT64_MIN - int1;
    }
    return output;
}

int processDataRegisterHelper(int instruction, int op1, int op2)
{
    bool sf = extractBits(instruction, 31, 31);
    uint8_t opc = extractBits(instruction, 29, 30);
    uint8_t opr = extractBits(instruction, 21, 24);
    int result;
    // Arithmetic
    if (opr >= 8 && opr % 2 == 0)
    {
        if (sf == 0)
        {
            // 32 bit
            switch (opc)
            {
            case 0:
                // add
                result = add32(op1, op2, false);
            case 1:
                // adds
                result = add32(op1, op2, true);
            case 2:
                // sub
                result = sub32(op1, op2, false);
            case 3:
                // subs
                result = sub32(op1, op2, true);
            }
        }
        else
        {
            // 64 bit
            switch (opc)
            {
            case 0:
                // add
                result = add64(op1, op2, false);
            case 1:
                // adds
                result = add64(op1, op2, true);
            case 2:
                // sub
                result = add64(op1, op2, false);
            case 3:
                // subs
                result = add64(op1, op2, true);
            }
        }
    }
    // Bit-logic
    else if (opr < 8)
    {
        bool N = extractBits(instruction, 21, 21);
        if (N == 0)
        {
            switch (opc)
            {
            case 0:
                // bitwise and
                result = op1 & op2;
                break;
            case 1:
                // bitwise inclusive or
                result = op1 | op2;
                break;
            case 2:
                // bitwise exclusive or
                result = op1 ^ op2;
                break;
            case 3:
                // bitwise and set flags
                result = op1 & op2;
                state.pstate.N = result < 0;
                state.pstate.Z = result == 0;
                state.pstate.C = 0;
                state.pstate.V = 0;
                break;
            }
        }
        else if (N == 1)
        {
            switch (opc)
            {
            case 0:
                // bitwise and
                result = op1 & ~op2;
                break;
            case 1:
                // bitwise inclusive or
                result = op1 | ~op2;
                break;
            case 2:
                // bitwise exclusive or
                result = op1 ^ ~op2;
                break;
            case 3:
                // bitwise and set flags
                result = op1 & ~op2;
                state.pstate.N = result < 0;
                state.pstate.Z = result == 0;
                state.pstate.C = 0;
                state.pstate.V = 0;
                break;
            }
        }
    }
    return result;
}

// 1.7 Single Data Transfer Instructions (Single Data Transfer & Load Literal)
void executeLoadsAndStores(uint32_t instruction)
{
    bool sf = extractBits(instruction, 30, 30);
    uint8_t rt = extractBits(instruction, 0, 4);
    int valueByte;
    uintptr_t ta;

    // Check size
    if (sf == 0)
    {
        valueByte = 4; // Size 32 bits
    }
    else
    {
        valueByte = 8; // Size 64 bits
    }

    // Single Data Transfer
    if (extractBits(instruction, 23, 23) == 0 && extractBits(instruction, 25, 29) == 0x1C && extractBits(instruction, 31, 31) == 1)
    {
        bool U = extractBits(instruction, 24, 24);
        bool L = extractBits(instruction, 22, 22);
        uint16_t offset = extractBits(instruction, 10, 21);
        uint8_t xn = extractBits(instruction, 5, 9);

        // Unsigned Immediate Offset
        if (U == 1)
        {
            ta = state.R[xn] + ((uint64_t)offset) * valueByte;
        }
        else
        {
            switch (extractBits(offset, 0, 1))
            {
            // Pre-Indexed
            case 3:
                // Target Address
                ta = state.R[xn] + extractBits(offset, 2, 9) - (extractBits(offset, 10, 10) * 256);

                // Update Xn
                state.R[xn] = ta;
                break;

            // Post-Indexed
            case 1:
                // Target Address
                ta = state.R[xn];

                // Update Xn
                state.R[xn] = ta + extractBits(offset, 2, 9) - (extractBits(offset, 10, 10) * 256);
                break;

            // Register Offset
            case 2:
            {
                uint8_t xm = extractBits(offset, 6, 10); // Xm

                // Target Address
                ta = state.R[xn] + state.R[xm];
                break;
            }
            }
        }
        // Load Operation
        if (L == 0)
        {
            int64_t newValue = 0; // Initialise the loading value

            for (int i = 0; i < valueByte; i++)
            {
                newValue += ((int64_t)(*((uint8_t *)ta + i))) << (i * 8);
            }
            // Load to Target Register
            state.R[rt] = newValue;
        }
        // Store Operation
        else
        {
            for (int i = 0; i < valueByte; i++)
            {
                // Store from Register to Target Address
                *((uint8_t *)(ta) + i) = (uint8_t)extractBits(state.R[rt], (i * 8), ((i + 1) * 8 - 1));
            }
        }
    }

    // Load Literal
    if (extractBits(instruction, 24, 29) == 0x18 && extractBits(instruction, 31, 31) == 0)
    {
        int32_t simm19 = extractBits(instruction, 5, 23);
        ta = state.PC + (signExtend(simm19) * 4);
        int64_t newValue = 0; // Initialise the loading value

        for (int i = 0; i < valueByte; i++)
        {
            // newValue += (*(ta + i)) << (i * 8);
            newValue += ((int64_t)memory[ta + i]) << (i * 8);
        }
        // Load to Target Register
        state.R[rt] = newValue;
    }

    state.PC += 4;
}

// 1.8 Branch Instructions (Unconditional & Register & Conditional)
void executeBranches(uint32_t instruction)
{
    // Unconditional branch
    if (extractBits(instruction, 26, 31) == 5)
    {
        int32_t simm26 = extractBits(instruction, 0, 25); // simm26
        int64_t offset = signExtend(simm26) * 4;          // offset

        state.PC += offset;
    }

    // Register branch
    if (extractBits(instruction, 0, 4) == 0 && extractBits(instruction, 10, 31) == 0x3587C0)
    {
        uint8_t xn = extractBits(instruction, 5, 9); // xn
        if (xn != 31)
        {
            state.PC = state.R[xn];
        }
        else
        {
            state.PC = state.ZR;
        }
    }

    // Conditional branch
    if (extractBits(instruction, 4, 4) == 0 && extractBits(instruction, 24, 31) == 0x54)
    {
        int32_t simm19 = extractBits(instruction, 5, 23); // simm19
        int64_t offset = signExtend(simm19) * 4;          // offset
        uint8_t cond = extractBits(instruction, 0, 3);    // cond
        bool condition = 0;
        switch (cond) // Encoding
        {
        case 0: // 0000
            condition = state.pstate.Z == 1;
            break;
        case 1: // 0001
            condition = state.pstate.Z == 0;
            break;
        case 10: // 1010
            condition = state.pstate.N == state.pstate.V;
            break;
        case 11: // 1011
            condition = state.pstate.N != state.pstate.V;
            break;
        case 12: // 1100
            condition = state.pstate.Z == 0 && state.pstate.N == state.pstate.V;
            break;
        case 13: // 1101
            condition = !(state.pstate.Z == 0 && state.pstate.N == state.pstate.V);
            break;
        case 14: // 1110
            condition = 1;
            break;
        }
        if (condition == 1)
        {
            state.PC += offset;
        }
        else
        {
            state.PC += 4;
        }
    }
}

// Aux function to extract bits from n[start] to n[end]
uint32_t extractBits(uint32_t n, int start, int end)
{
    int num_bits = end - start + 1;
    uint32_t mask = ((1 << num_bits) - 1) << start;
    uint32_t extracted_bits = (n & mask) >> start;
    return extracted_bits;
}

// Aux function to sign-extend to 64 bits
int64_t signExtend(int32_t value)
{
    // Find the most significant bit (MSB)
    int bitWidth = 32;
    for (int i = 31; i >= 0; --i)
    {
        if (value & (1 << i))
        {
            bitWidth = i + 1;
            break;
        }
    }
    int signBit = bitWidth - 1; // sign bit position
    if (value & (1 << signBit)) // check the sign bit
    {
        value |= ~((1 << bitWidth) - 1); // if sign bit is set, extend sign
    }

    return (int64_t)value;
}

/*
    Print the state of the machine
*/
void printState(const char *outputFileName)
{
    // Determine output file / stdout
    const char *dot = strrchr(outputFileName, '.');
    if (dot && strcmp(dot, ".out") == 0) // check to be of the form "__.out"
    {
        printf("Output file should be of type .out.");
        exit(0);
    }
    FILE *outputDest = outputFileName ? fopen(outputFileName, "wb") : stdout;
    if (outputDest == NULL)
    {
        fprintf(stderr, "Error: Can't open %s\n", outputFileName);
        exit(1);
    }

    // Print - General purpose registers
    fprintf(outputDest, "Registers:\n");
    for (int i = 0; i < 31; i++)
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

    // Print - non-zero memory
    fprintf(outputDest, "Non-zero memory:\n");
    for (int i = 0; i < MEMORY_SIZE; i += 4)
    {
        uint32_t value = *((uint32_t *)(memory + i));
        if (value != 0)
        {
            fprintf(outputDest, "0x%08x: %08x\n", i, value);
        }
    }

    // Close output file
    if (outputDest != stdout)
    {
        fclose(outputDest);
    }
    exit(0);
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
        exit(1);
    }
    // Define input and output files
    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];

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
