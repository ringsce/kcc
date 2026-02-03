// include/sh4_instruction_set.h
// SH4 Instruction Set Definitions

#ifndef SH4_INSTRUCTION_SET_H
#define SH4_INSTRUCTION_SET_H

#include "sh4_registers.h"

// SH-4 Instruction Opcodes
typedef enum {
    // Data Transfer
    SH4_OP_MOV,      // Move register
    SH4_OP_MOVI,     // Move immediate
    SH4_OP_MOVL,     // Move long
    SH4_OP_MOVW,     // Move word
    SH4_OP_MOVB,     // Move byte

    // Arithmetic
    SH4_OP_ADD,      // Add
    SH4_OP_ADDI,     // Add immediate
    SH4_OP_ADDC,     // Add with carry
    SH4_OP_SUB,      // Subtract
    SH4_OP_SUBC,     // Subtract with carry
    SH4_OP_MUL,      // Multiply
    SH4_OP_DMULS,    // Double multiply signed
    SH4_OP_DMULU,    // Double multiply unsigned

    // Logic
    SH4_OP_AND,      // Logical AND
    SH4_OP_OR,       // Logical OR
    SH4_OP_XOR,      // Logical XOR
    SH4_OP_NOT,      // Logical NOT
    SH4_OP_NEG,      // Negate

    // Shift
    SH4_OP_SHLL,     // Shift left logical
    SH4_OP_SHLR,     // Shift right logical
    SH4_OP_SHAR,     // Shift right arithmetic
    SH4_OP_ROTL,     // Rotate left
    SH4_OP_ROTR,     // Rotate right

    // Compare
    SH4_OP_CMP,      // Compare
    SH4_OP_TST,      // Test

    // Branch
    SH4_OP_BRA,      // Branch always
    SH4_OP_BT,       // Branch if true
    SH4_OP_BF,       // Branch if false
    SH4_OP_BSR,      // Branch to subroutine

    // Jump
    SH4_OP_JMP,      // Jump
    SH4_OP_JSR,      // Jump to subroutine
    SH4_OP_RTS,      // Return from subroutine

    // System
    SH4_OP_NOP,      // No operation
    SH4_OP_SLEEP,    // Sleep

    // Floating Point (single-precision)
    SH4_OP_FADD,     // Float add
    SH4_OP_FSUB,     // Float subtract
    SH4_OP_FMUL,     // Float multiply
    SH4_OP_FDIV,     // Float divide
    SH4_OP_FSQRT,    // Float square root
    SH4_OP_FCMP,     // Float compare
    SH4_OP_FMOV,     // Float move

    SH4_OP_COUNT
} SH4Opcode;

// Instruction information
typedef struct {
    SH4Opcode opcode;
    const char* mnemonic;
    int operand_count;
} SH4InstructionInfo;

// Instruction properties
const SH4InstructionInfo* sh4_get_instruction_info(SH4Opcode opcode);

#endif // SH4_INSTRUCTION_SET_H