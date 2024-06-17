// Standard library header files
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define macros: constants for memory, halt instruction, number of registers,
// bit masks and byte/instruction sizes
#define MEMORY_SIZE (2 * 1024 * 1024) // 2MB of Memory
#define HALT_INSTRUCTION 0x8a000000   // Halt instruction
#define NO_REGISTERS 31               // Number of registers
#define ZR_SP 31      // Zero or Stack Registers - when register value is 11111
#define INSTR_BYTES 4 // Number of bytes per instruction
#define BYTE_SIZE 8   // The size of a byte - 8 bits
#define MASK32 0xFFFFFFFFLL // Mask bits to 32 bits
#define MASK8 0xFFLL        // Mask bits to 8 bits

// Prototypes for functions - code structure
extern void binaryFileLoad(const char *inputFileName);
extern uint32_t fetch();
extern void decode(uint32_t instruction);
extern void executeDataProcessImmediate(uint32_t instruction);
extern void executeDataProcessRegister(uint32_t instruction);
extern void executeLoadsAndStores(uint32_t instruction);
extern void executeBranches(uint32_t instruction);
extern int64_t add(int64_t int1, int64_t int2, bool sf, bool setFlags);
extern int64_t sub(int64_t int1, int64_t int2, bool sf, bool setFlags);
extern int64_t loadMemory(int32_t targetAddress, bool sf);
extern void storeMemory(int32_t targetAddress, uint64_t value, bool sf);
extern uint32_t extractBits(uint32_t n, int start, int end);
extern int32_t signExtend(int32_t value, int nbits);
extern void printState(const char *outputFileName);

/*
    1.1 An Armv8 Emulator
        - define structure for the ARMv8 machine state
        - initialize memory, registers and flags
*/

// Define structure for the ARMv8 machine state
typedef struct ARMv8_State {
  int64_t R[NO_REGISTERS]; // General Purpose Registers
  uint64_t ZR;             // Zero Register
  uint64_t PC;             // Program Counter
  uint64_t SP;             // Stack Pointer
  struct PSTATE            // Processor State
  {
    bool N;
    bool Z;
    bool C;
    bool V;
  } pstate;
} ARMv8_State;