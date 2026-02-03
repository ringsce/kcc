// include/sh4_register_allocator.h
// SH4 Register Allocator

#ifndef SH4_REGISTER_ALLOCATOR_H
#define SH4_REGISTER_ALLOCATOR_H

#include "sh4_registers.h"

// Register allocator structure
typedef struct {
    int reg_used[SH4_REG_COUNT];      // Track which registers are in use
    int saved_regs[SH4_REG_COUNT];    // Track which callee-saved regs need saving
    int spill_count;                   // Number of spilled registers
} SH4RegisterAllocator;

// Initialize and cleanup
void sh4_regalloc_init(SH4RegisterAllocator* alloc);
void sh4_regalloc_cleanup(SH4RegisterAllocator* alloc);

// Allocation functions
int sh4_regalloc_allocate(SH4RegisterAllocator* alloc);
void sh4_regalloc_free(SH4RegisterAllocator* alloc, int reg);
int sh4_regalloc_is_allocated(SH4RegisterAllocator* alloc, int reg);

// Reset allocator
void sh4_regalloc_reset(SH4RegisterAllocator* alloc);

#endif // SH4_REGISTER_ALLOCATOR_H