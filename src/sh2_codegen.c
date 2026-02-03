// ============================================================================
// src/sh2_codegen.c - SH-2 Code Generation Implementation
// ============================================================================
#include "sh2_codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int callee_saved_regs[] = {8, 9, 10, 11, 12, 13, 14};
const int caller_saved_regs[] = {0, 1, 2, 3, 4, 5, 6, 7};
const int argument_regs[] = {4, 5, 6, 7};

void sh2_emit_prologue(FILE *out, const char *func_name, int frame_size) {
    fprintf(out, "\n\t.align 2\n");
    fprintf(out, "\t.global _%s\n", func_name);
    fprintf(out, "_%s:\n", func_name);

    // Save frame pointer and return address
    fprintf(out, "\tmov.l\tr14,@-r15\n");
    fprintf(out, "\tsts.l\tpr,@-r15\n");

    // Set up new frame pointer
    fprintf(out, "\tmov\tr15,r14\n");

    // Allocate stack frame
    if (frame_size > 0) {
        if (frame_size <= 127) {
            fprintf(out, "\tadd\t#-%d,r15\n", frame_size);
        } else {
            fprintf(out, "\tmov.l\t.L_frame_%d,r0\n", frame_size);
            fprintf(out, "\tsub\tr0,r15\n");
        }
    }
}

void sh2_emit_epilogue(FILE *out) {
    // Restore stack pointer
    fprintf(out, "\tmov\tr14,r15\n");

    // Restore return address and frame pointer
    fprintf(out, "\tlds.l\t@r15+,pr\n");
    fprintf(out, "\tmov.l\t@r15+,r14\n");

    // Return with delay slot
    fprintf(out, "\trts\n");
    fprintf(out, "\tnop\n");
}

void sh2_emit_load_imm(FILE *out, int reg, int32_t value) {
    if (value >= -128 && value <= 127) {
        fprintf(out, "\tmov\t#%d,r%d\n", value, reg);
    } else if (value >= -32768 && value <= 32767) {
        fprintf(out, "\tmov\t#%d,r%d\n", (int16_t)value, reg);
        if (value > 127 || value < -128) {
            fprintf(out, "\textu.w\tr%d,r%d\n", reg, reg);
        }
    } else {
        // Large constant from literal pool
        fprintf(out, "\tmov.l\t.L_const_%d,r%d\n", value, reg);
    }
}

void sh2_emit_mov(FILE *out, int dst, int src) {
    fprintf(out, "\tmov\tr%d,r%d\n", src, dst);
}

void sh2_emit_add(FILE *out, int dst, int src) {
    fprintf(out, "\tadd\tr%d,r%d\n", src, dst);
}

void sh2_emit_sub(FILE *out, int dst, int src) {
    fprintf(out, "\tsub\tr%d,r%d\n", src, dst);
}

void sh2_emit_mul(FILE *out, int dst, int src) {
    // SH-2 multiply: mul.l, result in MACL
    fprintf(out, "\tmul.l\tr%d,r%d\n", src, dst);
    fprintf(out, "\tsts\tmacl,r%d\n", dst);
}

void sh2_emit_div(FILE *out, int dst, int src) {
    // Division requires calling a library function
    fprintf(out, "\tmov\tr%d,r4\n", dst);
    fprintf(out, "\tmov\tr%d,r5\n", src);
    fprintf(out, "\tmov.l\t.L___divsi3,r0\n");
    fprintf(out, "\tjsr\t@r0\n");
    fprintf(out, "\tnop\n");
    if (dst != 0) {
        fprintf(out, "\tmov\tr0,r%d\n", dst);
    }
}

void sh2_emit_and(FILE *out, int dst, int src) {
    fprintf(out, "\tand\tr%d,r%d\n", src, dst);
}

void sh2_emit_or(FILE *out, int dst, int src) {
    fprintf(out, "\tor\tr%d,r%d\n", src, dst);
}

void sh2_emit_xor(FILE *out, int dst, int src) {
    fprintf(out, "\txor\tr%d,r%d\n", src, dst);
}

void sh2_emit_not(FILE *out, int reg) {
    fprintf(out, "\tnot\tr%d,r%d\n", reg, reg);
}

void sh2_emit_neg(FILE *out, int reg) {
    fprintf(out, "\tneg\tr%d,r%d\n", reg, reg);
}

void sh2_emit_load_mem(FILE *out, int dst, int base, int offset) {
    if (offset == 0) {
        fprintf(out, "\tmov.l\t@r%d,r%d\n", base, dst);
    } else if (offset > 0 && offset <= 60 && (offset & 3) == 0) {
        fprintf(out, "\tmov.l\t@(%d,r%d),r%d\n", offset, base, dst);
    } else {
        // Use r0 for large offsets
        sh2_emit_load_imm(out, 0, offset);
        fprintf(out, "\tmov.l\t@(r0,r%d),r%d\n", base, dst);
    }
}

void sh2_emit_store_mem(FILE *out, int src, int base, int offset) {
    if (offset == 0) {
        fprintf(out, "\tmov.l\tr%d,@r%d\n", src, base);
    } else if (offset > 0 && offset <= 60 && (offset & 3) == 0) {
        fprintf(out, "\tmov.l\tr%d,@(%d,r%d)\n", src, offset, base);
    } else {
        // Use r0 for large offsets
        sh2_emit_load_imm(out, 0, offset);
        fprintf(out, "\tmov.l\tr%d,@(r0,r%d)\n", src, base);
    }
}

void sh2_emit_push(FILE *out, int reg) {
    fprintf(out, "\tmov.l\tr%d,@-r15\n", reg);
}

void sh2_emit_pop(FILE *out, int reg) {
    fprintf(out, "\tmov.l\t@r15+,r%d\n", reg);
}

void sh2_emit_call(FILE *out, const char *func_name) {
    fprintf(out, "\tmov.l\t.L_%s,r0\n", func_name);
    fprintf(out, "\tjsr\t@r0\n");
    fprintf(out, "\tnop\n");
}

void sh2_emit_return(FILE *out) {
    sh2_emit_epilogue(out);
}

void sh2_emit_branch(FILE *out, const char *label) {
    fprintf(out, "\tbra\t%s\n", label);
    fprintf(out, "\tnop\n");
}

void sh2_emit_branch_if_zero(FILE *out, int reg, const char *label) {
    fprintf(out, "\ttst\tr%d,r%d\n", reg, reg);
    fprintf(out, "\tbt\t%s\n", label);
}

void sh2_emit_branch_if_not_zero(FILE *out, int reg, const char *label) {
    fprintf(out, "\ttst\tr%d,r%d\n", reg, reg);
    fprintf(out, "\tbf\t%s\n", label);
}

void sh2_emit_compare(FILE *out, int reg1, int reg2) {
    fprintf(out, "\tcmp/eq\tr%d,r%d\n", reg2, reg1);
}

void sh2_emit_label(FILE *out, const char *label) {
    fprintf(out, "%s:\n", label);
}

void sh2_emit_comment(FILE *out, const char *comment) {
    fprintf(out, "\t! %s\n", comment);
}
