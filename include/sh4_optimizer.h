// include/sh4_optimizer.h
// SH4 Code Optimizer

#ifndef SH4_OPTIMIZER_H
#define SH4_OPTIMIZER_H

// Optimizer structure
typedef struct {
    int enabled;
    int optimization_level; // 0=none, 1=basic, 2=normal, 3=aggressive
    int peephole_enabled;
    int delay_slot_enabled;
} SH4Optimizer;

// Initialize and cleanup
void sh4_optimizer_init(SH4Optimizer* opt);
void sh4_optimizer_cleanup(SH4Optimizer* opt);

// Optimization passes
int sh4_optimize_redundant_moves(SH4Optimizer* opt, void* instructions, int count);
int sh4_optimize_delay_slots(SH4Optimizer* opt, void* instructions, int count);
int sh4_optimize_strength_reduction(SH4Optimizer* opt, void* instructions, int count);
int sh4_optimize_constant_folding(SH4Optimizer* opt, void* instructions, int count);
int sh4_optimize_dead_code(SH4Optimizer* opt, void* instructions, int count);

// Run all optimizations
int sh4_optimize(SH4Optimizer* opt, void* instructions, int count);

#endif // SH4_OPTIMIZER_H