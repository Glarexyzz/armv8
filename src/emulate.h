// Standard library header files
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
extern void binaryFileLoad(const char *inputFileName);
extern uint32_t fetch();
extern void decode(uint32_t instruction);
extern void executeDataProcessImmediate(uint32_t instruction);
extern void executeDataProcessRegister(uint32_t instruction);
extern void executeLoadsAndStores(uint32_t instruction);
extern void executeBranches(uint32_t instruction);
extern int processDataRegisterHelper(int instruction, int op1, int op2);
extern uint32_t extractBits(uint32_t n, int start, int end);
extern int64_t signExtend(int32_t value);
extern void printState(const char *outputFileName);

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