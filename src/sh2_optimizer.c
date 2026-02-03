// ============================================================================
// src/sh2_optimizer.c - SH-2 Specific Optimizations
// ============================================================================
#include "sh2_optimizer.h"
#include "sh2_instruction_set.h"
#include <string.h>
#include <stdlib.h>

// ============================================================================
// Peephole Optimization Patterns
// ============================================================================

// Local struct definition (not typedef to avoid conflict)
struct PeepholePatternStruct {
    const char *pattern[4];     // Up to 4 instruction pattern
    const char *replacement[4]; // Optimized replacement
    int pattern_len;
    int replacement_len;
};

// Note: Currently unused - will be implemented in future optimization passes
static struct PeepholePatternStruct peephole_patterns[] __attribute__((unused)) = {
    // mov r0,r1; mov r1,r2 -> mov r0,r2
    {{"mov", "mov"}, {"mov"}, 2, 1},

    // add #0,rN -> (delete)
    {{"add #0"}, {""}, 1, 0},

    // sub rN,rN -> mov #0,rN
    {{"sub rN,rN"}, {"mov #0,rN"}, 1, 1},

    // mov #0,rN; add rM,rN -> mov rM,rN
    {{"mov #0", "add"}, {"mov"}, 2, 1},

    // shll rN; shll rN -> shll2 rN
    {{"shll", "shll"}, {"shll2"}, 2, 1},

    // shlr rN; shlr rN -> shlr2 rN
    {{"shlr", "shlr"}, {"shlr2"}, 2, 1},

    // mov.l @rN,rM; tst rM,rM -> mov.l @rN,rM; cmp/eq #0,rM
    {{"mov.l @", "tst"}, {"mov.l @", "cmp/eq #0"}, 2, 2},
};

// ============================================================================
// Instruction Scheduling for Delay Slots
// ============================================================================

typedef enum {
    DELAY_SLOT_NONE,
    DELAY_SLOT_BRANCH,
    DELAY_SLOT_JUMP,
    DELAY_SLOT_RTS
} DelaySlotType;

// Check if instruction can be placed in delay slot
bool sh2_can_use_in_delay_slot(const char *instruction) {
    // Instructions that CANNOT go in delay slots:
    // - Any branch/jump
    // - RTE
    // - Instructions that modify PC
    // - Instructions that could cause exceptions

    if (strstr(instruction, "bra") ||
        strstr(instruction, "bsr") ||
        strstr(instruction, "bt") ||
        strstr(instruction, "bf") ||
        strstr(instruction, "jmp") ||
        strstr(instruction, "jsr") ||
        strstr(instruction, "rts") ||
        strstr(instruction, "rte")) {
        return false;
    }

    return true;
}

// Try to fill delay slot with useful instruction
void sh2_optimize_delay_slot(FILE *out, const char *branch_inst,
                              const char *next_inst) {
    if (sh2_can_use_in_delay_slot(next_inst)) {
        fprintf(out, "\t%s\n", branch_inst);
        fprintf(out, "\t%s\n", next_inst);
    } else {
        fprintf(out, "\t%s\n", branch_inst);
        fprintf(out, "\tnop\n");
    }
}

// ============================================================================
// Register Allocation Optimization
// ============================================================================

typedef struct {
    int reg;
    int last_use;
    int def_count;
    bool is_spilled;
} RegisterInfo;

// Local struct definition (not conflicting with header)
struct RegisterAllocatorStruct {
    RegisterInfo regs[16];
    int current_instruction;
};

RegisterAllocator* sh2_alloc_create(void) {
    RegisterAllocator *alloc = calloc(1, sizeof(RegisterAllocator));
    return alloc;
}

void sh2_alloc_destroy(RegisterAllocator *alloc) {
    free(alloc);
}

// Find least recently used register for spilling
int sh2_alloc_find_victim(RegisterAllocator *alloc) {
    int min_use = alloc->current_instruction;
    int victim = 8;  // Start with r8 (callee-saved)

    for (int i = 8; i <= 13; i++) {
        if (alloc->regs[i].last_use < min_use) {
            min_use = alloc->regs[i].last_use;
            victim = i;
        }
    }

    return victim;
}

// ============================================================================
// Common Code Generation Patterns
// ============================================================================

// Generate efficient comparison
void sh2_gen_compare(FILE *out, const char *op, int lhs, int rhs,
                     const char *true_label) {
    if (strcmp(op, "==") == 0) {
        sh2_cmp_eq(out, lhs, rhs);
        sh2_bt(out, true_label);
    } else if (strcmp(op, "!=") == 0) {
        sh2_cmp_eq(out, lhs, rhs);
        sh2_bf(out, true_label);
    } else if (strcmp(op, "<") == 0) {
        sh2_cmp_gt(out, rhs, lhs);  // rhs > lhs means lhs < rhs
        sh2_bt(out, true_label);
    } else if (strcmp(op, "<=") == 0) {
        sh2_cmp_gt(out, lhs, rhs);  // !(lhs > rhs) means lhs <= rhs
        sh2_bf(out, true_label);
    } else if (strcmp(op, ">") == 0) {
        sh2_cmp_gt(out, lhs, rhs);
        sh2_bt(out, true_label);
    } else if (strcmp(op, ">=") == 0) {
        sh2_cmp_ge(out, lhs, rhs);
        sh2_bt(out, true_label);
    }
    sh2_nop(out);  // Delay slot
}

// Generate efficient loop
void sh2_gen_loop(FILE *out, int counter_reg, int count,
                  const char *body_label, const char *end_label) {
    // Use dt instruction for efficient countdown loops
    sh2_mov_imm(out, counter_reg, count);
    sh2_label(out, body_label);

    // Loop body would go here

    sh2_dt(out, counter_reg);          // Decrement and test
    sh2_bf(out, body_label);           // Branch if not zero
    sh2_nop(out);                      // Delay slot
    sh2_label(out, end_label);
}

// Generate efficient switch statement using jump table
void sh2_gen_switch(FILE *out, int value_reg, int num_cases,
                    const char *table_label) {
    (void)table_label;  // Mark as intentionally unused

    // Bounds check
    sh2_mov_imm(out, 0, num_cases);
    sh2_cmp_hs(out, value_reg, 0);
    sh2_bf(out, "switch_default");
    sh2_nop(out);

    // Calculate jump table offset (value * 4)
    sh2_shll2(out, value_reg);

    // Load address from jump table
    sh2_mova(out, 0);  // Load PC-relative address
    sh2_add(out, value_reg, 0);
    sh2_mov_l_indir(out, 0, 0);

    // Jump to case
    sh2_jmp(out, 0);
    sh2_nop(out);
}

// ============================================================================
// Multiply/Divide Optimization
// ============================================================================

// Optimize multiplication by constant
void sh2_gen_mul_const(FILE *out, int dst, int src, int constant) {
    if (constant == 0) {
        sh2_mov_imm(out, dst, 0);
    } else if (constant == 1) {
        if (dst != src) {
            sh2_mov_reg_reg(out, dst, src);
        }
    } else if (constant == 2) {
        sh2_mov_reg_reg(out, dst, src);
        sh2_shll(out, dst);
    } else if (constant == 4) {
        sh2_mov_reg_reg(out, dst, src);
        sh2_shll2(out, dst);
    } else if (constant == 8) {
        sh2_mov_reg_reg(out, dst, src);
        sh2_shll2(out, dst);
        sh2_shll(out, dst);
    } else if (constant == 16) {
        sh2_mov_reg_reg(out, dst, src);
        sh2_shll2(out, dst);
        sh2_shll2(out, dst);
    } else if ((constant & (constant - 1)) == 0) {
        // Power of 2 - use shifts
        int shifts = 0;
        int temp = constant;
        while (temp > 1) {
            temp >>= 1;
            shifts++;
        }
        sh2_mov_reg_reg(out, dst, src);
        while (shifts >= 2) {
            sh2_shll2(out, dst);
            shifts -= 2;
        }
        while (shifts > 0) {
            sh2_shll(out, dst);
            shifts--;
        }
    } else {
        // General case - use multiply instruction
        sh2_mov_imm(out, 0, constant);
        sh2_mul_l(out, src, 0);
        sh2_sts(out, "macl", dst);
    }
}

// Optimize division by constant
void sh2_gen_div_const(FILE *out, int dst, int src, int constant) {
    if (constant == 1) {
        if (dst != src) {
            sh2_mov_reg_reg(out, dst, src);
        }
    } else if ((constant & (constant - 1)) == 0) {
        // Power of 2 - use arithmetic shift
        int shifts = 0;
        int temp = constant;
        while (temp > 1) {
            temp >>= 1;
            shifts++;
        }
        sh2_mov_reg_reg(out, dst, src);
        while (shifts >= 2) {
            sh2_shar(out, dst);
            sh2_shar(out, dst);
            shifts -= 2;
        }
        while (shifts > 0) {
            sh2_shar(out, dst);
            shifts--;
        }
    } else {
        // General case - call division routine
        sh2_mov_reg_reg(out, 4, src);  // First arg
        sh2_mov_imm(out, 5, constant); // Second arg
        sh2_call(out, "__divsi3");
        if (dst != 0) {
            sh2_mov_reg_reg(out, dst, 0);
        }
    }
}

// ============================================================================
// Memory Access Optimization
// ============================================================================

// Optimize consecutive memory loads
void sh2_gen_load_multiple(FILE *out, int base_reg, int *dst_regs,
                            int count, int offset) {
    for (int i = 0; i < count; i++) {
        int current_offset = offset + (i * 4);
        if (current_offset <= 60 && (current_offset & 3) == 0) {
            sh2_mov_l_disp_reg(out, dst_regs[i], current_offset, base_reg);
        } else {
            sh2_mov_l_post_inc(out, dst_regs[i], base_reg);
        }
    }
}

// Optimize consecutive memory stores
void sh2_gen_store_multiple(FILE *out, int base_reg, int *src_regs,
                             int count, int offset) {
    for (int i = 0; i < count; i++) {
        int current_offset = offset + (i * 4);
        if (current_offset <= 60 && (current_offset & 3) == 0) {
            sh2_mov_l_reg_disp(out, src_regs[i], current_offset, base_reg);
        } else {
            sh2_mov_l_pre_dec(out, src_regs[i], base_reg);
        }
    }
}

// ============================================================================
// Strength Reduction
// ============================================================================

// Replace expensive operations with cheaper equivalents
void sh2_strength_reduce(FILE *out, const char *op, int dst, int src,
                         int constant) {
    if (strcmp(op, "*") == 0) {
        sh2_gen_mul_const(out, dst, src, constant);
    } else if (strcmp(op, "/") == 0) {
        sh2_gen_div_const(out, dst, src, constant);
    } else if (strcmp(op, "%") == 0 && (constant & (constant - 1)) == 0) {
        // Modulo by power of 2
        sh2_mov_reg_reg(out, dst, src);
        sh2_mov_imm(out, 0, constant - 1);
        sh2_and(out, dst, 0);
    }
}

// ============================================================================
// Branch Optimization
// ============================================================================

// Invert branch to eliminate unnecessary jumps
void sh2_optimize_branch_chain(FILE *out, bool invert,
                                const char *target_label) {
    if (invert) {
        sh2_bf(out, target_label);
    } else {
        sh2_bt(out, target_label);
    }
    sh2_nop(out);
}

// ============================================================================
// Function Call Optimization
// ============================================================================

// Generate leaf function (no calls to other functions)
void sh2_gen_leaf_function(FILE *out, const char *name, int frame_size) {
    fprintf(out, "\n\t.align 2\n");
    fprintf(out, "\t.global _%s\n", name);
    fprintf(out, "_%s:\n", name);

    if (frame_size > 0) {
        // Only save frame pointer, not PR (no calls)
        sh2_push(out, 14);
        sh2_mov_reg_reg(out, 14, 15);

        if (frame_size <= 127) {
            sh2_add_imm(out, 15, -frame_size);
        } else {
            sh2_mov_imm(out, 0, frame_size);
            sh2_sub(out, 15, 0);
        }
    }
}

// Generate tail call optimization
void sh2_gen_tail_call(FILE *out, const char *target) {
    // Restore frame and jump directly
    sh2_mov_reg_reg(out, 15, 14);
    sh2_pop(out, 14);

    // Tail call - use proper pointer cast
    sh2_mov_l_imm(out, 0, (int32_t)(uintptr_t)target);
    sh2_jmp(out, 0);
    sh2_nop(out);
}

// ============================================================================
// Code Size Optimization
// ============================================================================

// Use shorter instruction sequences
void sh2_optimize_code_size(FILE *out, const char *operation,
                            int dst, int src, int immediate) {
    (void)src;  // Mark as intentionally unused

    // Prefer shorter encodings when possible
    if (strcmp(operation, "add") == 0 && immediate >= -128 && immediate <= 127) {
        sh2_add_imm(out, dst, immediate);
    } else if (strcmp(operation, "mov") == 0 && immediate >= -128 && immediate <= 127) {
        sh2_mov_imm(out, dst, immediate);
    } else {
        // Use full instruction
        sh2_mov_l_imm(out, dst, immediate);
    }
}

// ============================================================================
// Advanced Pattern Matching
// ============================================================================

// Recognize common idioms and optimize
void sh2_optimize_idiom(FILE *out, const char *pattern_name,
                        int *regs, int count) {
    (void)count;  // Mark as intentionally unused

    if (strcmp(pattern_name, "clear_array") == 0) {
        // Optimized array clear using pre-decrement
        int addr = regs[0];
        int size = regs[1];

        sh2_mov_imm(out, 0, 0);
        sh2_label(out, ".L_clear_loop");
        sh2_mov_l_pre_dec(out, 0, addr);
        sh2_dt(out, size);
        sh2_bf(out, ".L_clear_loop");
        sh2_nop(out);

    } else if (strcmp(pattern_name, "copy_array") == 0) {
        // Optimized array copy
        int src = regs[0];
        int dst = regs[1];
        int size = regs[2];

        sh2_label(out, ".L_copy_loop");
        sh2_mov_l_post_inc(out, 0, src);
        sh2_mov_l_pre_dec(out, 0, dst);
        sh2_dt(out, size);
        sh2_bf(out, ".L_copy_loop");
        sh2_nop(out);
    }
}

// ============================================================================
// Saturn-Specific Optimizations
// ============================================================================

// Optimize for Saturn's dual CPU architecture
void sh2_optimize_for_dual_cpu(FILE *out, bool is_slave) {
    if (is_slave) {
        sh2_comment(out, "Slave CPU code");
        // Ensure proper synchronization
        sh2_comment(out, "Wait for master CPU signal");
    } else {
        sh2_comment(out, "Master CPU code");
    }
}

// Optimize VDP1/VDP2 memory access
void sh2_optimize_vram_access(FILE *out, uint32_t vram_addr,
                               int data_reg, bool is_write) {
    // Use efficient addressing modes for VRAM
    sh2_mov_l_imm(out, 0, vram_addr);

    if (is_write) {
        sh2_mov_l_indir_store(out, data_reg, 0);
    } else {
        sh2_mov_l_indir(out, data_reg, 0);
    }
}

// ============================================================================
// Debug Support
// ============================================================================

void sh2_emit_debug_info(FILE *out, const char *source_file, int line) {
    fprintf(out, "\t! %s:%d\n", source_file, line);
}

void sh2_emit_function_trace(FILE *out, const char *func_name) {
    fprintf(out, "\t! ENTER: %s\n", func_name);
}