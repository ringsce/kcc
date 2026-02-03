// ============================================================================
// examples/register_allocator_usage.c - Register Allocator Examples
// ============================================================================

#include "sh2_register_allocator.h"
#include "sh2_instruction_set.h"
#include <stdio.h>

// ============================================================================
// Example 1: Simple Function with Local Variables
// ============================================================================

void example_simple_function(FILE *out) {
    fprintf(out, "! Example 1: int add3(int a, int b, int c) { return a + b + c; }\n\n");

    SH2RegisterAllocator *alloc = sh2_regalloc_create(ALLOC_STRATEGY_GRAPH_COLOR);

    // Create virtual registers for parameters (precolored to argument registers)
    int v_a = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);  // Parameter a
    int v_b = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);  // Parameter b
    int v_c = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);  // Parameter c

    // Precolor parameters to SH-2 argument registers
    sh2_regalloc_precolor(alloc, v_a, 4);  // R4 = first arg
    sh2_regalloc_precolor(alloc, v_b, 5);  // R5 = second arg
    sh2_regalloc_precolor(alloc, v_c, 6);  // R6 = third arg

    // Create temporary for intermediate result
    int v_temp = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);

    // Record live ranges
    sh2_regalloc_add_use(alloc, v_a, 0);
    sh2_regalloc_add_use(alloc, v_b, 1);
    sh2_regalloc_add_use(alloc, v_temp, 1);
    sh2_regalloc_add_def(alloc, v_temp, 1);

    sh2_regalloc_add_use(alloc, v_temp, 2);
    sh2_regalloc_add_use(alloc, v_c, 2);

    // Allocate registers
    sh2_regalloc_allocate_registers(alloc);

    // Get physical registers
    int r_a = sh2_regalloc_get_register(alloc, v_a);      // R4
    int r_b = sh2_regalloc_get_register(alloc, v_b);      // R5
    int r_c = sh2_regalloc_get_register(alloc, v_c);      // R6
    int r_temp = sh2_regalloc_get_register(alloc, v_temp); // Allocated by algorithm

    // Generate code
    sh2_label(out, "_add3");
    sh2_comment(out, "a + b");
    sh2_mov_reg_reg(out, r_temp, r_a);
    sh2_add(out, r_temp, r_b);

    sh2_comment(out, "(a + b) + c");
    sh2_add(out, r_temp, r_c);

    sh2_comment(out, "Move result to R0");
    sh2_mov_reg_reg(out, 0, r_temp);

    sh2_rts(out);
    sh2_nop(out);
    fprintf(out, "\n");

    // Print allocation info
    sh2_regalloc_print_allocation(alloc, out);

    sh2_regalloc_destroy(alloc);
}

// ============================================================================
// Example 2: Function with Many Local Variables (Requires Spilling)
// ============================================================================

void example_register_pressure(FILE *out) {
    fprintf(out, "! Example 2: Function with high register pressure\n\n");

    SH2RegisterAllocator *alloc = sh2_regalloc_create(ALLOC_STRATEGY_GRAPH_COLOR);

    // Create 20 virtual registers (more than available physical registers)
    int vregs[20];
    for (int i = 0; i < 20; i++) {
        vregs[i] = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
    }

    // All live at the same time (worst case for register allocation)
    for (int i = 0; i < 20; i++) {
        sh2_regalloc_add_def(alloc, vregs[i], 0);
        sh2_regalloc_add_use(alloc, vregs[i], 100);  // Long live range
    }

    // Allocate registers
    sh2_regalloc_allocate_registers(alloc);

    // Generate function
    sh2_label(out, "_many_locals");
    sh2_push(out, 14);
    sh2_sts_l(out, "pr", 15);
    sh2_mov_reg_reg(out, 14, 15);

    // Allocate stack space for spills
    int num_spills = sh2_regalloc_get_num_spill_slots(alloc);
    if (num_spills > 0) {
        sh2_comment(out, "Allocate stack space for spills");
        sh2_add_imm(out, 15, -(num_spills * 4));
    }

    // Initialize all variables
    for (int i = 0; i < 20; i++) {
        int reg = sh2_regalloc_get_register(alloc, vregs[i]);

        if (sh2_regalloc_is_spilled(alloc, vregs[i])) {
            sh2_comment(out, "Initialize spilled variable");
            sh2_mov_imm(out, 0, i);
            sh2_regalloc_emit_spill(alloc, out, vregs[i], 0);
        } else {
            sh2_comment(out, "Initialize register variable");
            sh2_mov_imm(out, reg, i);
        }
    }

    // Use all variables (sum them)
    sh2_comment(out, "Sum all variables");
    sh2_mov_imm(out, 0, 0);  // Result in R0

    for (int i = 0; i < 20; i++) {
        if (sh2_regalloc_is_spilled(alloc, vregs[i])) {
            sh2_regalloc_emit_reload(alloc, out, vregs[i], 1);
            sh2_add(out, 0, 1);
        } else {
            int reg = sh2_regalloc_get_register(alloc, vregs[i]);
            sh2_add(out, 0, reg);
        }
    }

    // Epilogue
    sh2_mov_reg_reg(out, 15, 14);
    sh2_lds_l(out, 15, "pr");
    sh2_pop(out, 14);
    sh2_rts(out);
    sh2_nop(out);
    fprintf(out, "\n");

    sh2_regalloc_print_allocation(alloc, out);

    sh2_regalloc_destroy(alloc);
}

// ============================================================================
// Example 3: Loop with Live Range Analysis
// ============================================================================

void example_loop_allocation(FILE *out) {
    fprintf(out, "! Example 3: for (i=0; i<10; i++) sum += arr[i];\n\n");

    SH2RegisterAllocator *alloc = sh2_regalloc_create(ALLOC_STRATEGY_GRAPH_COLOR);

    // Virtual registers
    int v_arr = sh2_regalloc_new_vreg(alloc, VAR_TYPE_PTR);   // Array pointer
    int v_i = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);     // Loop counter
    int v_sum = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);   // Accumulator
    int v_temp = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);  // Temp for array access

    // Precolor parameters
    sh2_regalloc_precolor(alloc, v_arr, 4);  // R4 = array pointer

    // Live ranges
    sh2_regalloc_add_def(alloc, v_i, 0);      // i = 0
    sh2_regalloc_add_def(alloc, v_sum, 1);    // sum = 0
    sh2_regalloc_add_use(alloc, v_arr, 2);    // Loop start

    // Loop body (positions 10-20)
    for (int pos = 10; pos <= 20; pos++) {
        sh2_regalloc_add_use(alloc, v_i, pos);
        sh2_regalloc_add_use(alloc, v_sum, pos);
        sh2_regalloc_add_use(alloc, v_arr, pos);
        sh2_regalloc_add_use(alloc, v_temp, pos);
    }

    sh2_regalloc_add_use(alloc, v_i, 21);     // i++ at loop end

    // Prefer callee-saved register for loop counter (long live range)
    sh2_regalloc_set_hint(alloc, v_i, 8);     // Prefer R8
    sh2_regalloc_set_hint(alloc, v_sum, 9);   // Prefer R9

    // Allocate
    sh2_regalloc_allocate_registers(alloc);

    int r_arr = sh2_regalloc_get_register(alloc, v_arr);
    int r_i = sh2_regalloc_get_register(alloc, v_i);
    int r_sum = sh2_regalloc_get_register(alloc, v_sum);
    int r_temp = sh2_regalloc_get_register(alloc, v_temp);

    // Generate code
    sh2_label(out, "_sum_array");

    sh2_comment(out, "Initialize loop");
    sh2_mov_imm(out, r_i, 0);
    sh2_mov_imm(out, r_sum, 0);

    sh2_label(out, ".L_loop");
    sh2_comment(out, "Check loop condition");
    sh2_mov_imm(out, 0, 10);
    sh2_cmp_ge(out, r_i, 0);
    sh2_bt(out, ".L_done");
    sh2_nop(out);

    sh2_comment(out, "Load arr[i]");
    sh2_shll2(out, r_i);  // i * 4
    sh2_mov_l_r0_indexed(out, r_temp, r_arr);  // Load arr[i]
    sh2_shlr2(out, r_i);  // Restore i

    sh2_comment(out, "sum += arr[i]");
    sh2_add(out, r_sum, r_temp);

    sh2_comment(out, "i++");
    sh2_add_imm(out, r_i, 1);

    sh2_bra(out, ".L_loop");
    sh2_nop(out);

    sh2_label(out, ".L_done");
    sh2_mov_reg_reg(out, 0, r_sum);
    sh2_rts(out);
    sh2_nop(out);
    fprintf(out, "\n");

    sh2_regalloc_print_allocation(alloc, out);

    sh2_regalloc_destroy(alloc);
}

// ============================================================================
// Example 4: Register Coalescing for Move Elimination
// ============================================================================

void example_coalescing(FILE *out) {
    fprintf(out, "! Example 4: Move coalescing optimization\n\n");

    SH2RegisterAllocator *alloc = sh2_regalloc_create(ALLOC_STRATEGY_GRAPH_COLOR);

    // Create virtual registers
    int v1 = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
    int v2 = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
    int v3 = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);

    // Live ranges that don't overlap
    sh2_regalloc_add_def(alloc, v1, 0);
    sh2_regalloc_add_use(alloc, v1, 5);

    // v2 = v1 (copy)
    sh2_regalloc_add_def(alloc, v2, 6);
    sh2_regalloc_add_use(alloc, v2, 10);

    sh2_regalloc_add_def(alloc, v3, 7);
    sh2_regalloc_add_use(alloc, v3, 10);

    // Try to coalesce v1 and v2 (they're connected by a move)
    if (sh2_regalloc_can_coalesce(alloc, v1, v2)) {
        fprintf(out, "! Coalescing v1 and v2 (connected by move)\n");
        sh2_regalloc_coalesce(alloc, v1, v2);
    }

    sh2_regalloc_allocate_registers(alloc);

    int r1 = sh2_regalloc_get_register(alloc, v1);
    int r2 = sh2_regalloc_get_register(alloc, v2);
    int r3 = sh2_regalloc_get_register(alloc, v3);

    fprintf(out, "! After coalescing:\n");
    fprintf(out, "!   v1 -> r%d\n", r1);
    fprintf(out, "!   v2 -> r%d (should be same as v1)\n", r2);
    fprintf(out, "!   v3 -> r%d\n", r3);
    fprintf(out, "\n");

    sh2_regalloc_destroy(alloc);
}

// ============================================================================
// Example 5: Linear Scan vs Graph Coloring Comparison
// ============================================================================

void example_algorithm_comparison(FILE *out) {
    fprintf(out, "! Example 5: Comparing allocation strategies\n\n");

    // Same allocation problem with different strategies
    for (int strategy = 0; strategy < 2; strategy++) {
        const char *name = (strategy == 0) ? "Graph Coloring" : "Linear Scan";
        fprintf(out, "! Strategy: %s\n", name);

        SH2RegisterAllocator *alloc = sh2_regalloc_create(
            strategy == 0 ? ALLOC_STRATEGY_GRAPH_COLOR : ALLOC_STRATEGY_LINEAR_SCAN
        );

        // Create 10 virtual registers with overlapping live ranges
        int vregs[10];
        for (int i = 0; i < 10; i++) {
            vregs[i] = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
            sh2_regalloc_add_def(alloc, vregs[i], i);
            sh2_regalloc_add_use(alloc, vregs[i], i + 10);
        }

        // Allocate
        if (strategy == 0) {
            sh2_regalloc_allocate_registers(alloc);
        } else {
            sh2_regalloc_linear_scan(alloc);
        }

        // Get stats
        int spills, reloads, moves;
        sh2_regalloc_get_stats(alloc, &spills, &reloads, &moves);

        fprintf(out, "! Results:\n");
        fprintf(out, "!   Spills: %d\n", spills);
        fprintf(out, "!   Spill slots: %d\n", sh2_regalloc_get_num_spill_slots(alloc));
        fprintf(out, "\n");

        sh2_regalloc_destroy(alloc);
    }
}

// ============================================================================
// Example 6: Function Call with Register Saving
// ============================================================================

void example_function_call(FILE *out) {
    fprintf(out, "! Example 6: Function call with register preservation\n\n");

    SH2RegisterAllocator *alloc = sh2_regalloc_create(ALLOC_STRATEGY_GRAPH_COLOR);

    // Variables live across function call
    int v_a = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
    int v_b = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);
    int v_result = sh2_regalloc_new_vreg(alloc, VAR_TYPE_INT);

    // Live ranges spanning the call
    sh2_regalloc_add_def(alloc, v_a, 0);
    sh2_regalloc_add_use(alloc, v_a, 1);   // Before call
    sh2_regalloc_add_use(alloc, v_a, 20);  // After call (forces it across)

    sh2_regalloc_add_def(alloc, v_b, 0);
    sh2_regalloc_add_use(alloc, v_b, 20);  // Live across call

    sh2_regalloc_add_def(alloc, v_result, 15);  // Result from call

    // Hint callee-saved registers for values live across call
    sh2_regalloc_set_hint(alloc, v_a, 8);
    sh2_regalloc_set_hint(alloc, v_b, 9);

    sh2_regalloc_allocate_registers(alloc);

    int r_a = sh2_regalloc_get_register(alloc, v_a);
    int r_b = sh2_regalloc_get_register(alloc, v_b);
    int r_result = sh2_regalloc_get_register(alloc, v_result);

    sh2_label(out, "_caller");
    sh2_push(out, 14);
    sh2_sts_l(out, "pr", 15);

    // Save callee-saved registers we're using
    sh2_comment(out, "Save callee-saved registers");
    if (r_a >= 8 && r_a <= 13) sh2_push(out, r_a);
    if (r_b >= 8 && r_b <= 13 && r_b != r_a) sh2_push(out, r_b);

    sh2_comment(out, "Initialize variables");
    sh2_mov_imm(out, r_a, 10);
    sh2_mov_imm(out, r_b, 20);

    sh2_comment(out, "Prepare arguments and call");
    sh2_mov_reg_reg(out, 4, r_a);  // First argument
    sh2_call(out, "helper_function");

    sh2_comment(out, "Get result");
    sh2_mov_reg_reg(out, r_result, 0);

    sh2_comment(out, "Use variables after call");
    sh2_add(out, r_result, r_a);
    sh2_add(out, r_result, r_b);

    sh2_mov_reg_reg(out, 0, r_result);

    // Restore callee-saved registers
    sh2_comment(out, "Restore callee-saved registers");
    if (r_b >= 8 && r_b <= 13 && r_b != r_a) sh2_pop(out, r_b);
    if (r_a >= 8 && r_a <= 13) sh2_pop(out, r_a);

    sh2_lds_l(out, 15, "pr");
    sh2_pop(out, 14);
    sh2_rts(out);
    sh2_nop(out);
    fprintf(out, "\n");

    sh2_regalloc_print_allocation(alloc, out);

    sh2_regalloc_destroy(alloc);
}

// ============================================================================
// Main Function - Generate All Examples
// ============================================================================

int main(void) {
    FILE *out = fopen("register_allocation_examples.s", "w");
    if (!out) {
        fprintf(stderr, "Cannot create output file\n");
        return 1;
    }

    fprintf(out, "!\n");
    fprintf(out, "! SH-2 Register Allocation Examples\n");
    fprintf(out, "! Generated by KCC Register Allocator\n");
    fprintf(out, "!\n\n");

    fprintf(out, "\t.section .text\n");
    fprintf(out, "\t.align 2\n\n");

    example_simple_function(out);
    example_register_pressure(out);
    example_loop_allocation(out);
    example_coalescing(out);
    example_algorithm_comparison(out);
    example_function_call(out);

    fclose(out);

    printf("Generated register_allocation_examples.s successfully!\n");
    printf("\nRegister allocation examples demonstrate:\n");
    printf("  1. Simple function with parameter allocation\n");
    printf("  2. High register pressure with spilling\n");
    printf("  3. Loop optimization with live range analysis\n");
    printf("  4. Move coalescing for optimization\n");
    printf("  5. Algorithm comparison (graph coloring vs linear scan)\n");
    printf("  6. Function calls with register preservation\n");

    return 0;
}
