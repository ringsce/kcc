// include/sh4_codegen.h
// SH4 Code Generator Interface for Sega Dreamcast

#ifndef SH4_CODEGEN_H
#define SH4_CODEGEN_H

#include <stdio.h>
#include "sh4_registers.h"
#include "sh4_register_allocator.h"

// Forward declaration
typedef struct ASTNode ASTNode;

// SH4 Code Generator structure
typedef struct {
    FILE* output;
    int label_counter;
    int stack_offset;
    int in_function;
    SH4RegisterAllocator regalloc;
} SH4CodeGen;

// Initialization and cleanup
void sh4_codegen_init(SH4CodeGen* gen, FILE* output);
void sh4_codegen_cleanup(SH4CodeGen* gen);

// Code generation
void sh4_generate_code(SH4CodeGen* gen, ASTNode* node);

// Emission functions
void sh4_emit_header(SH4CodeGen* gen);
void sh4_emit_function_prologue(SH4CodeGen* gen, const char* func_name);
void sh4_emit_function_epilogue(SH4CodeGen* gen);

// Instruction emission
void sh4_emit_mov(SH4CodeGen* gen, int dest_reg, int src_reg);
void sh4_emit_movi(SH4CodeGen* gen, int dest_reg, int immediate);
void sh4_emit_add(SH4CodeGen* gen, int dest_reg, int src_reg);
void sh4_emit_sub(SH4CodeGen* gen, int dest_reg, int src_reg);
void sh4_emit_mul(SH4CodeGen* gen, int dest_reg, int src_reg);
void sh4_emit_return(SH4CodeGen* gen, int value_reg);

// Label management
int sh4_new_label(SH4CodeGen* gen);
void sh4_emit_label(SH4CodeGen* gen, int label_id);

#endif // SH4_CODEGEN_H