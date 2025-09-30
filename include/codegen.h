#ifndef CODEGEN_H
#define CODEGEN_H

#include "types.h"
#include <stdio.h>
#include <stdbool.h>

// Code generator lifecycle functions
CodeGenerator *codegen_create(const char *output_filename);
void codegen_destroy(CodeGenerator *codegen);

// Main code generation functions
bool codegen_generate(CodeGenerator *codegen, ASTNode *ast);
void codegen_emit(CodeGenerator *codegen, const char *format, ...);
char *codegen_new_label(CodeGenerator *codegen);
char *codegen_new_temp(CodeGenerator *codegen);

// Code generation for different AST nodes
void codegen_program(CodeGenerator *codegen, ASTNode *node);
void codegen_function_declaration(CodeGenerator *codegen, ASTNode *node);
void codegen_variable_declaration(CodeGenerator *codegen, ASTNode *node);
void codegen_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_compound_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_expression_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_return_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_if_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_while_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_for_statement(CodeGenerator *codegen, ASTNode *node);
void codegen_expression(CodeGenerator *codegen, ASTNode *node);
void codegen_binary_expression(CodeGenerator *codegen, ASTNode *node);
void codegen_unary_expression(CodeGenerator *codegen, ASTNode *node);
void codegen_call_expression(CodeGenerator *codegen, ASTNode *node);
void codegen_identifier(CodeGenerator *codegen, ASTNode *node);
void codegen_number(CodeGenerator *codegen, ASTNode *node);
void codegen_string(CodeGenerator *codegen, ASTNode *node);
void codegen_assignment(CodeGenerator *codegen, ASTNode *node);

// Array code generation functions
void codegen_array_declaration(CodeGenerator *codegen, ASTNode *node);
void codegen_array_access(CodeGenerator *codegen, ASTNode *node);
void codegen_array_literal(CodeGenerator *codegen, ASTNode *node);
void codegen_address_of(CodeGenerator *codegen, ASTNode *node);
void codegen_pointer_dereference(CodeGenerator *codegen, ASTNode *node);
void codegen_generate_array_runtime_includes(CodeGenerator *codegen);
void codegen_generate_with_arrays(CodeGenerator *codegen, ASTNode *ast);

// Add these to codegen.h (before any existing declarations)
void codegen_generate_type_declaration(CodeGenerator* codegen, DataType type);
void codegen_function_pointer(CodeGenerator* codegen, ASTNode* node);
void codegen_sizeof_expr(CodeGenerator* codegen, ASTNode* node);
void codegen_cast_expr(CodeGenerator* codegen, ASTNode* node);
void codegen_char_literal(CodeGenerator* codegen, ASTNode* node);
void codegen_float_literal(CodeGenerator* codegen, ASTNode* node);
void codegen_double_literal(CodeGenerator* codegen, ASTNode* node);
void codegen_long_literal(CodeGenerator* codegen, ASTNode* node);
void codegen_ulong_literal(CodeGenerator* codegen, ASTNode* node);


#endif // CODEGEN_H