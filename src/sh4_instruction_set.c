// src/sh4_instruction_set.c
// SH4 Instruction Set Implementation

#include "sh4_instruction_set.h"
#include <string.h>

// Get register name
const char* sh4_register_name(int reg) {
    static const char* reg_names[] = {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    };

    if (reg >= 0 && reg < SH4_REG_COUNT) {
        return reg_names[reg];
    }
    return "unknown";
}

// Check if register is callee-saved
int sh4_is_callee_saved(int reg) {
    return (reg >= SH4_CALLEE_SAVED_START && reg <= SH4_CALLEE_SAVED_END);
}

// Check if register is scratch (caller-saved)
int sh4_is_scratch_register(int reg) {
    return (reg >= SH4_SCRATCH_START && reg <= SH4_SCRATCH_END);
}

// Get instruction information
const SH4InstructionInfo* sh4_get_instruction_info(SH4Opcode opcode) {
    static const SH4InstructionInfo instructions[] = {
        {SH4_OP_MOV, "mov", 2},
        {SH4_OP_MOVI, "mov", 2},
        {SH4_OP_MOVL, "mov.l", 2},
        {SH4_OP_ADD, "add", 2},
        {SH4_OP_SUB, "sub", 2},
        {SH4_OP_MUL, "mul.l", 2},
        {SH4_OP_AND, "and", 2},
        {SH4_OP_OR, "or", 2},
        {SH4_OP_XOR, "xor", 2},
        {SH4_OP_NOT, "not", 2},
        {SH4_OP_NEG, "neg", 2},
        {SH4_OP_CMP, "cmp/eq", 2},
        {SH4_OP_BRA, "bra", 1},
        {SH4_OP_BT, "bt", 1},
        {SH4_OP_BF, "bf", 1},
        {SH4_OP_JSR, "jsr", 1},
        {SH4_OP_RTS, "rts", 0},
        {SH4_OP_NOP, "nop", 0},
    };

    if (opcode >= 0 && opcode < SH4_OP_COUNT) {
        return &instructions[opcode];
    }
    return NULL;
}