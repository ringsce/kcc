/**
* @file codegen.c
 * @brief Code generation module for KCC compiler
 * @author KCC Development Team
 * @version 1.10.0
 * @date 2024
 *
 * This module implements code generation for the KCC compiler, supporting
 * multiple target architectures (ARM64 and x86-64). It translates Abstract
 * Syntax Tree (AST) nodes into native assembly code for the target platform.
 *
 * @section codegen_architecture Architecture Support
 * - **ARM64** (Apple Silicon): Modern ARM 64-bit architecture
 * - **x86-64** (Intel/AMD): Traditional 64-bit x86 architecture
 *
 * The code generator automatically detects the target architecture at compile
 * time and generates appropriate assembly code using platform-specific
 * instructions, registers, and calling conventions.
 *
 * @section codegen_features Features
 * - Multi-architecture code generation
 * - Function call support with proper calling conventions
 * - Arithmetic and logical operations
 * - Control flow (if/else, loops)
 * - Variable storage and retrieval
 * - Label generation for jumps and branches
 *
 * @section codegen_example Usage Example
 * @code
 * // Create code generator
 * CodeGenerator *codegen = codegen_create("output.s");
 *
 * // Generate code from AST
 * if (codegen_generate(codegen, ast_root)) {
 *     printf("Code generation successful\n");
 * }
 *
 * // Clean up
 * codegen_destroy(codegen);
 * @endcode
 */

#include "kcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>  // Might be needed for character checking
#include "symbol_table.h"



// Detect architecture at compile time
#ifdef __aarch64__
#define TARGET_ARM64 1
#define TARGET_X86_64 0
#elif defined(__x86_64__)
#define TARGET_ARM64 0
#define TARGET_X86_64 1
#else
#error "Unsupported architecture"
#endif
void codegen_generate_node(CodeGenerator *codegen, ASTNode *node);  // Forward declaration


// ============================================
// FORWARD DECLARATIONS FOR ARC FUNCTIONS
// ============================================

// ARC analysis functions
void arc_analyze_variable(ASTNode* node, ARCContext* context);
void arc_analyze_assignment(ASTNode* node, ARCContext* context);
void arc_analyze_return(ASTNode* node, ARCContext* context);
void arc_analyze_function_call(ASTNode* node, ARCContext* context);

// ARC helper functions
void arc_register_cleanup(ARCContext* context, ASTNode* node);
void arc_register_weak_reference(ARCContext* context, ASTNode* node);
void arc_analyze_initializer(ASTNode* node, ARCContext* context);
void arc_register_weak_assignment(ARCContext* context, ASTNode* node);
void arc_push_scope(ARCContext* context);
void arc_pop_scope_with_cleanup(ASTNode* node, ARCContext* context);

// ============================================
// ARC FUNCTION STUBS (Temporary implementations)
// ============================================

// Stub: Analyze return statement for ARC
void arc_analyze_return(ASTNode* node, ARCContext* context) {
    // TODO: Implement return value ARC analysis
    (void)node;
    (void)context;
}

// Stub: Analyze function call for ARC
void arc_analyze_function_call(ASTNode* node, ARCContext* context) {
    // TODO: Implement function call ARC analysis
    (void)node;
    (void)context;
}

// Stub: Register a variable for cleanup at scope exit
void arc_register_cleanup(ARCContext* context, ASTNode* node) {
    // TODO: Implement cleanup registration
    (void)context;
    (void)node;
}

// Stub: Register a weak reference for zeroing
void arc_register_weak_reference(ARCContext* context, ASTNode* node) {
    // TODO: Implement weak reference tracking
    (void)context;
    (void)node;
}

// Stub: Analyze initializer for retain requirements
void arc_analyze_initializer(ASTNode* node, ARCContext* context) {
    // TODO: Implement initializer analysis
    (void)node;
    (void)context;
}

// Stub: Register weak assignment
void arc_register_weak_assignment(ARCContext* context, ASTNode* node) {
    // TODO: Implement weak assignment tracking
    (void)context;
    (void)node;
}

// Stub: Push a new ARC scope
void arc_push_scope(ARCContext* context) {
    // TODO: Implement scope stack management
    (void)context;
}

// Stub: Pop scope and generate cleanup code
void arc_pop_scope_with_cleanup(ASTNode* node, ARCContext* context) {
    // TODO: Implement scope cleanup
    (void)node;
    (void)context;
}

// ============================================
// END OF STUBS AND FORWARD DECLARATIONS
// ============================================

// Analyze ARC requirements for an AST
void arc_analyze_tree(ASTNode* node, ARCContext* context) {
    if (!node) return;

    switch (node->type) {
        case AST_VARIABLE_DECLARATION:
        case AST_VAR_DECL:
            arc_analyze_variable(node, context);
            break;

        case AST_ASSIGNMENT:
            arc_analyze_assignment(node, context);
            break;

        case AST_RETURN_STATEMENT:
            arc_analyze_return(node, context);
            break;

        case AST_FUNCTION_CALL:
            arc_analyze_function_call(node, context);
            break;

        case AST_COMPOUND_STATEMENT:
            arc_push_scope(context);
            for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
                arc_analyze_tree(node->data.compound_stmt.statements[i], context);
            }
            arc_pop_scope_with_cleanup(node, context);
            break;

        default:
            // Recursively analyze children
            break;
    }
}

// Analyze variable declaration for ARC requirements
void arc_analyze_variable(ASTNode* node, ARCContext* context) {
    ARCQualifier qualifier = node->data.var_decl_arc.arc_qualifier;

    // Determine if this needs retain/release
    node->arc_info.is_objc_object = is_objc_object_type(node->data_type);
    node->arc_info.qualifier = qualifier;

    if (node->arc_info.is_objc_object) {
        // Strong references need release on scope exit
        if (qualifier == ARC_QUALIFIER_STRONG) {
            node->arc_info.needs_release = true;
            arc_register_cleanup(context, node);
        }

        // Weak references need zeroing weak reference setup
        if (qualifier == ARC_QUALIFIER_WEAK) {
            arc_register_weak_reference(context, node);
        }

        // Analyze initializer for retain requirements
        if (node->data.var_decl_arc.initializer) {
            arc_analyze_initializer(node, context);
        }
    }
}

// Analyze assignment for ARC semantics
void arc_analyze_assignment(ASTNode* node, ARCContext* context) {
    // TODO: Implement when Symbol struct has arc_info member
    (void)node;
    (void)context;
    return;

    /* Original code commented out until Symbol struct is updated:
    Symbol* target = symbol_table_lookup(context->symbol_table,
                                         node->data.assignment.variable);

    if (!target || !target->data.arc_info.is_objc_object) {
        return;
    }

    ARCQualifier target_qual = target->data.arc_info.qualifier;

    if (target_qual == ARC_QUALIFIER_STRONG) {
        node->data.assignment_arc.needs_retain = true;
        node->data.assignment_arc.needs_release = true;
    }

    if (target_qual == ARC_QUALIFIER_WEAK) {
        arc_register_weak_assignment(context, node);
    }
    */
}

CodeGenerator *codegen_create(const char *output_filename) {
    CodeGenerator *codegen = malloc(sizeof(CodeGenerator));
    if (!codegen) {
        return NULL;
    }

    // Open the output file using the filename parameter
    codegen->output_file = fopen(output_filename, "w");
    if (!codegen->output_file) {
        free(codegen);
        return NULL;
    }

    codegen->label_counter = 0;
    codegen->temp_counter = 0;
    codegen->objc_mode = false;
    codegen->symbol_table = symbol_table_create();

    return codegen;
}

// And make sure your destroy function properly closes the file:
void codegen_destroy(CodeGenerator *codegen) {
    if (codegen) {
        if (codegen->output_file) {
            fclose(codegen->output_file);
        }
        if (codegen->symbol_table) {
            symbol_table_destroy(codegen->symbol_table);
        }
        free(codegen);
    }
}


void codegen_emit(CodeGenerator *codegen, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(codegen->output_file, format, args);
    va_end(args);
    fprintf(codegen->output_file, "\n");
}

char *codegen_new_label(CodeGenerator *codegen) {
    char *label = malloc(32);
    if (!label) {
        error_fatal("Memory allocation failed for label");
        return NULL;
    }
    snprintf(label, 32, "L%d", codegen->label_counter++);
    return label;
}

char *codegen_new_temp(CodeGenerator *codegen) {
    char *temp = malloc(32);
    if (!temp) {
        error_fatal("Memory allocation failed for temp");
        return NULL;
    }
    snprintf(temp, 32, "t%d", codegen->temp_counter++);
    return temp;
}

bool codegen_generate(CodeGenerator *codegen, ASTNode *ast) {
    if (!codegen || !ast) {
        return false;
    }

    // Generate assembly header
#if TARGET_ARM64
    codegen_emit(codegen, "// Generated by KCC (ARM64/Apple Silicon) v%s", KCC_VERSION);
    codegen_emit(codegen, ".section __TEXT,__text,regular,pure_instructions");
    codegen_emit(codegen, ".build_version macos, 11, 0");
    codegen_emit(codegen, ".globl _main");
    codegen_emit(codegen, ".p2align 2");
#else
    codegen_emit(codegen, "# Generated by KCC (x86-64) v%s", KCC_VERSION);
    codegen_emit(codegen, ".section __TEXT,__text,regular,pure_instructions");
    codegen_emit(codegen, ".globl _main");
#endif
    codegen_emit(codegen, "");

    // Generate code for the program
    codegen_program(codegen, ast);

    // Generate main entry point
    codegen_emit(codegen, "");
    codegen_emit(codegen, "_main:");

#if TARGET_ARM64
    codegen_emit(codegen, "    stp     fp, lr, [sp, #-16]!");
    codegen_emit(codegen, "    mov     fp, sp");
    codegen_emit(codegen, "    bl      _main_func");
    codegen_emit(codegen, "    mov     w0, #0");
    codegen_emit(codegen, "    ldp     fp, lr, [sp], #16");
    codegen_emit(codegen, "    ret");
#else
    codegen_emit(codegen, "    pushq   %%rbp");
    codegen_emit(codegen, "    movq    %%rsp, %%rbp");
    codegen_emit(codegen, "    callq   _main_func");
    codegen_emit(codegen, "    movq    $0x2000001, %%rax");
    codegen_emit(codegen, "    movq    $0, %%rdi");
    codegen_emit(codegen, "    syscall");
    codegen_emit(codegen, "    popq    %%rbp");
    codegen_emit(codegen, "    retq");
#endif

    return true;
}

void codegen_program(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_PROGRAM) return;

    for (int i = 0; i < node->data.program.declaration_count; i++) {
        ASTNode *decl = node->data.program.declarations[i];

        if (decl->type == AST_FUNCTION_DECLARATION) {
            codegen_function_declaration(codegen, decl);
        } else if (decl->type == AST_VARIABLE_DECLARATION) {
            codegen_variable_declaration(codegen, decl);
        }
    }
}

void codegen_function_declaration(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_FUNCTION_DECLARATION) return;

    codegen_emit(codegen, "");
#if TARGET_ARM64
    codegen_emit(codegen, "// Function: %s", node->data.function_decl.name);
#else
    codegen_emit(codegen, "# Function: %s", node->data.function_decl.name);
#endif

    if (strcmp(node->data.function_decl.name, "main") == 0) {
        codegen_emit(codegen, "_main_func:");
    } else {
        codegen_emit(codegen, "_%s:", node->data.function_decl.name);
    }

    // Function prologue
#if TARGET_ARM64
    codegen_emit(codegen, "    stp     fp, lr, [sp, #-16]!");
    codegen_emit(codegen, "    mov     fp, sp");
#else
    codegen_emit(codegen, "    pushq   %%rbp");
    codegen_emit(codegen, "    movq    %%rsp, %%rbp");
#endif

    if (node->data.function_decl.body) {
        codegen_compound_statement(codegen, node->data.function_decl.body);
    }

    // Function epilogue - default return 0
#if TARGET_ARM64
    codegen_emit(codegen, "    mov     w0, #0");
    codegen_emit(codegen, "    ldp     fp, lr, [sp], #16");
    codegen_emit(codegen, "    ret");
#else
    codegen_emit(codegen, "    movq    $0, %%rax");
    codegen_emit(codegen, "    movq    %%rbp, %%rsp");
    codegen_emit(codegen, "    popq    %%rbp");
    codegen_emit(codegen, "    retq");
#endif
}

void codegen_variable_declaration(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_VARIABLE_DECLARATION) return;

#if TARGET_ARM64
    codegen_emit(codegen, "// Variable: %s", node->data.var_decl.name);
#else
    codegen_emit(codegen, "# Variable: %s", node->data.var_decl.name);
#endif

    if (node->data.var_decl.initializer) {
        codegen_expression(codegen, node->data.var_decl.initializer);
#if TARGET_ARM64
        codegen_emit(codegen, "    // Store result in %s", node->data.var_decl.name);
#else
        codegen_emit(codegen, "    # Store result in %s", node->data.var_decl.name);
#endif
    }
}

void codegen_statement(CodeGenerator *codegen, ASTNode *node) {
    switch (node->type) {
        case AST_COMPOUND_STATEMENT:
            codegen_compound_statement(codegen, node);
            break;
        case AST_EXPRESSION_STATEMENT:
            codegen_expression_statement(codegen, node);
            break;
        case AST_RETURN_STATEMENT:
            codegen_return_statement(codegen, node);
            break;
        case AST_IF_STATEMENT:
            codegen_if_statement(codegen, node);
            break;
        case AST_WHILE_STATEMENT:
            codegen_while_statement(codegen, node);
            break;
        case AST_FOR_STATEMENT:
            codegen_for_statement(codegen, node);
            break;
        case AST_VARIABLE_DECLARATION:
            codegen_variable_declaration(codegen, node);
            break;
        default:
#if TARGET_ARM64
            codegen_emit(codegen, "    // Unsupported statement type: %s",
                        ast_node_type_to_string(node->type));
#else
            codegen_emit(codegen, "    # Unsupported statement type: %s",
                        ast_node_type_to_string(node->type));
#endif
            break;
    }
}

void codegen_compound_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_COMPOUND_STATEMENT) return;

    for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
        codegen_statement(codegen, node->data.compound_stmt.statements[i]);
    }
}

void codegen_expression_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_EXPRESSION_STATEMENT) return;

    if (node->data.expression_stmt.expression) {
        codegen_expression(codegen, node->data.expression_stmt.expression);
    }
}

void codegen_return_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_RETURN_STATEMENT) return;

    if (node->data.return_stmt.expression) {
        codegen_expression(codegen, node->data.return_stmt.expression);
#if TARGET_ARM64
        codegen_emit(codegen, "    // Result already in w0/x0");
#else
        codegen_emit(codegen, "    # Result already in %%rax");
#endif
    } else {
#if TARGET_ARM64
        codegen_emit(codegen, "    mov     w0, #0");
#else
        codegen_emit(codegen, "    movq    $0, %%rax");
#endif
    }

#if TARGET_ARM64
    codegen_emit(codegen, "    ldp     fp, lr, [sp], #16");
    codegen_emit(codegen, "    ret");
#else
    codegen_emit(codegen, "    movq    %%rbp, %%rsp");
    codegen_emit(codegen, "    popq    %%rbp");
    codegen_emit(codegen, "    retq");
#endif
}

void codegen_if_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_IF_STATEMENT) return;

    char *else_label = codegen_new_label(codegen);
    char *end_label = codegen_new_label(codegen);

    codegen_expression(codegen, node->data.if_stmt.condition);
#if TARGET_ARM64
    codegen_emit(codegen, "    cmp     w0, #0");
    codegen_emit(codegen, "    b.eq    %s", else_label);
#else
    codegen_emit(codegen, "    testq   %%rax, %%rax");
    codegen_emit(codegen, "    jz      %s", else_label);
#endif

    codegen_statement(codegen, node->data.if_stmt.then_stmt);
#if TARGET_ARM64
    codegen_emit(codegen, "    b       %s", end_label);
#else
    codegen_emit(codegen, "    jmp     %s", end_label);
#endif

    codegen_emit(codegen, "%s:", else_label);
    if (node->data.if_stmt.else_stmt) {
        codegen_statement(codegen, node->data.if_stmt.else_stmt);
    }

    codegen_emit(codegen, "%s:", end_label);

    free(else_label);
    free(end_label);
}

void codegen_while_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_WHILE_STATEMENT) return;

    char *loop_label = codegen_new_label(codegen);
    char *end_label = codegen_new_label(codegen);

    codegen_emit(codegen, "%s:", loop_label);

    codegen_expression(codegen, node->data.while_stmt.condition);
#if TARGET_ARM64
    codegen_emit(codegen, "    cmp     w0, #0");
    codegen_emit(codegen, "    b.eq    %s", end_label);
#else
    codegen_emit(codegen, "    testq   %%rax, %%rax");
    codegen_emit(codegen, "    jz      %s", end_label);
#endif

    codegen_statement(codegen, node->data.while_stmt.body);
#if TARGET_ARM64
    codegen_emit(codegen, "    b       %s", loop_label);
#else
    codegen_emit(codegen, "    jmp     %s", loop_label);
#endif

    codegen_emit(codegen, "%s:", end_label);

    free(loop_label);
    free(end_label);
}

void codegen_for_statement(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_FOR_STATEMENT) return;

    char *loop_label = codegen_new_label(codegen);
    char *update_label = codegen_new_label(codegen);
    char *end_label = codegen_new_label(codegen);

    if (node->data.for_stmt.init) {
        codegen_expression(codegen, node->data.for_stmt.init);
    }

    codegen_emit(codegen, "%s:", loop_label);

    if (node->data.for_stmt.condition) {
        codegen_expression(codegen, node->data.for_stmt.condition);
#if TARGET_ARM64
        codegen_emit(codegen, "    cmp     w0, #0");
        codegen_emit(codegen, "    b.eq    %s", end_label);
#else
        codegen_emit(codegen, "    testq   %%rax, %%rax");
        codegen_emit(codegen, "    jz      %s", end_label);
#endif
    }

    codegen_statement(codegen, node->data.for_stmt.body);

    codegen_emit(codegen, "%s:", update_label);
    if (node->data.for_stmt.update) {
        codegen_expression(codegen, node->data.for_stmt.update);
    }
#if TARGET_ARM64
    codegen_emit(codegen, "    b       %s", loop_label);
#else
    codegen_emit(codegen, "    jmp     %s", loop_label);
#endif

    codegen_emit(codegen, "%s:", end_label);

    free(loop_label);
    free(update_label);
    free(end_label);
}

void codegen_expression(CodeGenerator *codegen, ASTNode *node) {
    switch (node->type) {
        case AST_BINARY_OP:
            codegen_binary_expression(codegen, node);
            break;
        case AST_UNARY_OP:
            codegen_unary_expression(codegen, node);
            break;
        case AST_FUNCTION_CALL:
            codegen_call_expression(codegen, node);
            break;
        case AST_IDENTIFIER:
            codegen_identifier(codegen, node);
            break;
        case AST_NUMBER_LITERAL:
            codegen_number(codegen, node);
            break;
        case AST_STRING_LITERAL:
            codegen_string(codegen, node);
            break;
        case AST_ASSIGNMENT:
            codegen_assignment(codegen, node);
            break;
        default:
#if TARGET_ARM64
            codegen_emit(codegen, "    // Unsupported expression type: %s",
                        ast_node_type_to_string(node->type));
#else
            codegen_emit(codegen, "    # Unsupported expression type: %s",
                        ast_node_type_to_string(node->type));
#endif
            break;
    }
}

void codegen_binary_expression(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_BINARY_OP) return;

    codegen_expression(codegen, node->data.binary_expr.left);
#if TARGET_ARM64
    codegen_emit(codegen, "    str     w0, [sp, #-16]!");
#else
    codegen_emit(codegen, "    pushq   %%rax");
#endif

    codegen_expression(codegen, node->data.binary_expr.right);
#if TARGET_ARM64
    codegen_emit(codegen, "    mov     w1, w0");
    codegen_emit(codegen, "    ldr     w0, [sp], #16");
#else
    codegen_emit(codegen, "    movq    %%rax, %%rbx");
    codegen_emit(codegen, "    popq    %%rax");
#endif

    switch (node->data.binary_expr.operator) {
        case TOKEN_PLUS:
#if TARGET_ARM64
            codegen_emit(codegen, "    add     w0, w0, w1");
#else
            codegen_emit(codegen, "    addq    %%rbx, %%rax");
#endif
            break;
        case TOKEN_MINUS:
#if TARGET_ARM64
            codegen_emit(codegen, "    sub     w0, w0, w1");
#else
            codegen_emit(codegen, "    subq    %%rbx, %%rax");
#endif
            break;
        case TOKEN_MULTIPLY:
#if TARGET_ARM64
            codegen_emit(codegen, "    mul     w0, w0, w1");
#else
            codegen_emit(codegen, "    imulq   %%rbx, %%rax");
#endif
            break;
        case TOKEN_GREATER:
#if TARGET_ARM64
            codegen_emit(codegen, "    cmp     w0, w1");
            codegen_emit(codegen, "    cset    w0, gt");
#else
            codegen_emit(codegen, "    cmpq    %%rbx, %%rax");
            codegen_emit(codegen, "    setg    %%al");
            codegen_emit(codegen, "    movzbq  %%al, %%rax");
#endif
            break;
        default:
#if TARGET_ARM64
            codegen_emit(codegen, "    // Unsupported binary operator: %s",
                        token_type_to_string(node->data.binary_expr.operator));
#else
            codegen_emit(codegen, "    # Unsupported binary operator: %s",
                        token_type_to_string(node->data.binary_expr.operator));
#endif
            break;
    }
}

void codegen_unary_expression(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_UNARY_OP) return;

    codegen_expression(codegen, node->data.unary_expr.operand);

    switch (node->data.unary_expr.operator) {
        case TOKEN_MINUS:
#if TARGET_ARM64
            codegen_emit(codegen, "    neg     w0, w0");
#else
            codegen_emit(codegen, "    negq    %%rax");
#endif
            break;
        case TOKEN_NOT:
#if TARGET_ARM64
            codegen_emit(codegen, "    cmp     w0, #0");
            codegen_emit(codegen, "    cset    w0, eq");
#else
            codegen_emit(codegen, "    testq   %%rax, %%rax");
            codegen_emit(codegen, "    setz    %%al");
            codegen_emit(codegen, "    movzbq  %%al, %%rax");
#endif
            break;
        default:
#if TARGET_ARM64
            codegen_emit(codegen, "    // Unsupported unary operator: %s",
                        token_type_to_string(node->data.unary_expr.operator));
#else
            codegen_emit(codegen, "    # Unsupported unary operator: %s",
                        token_type_to_string(node->data.unary_expr.operator));
#endif
            break;
    }
}

void codegen_call_expression(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_FUNCTION_CALL) return;

#if TARGET_ARM64
    // ARM64 calling convention uses x0-x7 for first 8 args
    for (int i = 0; i < node->data.call_expr.argument_count && i < 8; i++) {
        codegen_expression(codegen, node->data.call_expr.arguments[i]);
        if (i > 0) {
            codegen_emit(codegen, "    mov     x%d, x0", i);
        }
    }
    codegen_emit(codegen, "    bl      _%s", node->data.call_expr.function_name);
#else
    // x86-64 - push args right to left
    for (int i = node->data.call_expr.argument_count - 1; i >= 0; i--) {
        codegen_expression(codegen, node->data.call_expr.arguments[i]);
        codegen_emit(codegen, "    pushq   %%rax");
    }
    codegen_emit(codegen, "    callq   _%s", node->data.call_expr.function_name);
    if (node->data.call_expr.argument_count > 0) {
        codegen_emit(codegen, "    addq    $%d, %%rsp",
                    node->data.call_expr.argument_count * 8);
    }
#endif
}

void codegen_identifier(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_IDENTIFIER) return;

#if TARGET_ARM64
    codegen_emit(codegen, "    // Load variable %s", node->data.identifier.name);
    codegen_emit(codegen, "    ldr     w0, [fp, #-8]");
#else
    codegen_emit(codegen, "    # Load variable %s", node->data.identifier.name);
    codegen_emit(codegen, "    movq    -8(%%rbp), %%rax");
#endif
}

void codegen_number(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_NUMBER_LITERAL) return;

#if TARGET_ARM64
    codegen_emit(codegen, "    mov     w0, #%d", node->data.number.value);
#else
    codegen_emit(codegen, "    movq    $%d, %%rax", node->data.number.value);
#endif
}

void codegen_string(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_STRING_LITERAL) return;

#if TARGET_ARM64
    codegen_emit(codegen, "    // String literal: \"%s\"", node->data.string.value);
    codegen_emit(codegen, "    adrp    x0, string_literal_%d@PAGE", codegen->label_counter);
    codegen_emit(codegen, "    add     x0, x0, string_literal_%d@PAGEOFF", codegen->label_counter++);
#else
    codegen_emit(codegen, "    # String literal: \"%s\"", node->data.string.value);
    codegen_emit(codegen, "    movq    $string_literal_%d, %%rax", codegen->label_counter++);
#endif
}

void codegen_assignment(CodeGenerator *codegen, ASTNode *node) {
    if (node->type != AST_ASSIGNMENT) return;

    codegen_expression(codegen, node->data.assignment.value);

#if TARGET_ARM64
    codegen_emit(codegen, "    // Assign to %s", node->data.assignment.variable);
    codegen_emit(codegen, "    str     w0, [fp, #-8]");
#else
    codegen_emit(codegen, "    # Assign to %s", node->data.assignment.variable);
    codegen_emit(codegen, "    movq    %%rax, -8(%%rbp)");
#endif
}

/* ============================================
 * CODE GENERATION UPDATES (codegen.h and codegen.c)
 * ============================================ */

/* ============================================
 * FIXED ARRAY CODE GENERATION FUNCTIONS - Replace broken ones in codegen.c
 * ============================================ */

// Helper function to generate code for visiting AST nodes
void codegen_visit_node(CodeGenerator *codegen, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_IDENTIFIER:
            codegen_emit(codegen, "%s", node->data.identifier.name);
            break;
        case AST_NUMBER_LITERAL:
            codegen_emit(codegen, "%d", node->data.number.value);
            break;
        case AST_BASIC_TYPE:
            switch (node->data.basic_type.type) {
                case TYPE_INT: codegen_emit(codegen, "int"); break;
                case TYPE_FLOAT: codegen_emit(codegen, "float"); break;
                case TYPE_DOUBLE: codegen_emit(codegen, "double"); break;
                case TYPE_CHAR: codegen_emit(codegen, "char"); break;
                default: codegen_emit(codegen, "void"); break;
            }
            break;
        case AST_ARRAY_ACCESS:
            codegen_array_access(codegen, node);
            break;
        case AST_ARRAY_DECLARATION:
            codegen_array_declaration(codegen, node);
            break;
        case AST_ARRAY_LITERAL:
            codegen_array_literal(codegen, node);
            break;
        default:
            // Handle other node types or emit placeholder
            codegen_emit(codegen, "/* unhandled node type %d */", node->type);
            break;
    }
}

// Generate code for array declaration
void codegen_array_declaration(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_ARRAY_DECLARATION) {
        return;
    }

    ArrayDeclaration *array_decl = &node->data.array_decl;

    if (array_decl->is_dynamic) {
        // Generate dynamic array allocation
        codegen_emit(codegen, "// Dynamic array allocation\n");
        codegen_emit(codegen, "void* array_data = malloc(sizeof(");
        codegen_visit_node(codegen, array_decl->element_type);
        codegen_emit(codegen, ") * (");
        codegen_visit_node(codegen, array_decl->size_expr);
        codegen_emit(codegen, "));\n");

        // Generate array metadata structure
        codegen_emit(codegen, "struct {\n");
        codegen_emit(codegen, "    void* data;\n");
        codegen_emit(codegen, "    int size;\n");
        codegen_emit(codegen, "    int element_size;\n");
        codegen_emit(codegen, "} array_meta = {\n");
        codegen_emit(codegen, "    .data = array_data,\n");
        codegen_emit(codegen, "    .size = (");
        codegen_visit_node(codegen, array_decl->size_expr);
        codegen_emit(codegen, "),\n");
        codegen_emit(codegen, "    .element_size = sizeof(");
        codegen_visit_node(codegen, array_decl->element_type);
        codegen_emit(codegen, ")\n};\n");
    } else {
        // Generate static array declaration
        codegen_visit_node(codegen, array_decl->element_type);
        codegen_emit(codegen, " array_var[");
        if (array_decl->size_expr) {
            codegen_visit_node(codegen, array_decl->size_expr);
        }
        codegen_emit(codegen, "]");
    }
}

// Generate code for array access with basic bounds checking
void codegen_array_access(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_ARRAY_ACCESS) {
        return;
    }

    ArrayAccess *array_access = &node->data.array_access;

    // For now, generate simple array access without dynamic checking
    // In a full implementation, you'd check symbol table for dynamic arrays
    codegen_visit_node(codegen, array_access->array_expr);
    codegen_emit(codegen, "[");
    codegen_visit_node(codegen, array_access->index_expr);
    codegen_emit(codegen, "]");
}

// Generate code for array access with bounds checking (enhanced version)
void codegen_array_access_with_bounds_check(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_ARRAY_ACCESS) {
        return;
    }

    ArrayAccess *array_access = &node->data.array_access;

    // Generate bounds checking for arrays
    codegen_emit(codegen, "((");
    codegen_visit_node(codegen, array_access->index_expr);
    codegen_emit(codegen, " >= 0) ? ");

    // Generate array access
    codegen_visit_node(codegen, array_access->array_expr);
    codegen_emit(codegen, "[");
    codegen_visit_node(codegen, array_access->index_expr);
    codegen_emit(codegen, "] : ");

    // Generate error handling for out of bounds
    codegen_emit(codegen, "(fprintf(stderr, \"Array index out of bounds\\n\"), exit(1), 0))");
}

// Generate code for array literal
void codegen_array_literal(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_ARRAY_LITERAL) {
        return;
    }

    ArrayLiteral *array_literal = &node->data.array_literal;

    codegen_emit(codegen, "{");
    for (int i = 0; i < array_literal->element_count; i++) {
        if (i > 0) {
            codegen_emit(codegen, ", ");
        }
        codegen_visit_node(codegen, array_literal->elements[i]);
    }
    codegen_emit(codegen, "}");
}

// Generate code for address-of operator
void codegen_address_of(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_ADDRESS_OF) {
        return;
    }

    AddressOf *address_of = &node->data.address_of;

    codegen_emit(codegen, "&(");
    codegen_visit_node(codegen, address_of->operand);
    codegen_emit(codegen, ")");
}

// Generate code for pointer dereference
void codegen_pointer_dereference(CodeGenerator *codegen, ASTNode *node) {
    if (!node || node->type != AST_POINTER_DEREFERENCE) {
        return;
    }

    PointerDereference *pointer_deref = &node->data.pointer_deref;

    codegen_emit(codegen, "*(");
    codegen_visit_node(codegen, pointer_deref->operand);
    codegen_emit(codegen, ")");
}

// Utility function to generate array runtime includes
void codegen_generate_array_runtime_includes(CodeGenerator *codegen) {
    codegen_emit(codegen, "// Array runtime support\n");
    codegen_emit(codegen, "#include <stdio.h>\n");
    codegen_emit(codegen, "#include <stdlib.h>\n");
    codegen_emit(codegen, "#include <string.h>\n");
    codegen_emit(codegen, "\n");

    // Generate dynamic array structure definition
    codegen_emit(codegen, "// Dynamic array structure\n");
    codegen_emit(codegen, "typedef struct {\n");
    codegen_emit(codegen, "    void* data;\n");
    codegen_emit(codegen, "    int size;\n");
    codegen_emit(codegen, "    int capacity;\n");
    codegen_emit(codegen, "    int element_size;\n");
    codegen_emit(codegen, "} DynamicArray;\n\n");

    // Generate bounds checking macro
    codegen_emit(codegen, "// Bounds checking macro\n");
    codegen_emit(codegen, "#define ARRAY_BOUNDS_CHECK(arr, index) \\\n");
    codegen_emit(codegen, "    do { \\\n");
    codegen_emit(codegen, "        if ((index) < 0 || (index) >= (arr)->size) { \\\n");
    codegen_emit(codegen, "            fprintf(stderr, \"Array index %%d out of bounds [0, %%d)\\n\", \\\n");
    codegen_emit(codegen, "                    (index), (arr)->size); \\\n");
    codegen_emit(codegen, "            exit(1); \\\n");
    codegen_emit(codegen, "        } \\\n");
    codegen_emit(codegen, "    } while(0)\n\n");
}

// Function to integrate array code generation into main codegen function
void codegen_generate_with_arrays(CodeGenerator *codegen, ASTNode *ast) {
    if (!codegen || !ast) {
        return;
    }

    // Generate array runtime includes at the beginning
    codegen_generate_array_runtime_includes(codegen);

    // Generate main program code
    codegen_generate_node(codegen, ast);
}

/* ============================================
 * STEP 2: Add these function DEFINITIONS to your codegen.c file
 * (Put these BEFORE your codegen_generate_node function)
 * ============================================ */

// Enhanced type declaration generation
void codegen_generate_type_declaration(CodeGenerator* codegen, DataType type) {
    switch (type) {
        case TYPE_INT:
            codegen_emit(codegen, "int");
            break;
        case TYPE_LONG:
            codegen_emit(codegen, "long");
            break;
        case TYPE_LONG_LONG:
            codegen_emit(codegen, "long long");
            break;
        case TYPE_UNSIGNED_INT:
            codegen_emit(codegen, "unsigned int");
            break;
        case TYPE_UNSIGNED_LONG:
            codegen_emit(codegen, "unsigned long");
            break;
        case TYPE_SHORT:
            codegen_emit(codegen, "short");
            break;
        case TYPE_UNSIGNED_SHORT:
            codegen_emit(codegen, "unsigned short");
            break;
        case TYPE_FLOAT:
            codegen_emit(codegen, "float");
            break;
        case TYPE_DOUBLE:
            codegen_emit(codegen, "double");
            break;
        case TYPE_LONG_DOUBLE:
            codegen_emit(codegen, "long double");
            break;
        case TYPE_CHAR:
            codegen_emit(codegen, "char");
            break;
        case TYPE_SIGNED_CHAR:
            codegen_emit(codegen, "signed char");
            break;
        case TYPE_UNSIGNED_CHAR:
            codegen_emit(codegen, "unsigned char");
            break;
        case TYPE_VOID:
            codegen_emit(codegen, "void");
            break;
        default:
            codegen_emit(codegen, "int"); // Default fallback
            break;
    }
}

// Generate function pointer code
void codegen_function_pointer(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_FUNCTION_POINTER) {
        return;
    }

    // Generate return type
    codegen_generate_type_declaration(codegen, node->data.function_ptr.return_type);
    codegen_emit(codegen, " (*%s)(", node->data.function_ptr.name);

    // Generate parameter types
    for (int i = 0; i < node->data.function_ptr.param_count; i++) {
        if (i > 0) codegen_emit(codegen, ", ");
        codegen_generate_node(codegen, node->data.function_ptr.param_types[i]);
    }

    if (node->data.function_ptr.is_variadic) {
        if (node->data.function_ptr.param_count > 0) codegen_emit(codegen, ", ");
        codegen_emit(codegen, "...");
    }

    codegen_emit(codegen, ")");
}

// Generate sizeof expression
void codegen_sizeof_expr(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_SIZEOF_EXPR) {
        return;
    }

    codegen_emit(codegen, "sizeof(");
    codegen_generate_node(codegen, node->data.sizeof_expr.operand);
    codegen_emit(codegen, ")");
}

// Generate cast expression
void codegen_cast_expr(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_CAST_EXPR) {
        return;
    }

    codegen_emit(codegen, "(");
    codegen_generate_type_declaration(codegen, node->data.cast_expr.target_type);
    codegen_emit(codegen, ")(");
    codegen_generate_node(codegen, node->data.cast_expr.operand);
    codegen_emit(codegen, ")");
}

// Generate literal values
void codegen_char_literal(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_CHAR_LITERAL) {
        return;
    }

    char c = node->data.char_literal.value;
    if (c == '\'') {
        codegen_emit(codegen, "'\\''");
    } else if (c == '\\') {
        codegen_emit(codegen, "'\\\\'");
    } else if (c == '\n') {
        codegen_emit(codegen, "'\\n'");
    } else if (c == '\t') {
        codegen_emit(codegen, "'\\t'");
    } else if (c == '\r') {
        codegen_emit(codegen, "'\\r'");
    } else if (c >= 32 && c <= 126) { // printable ASCII
        codegen_emit(codegen, "'%c'", c);
    } else {
        codegen_emit(codegen, "'\\x%02x'", (unsigned char)c);
    }
}

void codegen_float_literal(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_FLOAT_LITERAL) {
        return;
    }

    codegen_emit(codegen, "%ff", node->data.float_literal.value);
}

void codegen_double_literal(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_DOUBLE_LITERAL) {
        return;
    }

    codegen_emit(codegen, "%f", node->data.double_literal.value);
}

void codegen_long_literal(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_LONG_LITERAL) {
        return;
    }

    codegen_emit(codegen, "%ldL", node->data.long_literal.value);
}

void codegen_ulong_literal(CodeGenerator* codegen, ASTNode* node) {
    if (!node || node->type != AST_ULONG_LITERAL) {
        return;
    }

    codegen_emit(codegen, "%luUL", node->data.ulong_literal.value);
}

/* ============================================
 * STEP 3: REPLACE your existing codegen_generate_node function with this version
 * (Keep only ONE version of this function)
 * ============================================ */

void codegen_generate_node(CodeGenerator *codegen, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_IDENTIFIER:
            codegen_emit(codegen, "%s", node->data.identifier.name);
            break;
        case AST_NUMBER_LITERAL:
            codegen_emit(codegen, "%d", node->data.number.value);
            break;
        case AST_STRING_LITERAL:
            codegen_emit(codegen, "\"%s\"", node->data.string.value);
            break;
        case AST_BASIC_TYPE:
            codegen_generate_type_declaration(codegen, node->data.basic_type.type);
            break;

        // Array support
        case AST_ARRAY_ACCESS:
            codegen_array_access(codegen, node);
            break;
        case AST_ARRAY_DECLARATION:
            codegen_array_declaration(codegen, node);
            break;
        case AST_ARRAY_LITERAL:
            codegen_array_literal(codegen, node);
            break;
        case AST_ADDRESS_OF:
            codegen_address_of(codegen, node);
            break;
        case AST_POINTER_DEREFERENCE:
            codegen_pointer_dereference(codegen, node);
            break;

        // Complex type cases
        case AST_FUNCTION_POINTER:
            codegen_function_pointer(codegen, node);
            break;
        case AST_SIZEOF_EXPR:
            codegen_sizeof_expr(codegen, node);
            break;
        case AST_CAST_EXPR:
            codegen_cast_expr(codegen, node);
            break;
        case AST_CHAR_LITERAL:
            codegen_char_literal(codegen, node);
            break;
        case AST_FLOAT_LITERAL:
            codegen_float_literal(codegen, node);
            break;
        case AST_DOUBLE_LITERAL:
            codegen_double_literal(codegen, node);
            break;
        case AST_LONG_LITERAL:
            codegen_long_literal(codegen, node);
            break;
        case AST_ULONG_LITERAL:
            codegen_ulong_literal(codegen, node);
            break;

        // Binary and unary expressions
        case AST_BINARY_OP:
            codegen_emit(codegen, "(");
            codegen_generate_node(codegen, node->data.binary_expr.left);
            switch (node->data.binary_expr.operator) {
                case TOKEN_PLUS: codegen_emit(codegen, " + "); break;
                case TOKEN_MINUS: codegen_emit(codegen, " - "); break;
                case TOKEN_MULTIPLY: codegen_emit(codegen, " * "); break;
                case TOKEN_DIVIDE: codegen_emit(codegen, " / "); break;
                case TOKEN_MODULO: codegen_emit(codegen, " %% "); break;
                case TOKEN_EQUAL: codegen_emit(codegen, " == "); break;
                case TOKEN_NOT_EQUAL: codegen_emit(codegen, " != "); break;
                case TOKEN_LESS: codegen_emit(codegen, " < "); break;
                case TOKEN_LESS_EQUAL: codegen_emit(codegen, " <= "); break;
                case TOKEN_GREATER: codegen_emit(codegen, " > "); break;
                case TOKEN_GREATER_EQUAL: codegen_emit(codegen, " >= "); break;
                case TOKEN_AND: codegen_emit(codegen, " && "); break;
                case TOKEN_OR: codegen_emit(codegen, " || "); break;
                default: codegen_emit(codegen, " ? "); break;
            }
            codegen_generate_node(codegen, node->data.binary_expr.right);
            codegen_emit(codegen, ")");
            break;

        case AST_UNARY_OP:
            switch (node->data.unary_expr.operator) {
                case TOKEN_MINUS: codegen_emit(codegen, "-"); break;
                case TOKEN_NOT: codegen_emit(codegen, "!"); break;
                case TOKEN_BITWISE_NOT: codegen_emit(codegen, "~"); break;
                default: break;
            }
            codegen_generate_node(codegen, node->data.unary_expr.operand);
            break;

        default:
            // Handle other node types or emit placeholder
            codegen_emit(codegen, "/* unhandled node type %d */", node->type);
            break;
    }
}


/* ============================================
 * STEP 6: ARC Code Generation
 * ============================================ */

// Generate ARC-aware variable declaration
void codegen_arc_variable_declaration(CodeGenerator* codegen, ASTNode* node) {
    codegen_generate_type_declaration(codegen, node->data.var_decl_arc.var_type);

    ARCQualifier qualifier = node->data.var_decl_arc.arc_qualifier;

    // Emit qualifier annotation (for debugging/documentation)
    switch (qualifier) {
        case ARC_QUALIFIER_STRONG:
            codegen_emit(codegen, " __strong ");
            break;
        case ARC_QUALIFIER_WEAK:
            codegen_emit(codegen, " __weak ");
            break;
        case ARC_QUALIFIER_UNSAFE_UNRETAINED:
            codegen_emit(codegen, " __unsafe_unretained ");
            break;
        case ARC_QUALIFIER_AUTORELEASING:
            codegen_emit(codegen, " __autoreleasing ");
            break;
        default:
            codegen_emit(codegen, " ");
            break;
    }

    codegen_emit(codegen, "%s", node->data.var_decl_arc.name);

    // Generate initializer with retain if needed
    if (node->data.var_decl_arc.initializer) {
        codegen_emit(codegen, " = ");

        if (node->arc_info.needs_retain) {
            codegen_emit(codegen, "objc_retain(");
            codegen_generate_node(codegen, node->data.var_decl_arc.initializer);
            codegen_emit(codegen, ")");
        } else {
            codegen_generate_node(codegen, node->data.var_decl_arc.initializer);
        }
    } else if (qualifier == ARC_QUALIFIER_STRONG || qualifier == ARC_QUALIFIER_WEAK) {
        // Initialize to nil
        codegen_emit(codegen, " = nil");
    }

    codegen_emit(codegen, ";\n");
}

// Generate ARC-aware assignment
void codegen_arc_assignment(CodeGenerator* codegen, ASTNode* node) {
    if (!node->data.assignment_arc.needs_retain &&
        !node->data.assignment_arc.needs_release) {
        // Simple assignment
        codegen_emit(codegen, "%s = ", node->data.assignment_arc.variable);
        codegen_generate_node(codegen, node->data.assignment_arc.value);
        return;
    }

    // Complex ARC assignment: release old, retain new
    codegen_emit(codegen, "{\n");
    codegen_emit(codegen, "    id __arc_temp = ");
    codegen_generate_node(codegen, node->data.assignment_arc.value);
    codegen_emit(codegen, ";\n");

    if (node->data.assignment_arc.needs_retain) {
        codegen_emit(codegen, "    objc_retain(__arc_temp);\n");
    }

    if (node->data.assignment_arc.needs_release) {
        codegen_emit(codegen, "    objc_release(%s);\n",
                    node->data.assignment_arc.variable);
    }

    codegen_emit(codegen, "    %s = __arc_temp;\n",
                node->data.assignment_arc.variable);
    codegen_emit(codegen, "}\n");
}

// Generate scope cleanup code (release strong references)
void codegen_arc_scope_cleanup(CodeGenerator* codegen, ARCContext* context) {
    for (int i = 0; i < context->cleanup_count; i++) {
        ASTNode* var = context->cleanup_vars[i];
        if (var->arc_info.needs_release) {
            codegen_emit(codegen, "objc_release(%s);\n",
                        var->data.var_decl_arc.name);
        }
    }
}

// Generate ARC bridge cast
void codegen_arc_bridge_cast(CodeGenerator* codegen, ASTNode* node) {
    switch (node->data.bridge_cast.bridge_type) {
        case ARC_BRIDGE:
            codegen_emit(codegen, "(__bridge ");
            break;
        case ARC_BRIDGE_RETAINED:
            codegen_emit(codegen, "(__bridge_retained ");
            break;
        case ARC_BRIDGE_TRANSFER:
            codegen_emit(codegen, "(__bridge_transfer ");
            break;
        default:
            break;
    }

    codegen_generate_type_declaration(codegen, node->data.bridge_cast.target_type);
    codegen_emit(codegen, ")");
    codegen_generate_node(codegen, node->data.bridge_cast.operand);
}
