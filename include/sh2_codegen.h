// include/sh2_codegen.h - SH-2 Code Generation
// ============================================================================
#ifndef SH2_CODEGEN_H
#define SH2_CODEGEN_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    int reg;
    int offset;
    int is_memory;
} SH2Operand;

void sh2_emit_prologue(FILE *out, const char *func_name, int frame_size);
void sh2_emit_epilogue(FILE *out);
void sh2_emit_load_imm(FILE *out, int reg, int32_t value);
void sh2_emit_mov(FILE *out, int dst, int src);
void sh2_emit_add(FILE *out, int dst, int src);
void sh2_emit_sub(FILE *out, int dst, int src);
void sh2_emit_mul(FILE *out, int dst, int src);
void sh2_emit_div(FILE *out, int dst, int src);
void sh2_emit_and(FILE *out, int dst, int src);
void sh2_emit_or(FILE *out, int dst, int src);
void sh2_emit_xor(FILE *out, int dst, int src);
void sh2_emit_not(FILE *out, int reg);
void sh2_emit_neg(FILE *out, int reg);
void sh2_emit_load_mem(FILE *out, int dst, int base, int offset);
void sh2_emit_store_mem(FILE *out, int src, int base, int offset);
void sh2_emit_push(FILE *out, int reg);
void sh2_emit_pop(FILE *out, int reg);
void sh2_emit_call(FILE *out, const char *func_name);
void sh2_emit_return(FILE *out);
void sh2_emit_branch(FILE *out, const char *label);
void sh2_emit_branch_if_zero(FILE *out, int reg, const char *label);
void sh2_emit_branch_if_not_zero(FILE *out, int reg, const char *label);
void sh2_emit_compare(FILE *out, int reg1, int reg2);
void sh2_emit_label(FILE *out, const char *label);
void sh2_emit_comment(FILE *out, const char *comment);

#endif // SH2_CODEGEN_H