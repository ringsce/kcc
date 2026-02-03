// src/sh4_register_allocator.c
// SH4 Register Allocation

#include "sh4_register_allocator.h"
#include <string.h>

// Initialize register allocator
void sh4_regalloc_init(SH4RegisterAllocator* alloc) {
    if (!alloc) return;

    memset(alloc, 0, sizeof(SH4RegisterAllocator));

    // Mark special registers as unavailable
    alloc->reg_used[SH4_REG_R14] = 1; // Frame pointer
    alloc->reg_used[SH4_REG_R15] = 1; // Stack pointer
}

// Allocate a register
int sh4_regalloc_allocate(SH4RegisterAllocator* alloc) {
    if (!alloc) return -1;

    // Try to allocate scratch registers first (R0-R3)
    for (int i = SH4_SCRATCH_START; i <= SH4_SCRATCH_END; i++) {
        if (!alloc->reg_used[i]) {
            alloc->reg_used[i] = 1;
            return i;
        }
    }

    // Then try callee-saved registers (R8-R13)
    for (int i = SH4_CALLEE_SAVED_START; i <= SH4_CALLEE_SAVED_END - 1; i++) {
        if (!alloc->reg_used[i]) {
            alloc->reg_used[i] = 1;
            alloc->saved_regs[i] = 1;
            return i;
        }
    }

    // No registers available
    return -1;
}

// Free a register
void sh4_regalloc_free(SH4RegisterAllocator* alloc, int reg) {
    if (!alloc || reg < 0 || reg >= SH4_REG_COUNT) return;

    // Don't free special registers
    if (reg == SH4_REG_R14 || reg == SH4_REG_R15) return;

    alloc->reg_used[reg] = 0;
}

// Check if register is allocated
int sh4_regalloc_is_allocated(SH4RegisterAllocator* alloc, int reg) {
    if (!alloc || reg < 0 || reg >= SH4_REG_COUNT) return 0;
    return alloc->reg_used[reg];
}

// Reset allocator
void sh4_regalloc_reset(SH4RegisterAllocator* alloc) {
    sh4_regalloc_init(alloc);
}

// Cleanup allocator
void sh4_regalloc_cleanup(SH4RegisterAllocator* alloc) {
    if (!alloc) return;
    memset(alloc, 0, sizeof(SH4RegisterAllocator));
}