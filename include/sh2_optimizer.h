// ============================================================================
// include/sh2_optimizer.h - SH-2 Optimization Engine
// ============================================================================
#ifndef SH2_OPTIMIZER_H
#define SH2_OPTIMIZER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// ============================================================================
// Forward Declarations
// ============================================================================

typedef struct RegisterAllocatorStruct RegisterAllocator;
typedef struct BasicBlock BasicBlock;
typedef struct FlowGraph FlowGraph;

// ============================================================================
// Optimization Levels
// ============================================================================

typedef enum {
    OPT_LEVEL_NONE = 0,      // -O0: No optimization
    OPT_LEVEL_SIZE = 1,      // -Os: Optimize for size
    OPT_LEVEL_SPEED = 2,     // -O1: Basic optimizations
    OPT_LEVEL_AGGRESSIVE = 3 // -O2/-O3: Aggressive optimizations
} OptimizationLevel;

// ============================================================================
// Register Allocation
// ============================================================================

RegisterAllocator* sh2_alloc_create(void);
void sh2_alloc_destroy(RegisterAllocator *alloc);
int sh2_alloc_find_victim(RegisterAllocator *alloc);
int sh2_alloc_get_free_register(RegisterAllocator *alloc);
void sh2_alloc_mark_used(RegisterAllocator *alloc, int reg);
void sh2_alloc_mark_free(RegisterAllocator *alloc, int reg);
bool sh2_alloc_is_caller_saved(int reg);
bool sh2_alloc_is_callee_saved(int reg);

// ============================================================================
// Peephole Optimization
// ============================================================================

typedef struct {
    char **instructions;
    int count;
} InstructionSequence;

InstructionSequence* sh2_peephole_optimize(const char **input, int count);
void sh2_peephole_free(InstructionSequence *seq);
bool sh2_peephole_apply_pattern(const char **input, int count,
                                 char **output, int *output_count);

// ============================================================================
// Delay Slot Optimization
// ============================================================================

bool sh2_can_use_in_delay_slot(const char *instruction);
void sh2_optimize_delay_slot(FILE *out, const char *branch_inst,
                              const char *next_inst);
bool sh2_is_branch_instruction(const char *inst);
bool sh2_has_delay_slot(const char *inst);

// ============================================================================
// Code Generation Patterns
// ============================================================================

// Comparison and branching
void sh2_gen_compare(FILE *out, const char *op, int lhs, int rhs,
                     const char *true_label);

// Loop generation
void sh2_gen_loop(FILE *out, int counter_reg, int count,
                  const char *body_label, const char *end_label);

// Switch statement with jump table
void sh2_gen_switch(FILE *out, int value_reg, int num_cases,
                    const char *table_label);

// ============================================================================
// Arithmetic Optimization
// ============================================================================

// Optimize multiplication by constant
void sh2_gen_mul_const(FILE *out, int dst, int src, int constant);

// Optimize division by constant
void sh2_gen_div_const(FILE *out, int dst, int src, int constant);

// Optimize modulo by constant (power of 2)
void sh2_gen_mod_const(FILE *out, int dst, int src, int constant);

// Check if value is power of 2
bool sh2_is_power_of_2(int value);

// Count trailing zeros (for shift optimization)
int sh2_count_trailing_zeros(unsigned int value);

// ============================================================================
// Memory Access Optimization
// ============================================================================

// Optimize consecutive loads
void sh2_gen_load_multiple(FILE *out, int base_reg, int *dst_regs,
                            int count, int offset);

// Optimize consecutive stores
void sh2_gen_store_multiple(FILE *out, int base_reg, int *src_regs,
                             int count, int offset);

// Optimize structure field access
void sh2_gen_struct_access(FILE *out, int dst, int base, int offset);

// Optimize array element access
void sh2_gen_array_access(FILE *out, int dst, int base, int index,
                           int element_size);

// ============================================================================
// Strength Reduction
// ============================================================================

// Replace expensive operations with cheaper equivalents
void sh2_strength_reduce(FILE *out, const char *op, int dst, int src,
                         int constant);

// Replace multiplication with shifts and adds
bool sh2_can_strength_reduce_mul(int constant);

// Replace division with shifts
bool sh2_can_strength_reduce_div(int constant);

// ============================================================================
// Branch Optimization
// ============================================================================

// Invert branch conditions to eliminate jumps
void sh2_optimize_branch_chain(FILE *out, bool invert,
                                const char *target_label);

// Eliminate dead code after unconditional branches
bool sh2_is_unreachable(const char **instructions, int index);

// Merge basic blocks
void sh2_merge_basic_blocks(FlowGraph *cfg);

// ============================================================================
// Function Call Optimization
// ============================================================================

// Generate leaf function (no calls to other functions)
void sh2_gen_leaf_function(FILE *out, const char *name, int frame_size);

// Generate tail call optimization
void sh2_gen_tail_call(FILE *out, const char *target);

// Inline small functions
bool sh2_should_inline(const char *func_name, int size);

// ============================================================================
// Code Size Optimization
// ============================================================================

// Use shorter instruction sequences
void sh2_optimize_code_size(FILE *out, const char *operation,
                            int dst, int src, int immediate);

// Compress immediate values
bool sh2_can_use_short_immediate(int value);

// Share common code sequences
void sh2_share_common_sequences(FILE *out);

// ============================================================================
// Loop Optimization
// ============================================================================

// Unroll small loops
void sh2_unroll_loop(FILE *out, int iterations, const char *body_label);

// Convert while loops to do-while (eliminates one branch)
void sh2_optimize_loop_structure(FILE *out);

// Hoist loop-invariant code
void sh2_hoist_invariants(FILE *out);

// ============================================================================
// Common Pattern Recognition
// ============================================================================

// Recognize and optimize common idioms
void sh2_optimize_idiom(FILE *out, const char *pattern_name,
                        int *regs, int count);

// Pattern: clear memory region
void sh2_gen_memset_zero(FILE *out, int addr_reg, int size_reg);

// Pattern: copy memory region
void sh2_gen_memcpy_fast(FILE *out, int dst_reg, int src_reg, int size_reg);

// Pattern: bit manipulation
void sh2_gen_set_bit(FILE *out, int reg, int bit_pos);
void sh2_gen_clear_bit(FILE *out, int reg, int bit_pos);
void sh2_gen_toggle_bit(FILE *out, int reg, int bit_pos);
void sh2_gen_test_bit(FILE *out, int reg, int bit_pos, const char *label);

// ============================================================================
// Saturn-Specific Optimizations
// ============================================================================

// Optimize for dual CPU architecture
void sh2_optimize_for_dual_cpu(FILE *out, bool is_slave);

// Synchronize master and slave CPUs
void sh2_gen_cpu_sync(FILE *out, bool is_master);

// Optimize VDP1/VDP2 memory access
void sh2_optimize_vram_access(FILE *out, uint32_t vram_addr,
                               int data_reg, bool is_write);

// Optimize DMA transfers
void sh2_gen_dma_transfer(FILE *out, int src_reg, int dst_reg,
                          int size_reg, int channel);

// Batch VDP commands
void sh2_optimize_vdp_commands(FILE *out);

// ============================================================================
// Data Flow Analysis
// ============================================================================

typedef struct {
    bool *live_in;
    bool *live_out;
    bool *def;
    bool *use;
} LivenessInfo;

LivenessInfo* sh2_analyze_liveness(BasicBlock *bb, int num_regs);
void sh2_free_liveness_info(LivenessInfo *info);

// ============================================================================
// Control Flow Graph
// ============================================================================

struct BasicBlock {
    char **instructions;
    int inst_count;
    struct BasicBlock **successors;
    int succ_count;
    struct BasicBlock **predecessors;
    int pred_count;
    int id;
};

struct FlowGraph {
    BasicBlock **blocks;
    int block_count;
    BasicBlock *entry;
    BasicBlock *exit;
};

FlowGraph* sh2_build_cfg(const char **instructions, int count);
void sh2_free_cfg(FlowGraph *cfg);
void sh2_optimize_cfg(FlowGraph *cfg);

// ============================================================================
// Dead Code Elimination
// ============================================================================

void sh2_eliminate_dead_code(FlowGraph *cfg);
bool sh2_is_dead_instruction(const char *inst, LivenessInfo *info);

// ============================================================================
// Constant Propagation
// ============================================================================

typedef struct {
    int reg;
    int value;
    bool is_constant;
} ConstantInfo;

void sh2_propagate_constants(FlowGraph *cfg);
bool sh2_is_constant_op(const char *inst, ConstantInfo *info);

// ============================================================================
// Common Subexpression Elimination
// ============================================================================

void sh2_eliminate_common_subexpressions(FlowGraph *cfg);
bool sh2_are_expressions_equivalent(const char *expr1, const char *expr2);

// ============================================================================
// Instruction Scheduling
// ============================================================================

typedef struct {
    const char *instruction;
    int *dependencies;
    int dep_count;
    int earliest_time;
    int priority;
} ScheduledInstruction;

void sh2_schedule_instructions(FILE *out, const char **instructions,
                                int count, bool optimize_for_pipeline);

// ============================================================================
// Debug and Profiling Support
// ============================================================================

void sh2_emit_debug_info(FILE *out, const char *source_file, int line);
void sh2_emit_function_trace(FILE *out, const char *func_name);
void sh2_emit_profiling_code(FILE *out, const char *label);

// ============================================================================
// Optimization Statistics
// ============================================================================

typedef struct {
    int instructions_eliminated;
    int branches_optimized;
    int constants_folded;
    int registers_saved;
    int code_size_reduction;
} OptimizationStats;

OptimizationStats* sh2_create_stats(void);
void sh2_update_stats(OptimizationStats *stats, const char *opt_type,
                      int improvement);
void sh2_print_stats(OptimizationStats *stats, FILE *out);
void sh2_free_stats(OptimizationStats *stats);

// ============================================================================
// Optimization Pipeline
// ============================================================================

typedef struct {
    OptimizationLevel level;
    bool optimize_size;
    bool optimize_speed;
    bool enable_peephole;
    bool enable_cse;
    bool enable_dce;
    bool enable_constant_prop;
    bool enable_loop_unroll;
    bool enable_inline;
    bool saturn_dual_cpu;
} OptimizationOptions;

void sh2_optimize_function(FILE *out, const char *func_name,
                           const char **instructions, int count,
                           OptimizationOptions *opts);

// ============================================================================
// Utility Functions
// ============================================================================

// Parse instruction to get operands
void sh2_parse_instruction(const char *inst, char *opcode,
                           int *operands, int *operand_count);

// Check if instruction modifies register
bool sh2_modifies_register(const char *inst, int reg);

// Check if instruction uses register
bool sh2_uses_register(const char *inst, int reg);

// Get instruction latency (for scheduling)
int sh2_get_instruction_latency(const char *inst);

// Check if instructions can be reordered
bool sh2_can_reorder(const char *inst1, const char *inst2);

#endif // SH2_OPTIMIZER_H