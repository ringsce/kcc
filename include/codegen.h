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

#endif // CODEGEN_H