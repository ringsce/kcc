#ifndef MULTIARCH_CODEGEN_H
#define MULTIARCH_CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// Target architectures
typedef enum {
    ARCH_X86_64,
    ARCH_ARM64,
    ARCH_UNKNOWN
} TargetArch;

// Target platforms
typedef enum {
    PLATFORM_LINUX,
    PLATFORM_MACOS,
    PLATFORM_UNKNOWN
} TargetPlatform;

// Calling conventions
typedef enum {
    CALL_CONV_SYSV,     // System V ABI (Linux x86_64)
    CALL_CONV_MS,       // Microsoft ABI (Windows x86_64) 
    CALL_CONV_AARCH64   // ARM64 ABI (both Linux and macOS)
} CallingConvention;

// Register types
typedef enum {
    REG_GENERAL,    // General purpose registers
    REG_FLOAT,      // Floating point registers
    REG_VECTOR,     // Vector/SIMD registers
    REG_SPECIAL     // Special purpose registers
} RegisterType;

// Register information
typedef struct {
    const char *name;
    RegisterType type;
    int size;        // Size in bytes
    bool preserved;  // Callee-saved register
} RegisterInfo;

// Target configuration
typedef struct {
    TargetArch arch;
    TargetPlatform platform;
    CallingConvention calling_conv;
    
    // Architecture-specific info
    const char *arch_name;
    const char *platform_name;
    int pointer_size;
    int stack_alignment;
    
    // Register sets
    const RegisterInfo *general_regs;
    const RegisterInfo *float_regs;
    int num_general_regs;
    int num_float_regs;
    
    // Calling convention specifics
    const char **param_regs;      // Parameter passing registers
    const char **return_regs;     // Return value registers
    int num_param_regs;
    int num_return_regs;
    
    // System call information
    const char *syscall_reg;      // System call number register
    const char *syscall_instruction;
    
    // Assembly syntax
    const char *comment_prefix;
    const char *global_directive;
    const char *section_text;
    const char *section_data;
    bool att_syntax;              // AT&T vs Intel syntax
    
} TargetConfig;

// Multi-architecture code generator
typedef struct MultiArchCodegen {
    FILE *output_file;
    TargetConfig *target;
    
    // Code generation state
    int label_counter;
    int temp_counter;
    int current_function_locals;
    int stack_offset;
    
    // Symbol tables and tracking
    struct {
        char name[64];
        int offset;
        int size;
    } local_vars[256];
    int local_var_count;
    
    // Function prologue/epilogue tracking
    bool in_function;
    char current_function[64];
    int stack_size;
    
} MultiArchCodegen;

// Target configuration functions
TargetConfig *target_config_create(TargetArch arch, TargetPlatform platform);
void target_config_destroy(TargetConfig *config);
TargetArch detect_host_architecture(void);
TargetPlatform detect_host_platform(void);

// Multi-arch codegen functions
MultiArchCodegen *multiarch_codegen_create(const char *output_file, TargetArch arch, TargetPlatform platform);
void multiarch_codegen_destroy(MultiArchCodegen *codegen);

// Main code generation
bool multiarch_codegen_generate(MultiArchCodegen *codegen, struct ASTNode *ast);

// Architecture-neutral code generation
void multiarch_emit(MultiArchCodegen *codegen, const char *format, ...);
void multiarch_emit_comment(MultiArchCodegen *codegen, const char *comment);
void multiarch_emit_label(MultiArchCodegen *codegen, const char *label);
void multiarch_emit_directive(MultiArchCodegen *codegen, const char *directive, const char *args);

// Function management
void multiarch_function_prologue(MultiArchCodegen *codegen, const char *func_name, int param_count);
void multiarch_function_epilogue(MultiArchCodegen *codegen);
void multiarch_function_call(MultiArchCodegen *codegen, const char *func_name, int arg_count);
void multiarch_function_return(MultiArchCodegen *codegen, bool has_value);

// Memory operations
void multiarch_load_immediate(MultiArchCodegen *codegen, const char *dest_reg, long value);
void multiarch_load_address(MultiArchCodegen *codegen, const char *dest_reg, const char *symbol);
void multiarch_load_memory(MultiArchCodegen *codegen, const char *dest_reg, const char *src_addr, int size);
void multiarch_store_memory(MultiArchCodegen *codegen, const char *src_reg, const char *dest_addr, int size);

// Arithmetic operations
void multiarch_add(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_sub(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_mul(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_div(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_mod(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_neg(MultiArchCodegen *codegen, const char *dest, const char *src);

// Logical operations
void multiarch_and(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_or(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_xor(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2);
void multiarch_not(MultiArchCodegen *codegen, const char *dest, const char *src);

// Comparison operations
void multiarch_compare(MultiArchCodegen *codegen, const char *reg1, const char *reg2);
void multiarch_test(MultiArchCodegen *codegen, const char *reg1, const char *reg2);

// Control flow
void multiarch_jump(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_zero(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_not_zero(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_equal(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_not_equal(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_less(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_greater(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_less_equal(MultiArchCodegen *codegen, const char *label);
void multiarch_jump_if_greater_equal(MultiArchCodegen *codegen, const char *label);

// Register management
const char *multiarch_get_param_reg(MultiArchCodegen *codegen, int param_index);
const char *multiarch_get_return_reg(MultiArchCodegen *codegen);
const char *multiarch_get_temp_reg(MultiArchCodegen *codegen, int index);
const char *multiarch_get_stack_pointer(MultiArchCodegen *codegen);
const char *multiarch_get_frame_pointer(MultiArchCodegen *codegen);

// Stack operations
void multiarch_push(MultiArchCodegen *codegen, const char *reg);
void multiarch_pop(MultiArchCodegen *codegen, const char *reg);
void multiarch_stack_alloc(MultiArchCodegen *codegen, int bytes);
void multiarch_stack_dealloc(MultiArchCodegen *codegen, int bytes);

// Variable management
void multiarch_declare_local_var(MultiArchCodegen *codegen, const char *name, int size);
int multiarch_get_local_var_offset(MultiArchCodegen *codegen, const char *name);
void multiarch_load_local_var(MultiArchCodegen *codegen, const char *dest_reg, const char *var_name);
void multiarch_store_local_var(MultiArchCodegen *codegen, const char *src_reg, const char *var_name);

// System calls
void multiarch_syscall(MultiArchCodegen *codegen, int syscall_num, int arg_count);
void multiarch_exit_program(MultiArchCodegen *codegen, int exit_code);

// Utility functions
char *multiarch_new_label(MultiArchCodegen *codegen);
char *multiarch_new_temp(MultiArchCodegen *codegen);
void multiarch_align_stack(MultiArchCodegen *codegen);

// AST-specific code generation
void multiarch_codegen_program(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_function_declaration(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_variable_declaration(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_statement(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_expression(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_binary_expr(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_unary_expr(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_call_expr(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_return_stmt(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_if_stmt(MultiArchCodegen *codegen, struct ASTNode *node);
void multiarch_codegen_while_stmt(MultiArchCodegen *codegen, struct ASTNode *node);

#endif // MULTIARCH_CODEGEN_H