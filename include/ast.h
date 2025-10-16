#ifndef AST_H
#define AST_H

#include "types.h"

// Function declarations for array AST nodes
ASTNode* create_array_declaration(ASTNode* element_type, ASTNode* size_expr,
                                 int is_dynamic, int line, int column);
ASTNode* create_multidim_array_declaration(ASTNode* element_type,
                                         ASTNode** dimensions, int dim_count,
                                         int line, int column);
ASTNode* create_array_access(ASTNode* array_expr, ASTNode* index_expr,
                           int line, int column);
ASTNode* create_array_literal(ASTNode** elements, int element_count,
                            int line, int column);
ASTNode* create_address_of(ASTNode* operand, int line, int column);
ASTNode* create_pointer_dereference(ASTNode* operand, int line, int column);


// AST creation functions
ASTNode *ast_create_program(void);
ASTNode *ast_create_function_decl(DataType return_type, const char *name, ASTNode **params, ASTNode *body);
ASTNode *ast_create_var_decl(DataType var_type, const char *name, ASTNode *initializer);
ASTNode *ast_create_parameter(DataType param_type, const char *name);
ASTNode *ast_create_compound_stmt(void);
ASTNode *ast_create_expression_stmt(ASTNode *expression);
ASTNode *ast_create_return_stmt(ASTNode *expression);
ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_stmt, ASTNode *else_stmt);
ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body);
ASTNode *ast_create_for_stmt(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body);
ASTNode *ast_create_break_stmt(void);
ASTNode *ast_create_continue_stmt(void);
ASTNode *ast_create_binary_expr(TokenType operator, ASTNode *left, ASTNode *right);
ASTNode *ast_create_unary_expr(TokenType operator, ASTNode *operand);
ASTNode *ast_create_call_expr(const char *function_name);
ASTNode *ast_create_identifier(const char *name);
ASTNode *ast_create_number(int value);
ASTNode *ast_create_string(const char *value);
ASTNode *ast_create_assignment(const char *variable, ASTNode *value);

// AST manipulation functions
void ast_add_declaration(ASTNode *program, ASTNode *declaration);
void ast_add_parameter(ASTNode *function, ASTNode *parameter);
void ast_add_statement(ASTNode *compound_stmt, ASTNode *statement);
void ast_add_argument(ASTNode *call_expr, ASTNode *argument);

// AST utility functions
void ast_destroy(ASTNode *node);
// In the ast.c header file
void ast_print(ASTNode *node, int indent);
const char *ast_node_type_to_string(ASTNodeType type);
const char *data_type_to_string(DataType type);
DataType token_to_data_type(TokenType token);

// Prototypes for new AST node creation functions
ASTNode *ast_create_typedef(ASTNode *base_type, const char *alias_name);
ASTNode *ast_create_struct(const char *name);
ASTNode *ast_create_union(const char *name);
ASTNode *ast_create_enum(const char *name);
ASTNode *ast_create_enum_constant(const char *name, int value);
ASTNode *ast_create_struct_member(DataType type, const char *name, int bitfield_width);
ASTNode *ast_create_var_decl_with_type_node(ASTNode *type_node, const char *name);
ASTNode *ast_create_basic_type(DataType type);

// Add these to ast.h
ASTNode *ast_create_typedef(ASTNode *base_type, const char *alias_name);
ASTNode *ast_create_struct(const char *name);
ASTNode *ast_create_union(const char *name);
ASTNode *ast_create_enum(const char *name);
ASTNode *ast_create_enum_constant(const char *name, int value);
ASTNode *ast_create_struct_member(DataType type, const char *name, int bitfield_width);
ASTNode *ast_create_basic_type(DataType type);
ASTNode *ast_create_var_decl_with_type_node(ASTNode *type_node, const char *name);

void ast_add_struct_member(ASTNode *struct_node, ASTNode *member);
void ast_add_union_member(ASTNode *union_node, ASTNode *member);
void ast_add_enum_constant(ASTNode *enum_node, ASTNode *constant);
void ast_set_member_type_node(ASTNode *member, ASTNode *type_node);

// Add these to your ast.h file
ASTNode* ast_create_array_declaration(ASTNode* element_type, ASTNode* size_expr, int is_dynamic, int line, int column);
ASTNode* ast_create_multidim_array_declaration(ASTNode* element_type, ASTNode** dimensions, int dim_count, int line, int column);
ASTNode* ast_create_array_access(ASTNode* array_expr, ASTNode* index_expr, int line, int column);
ASTNode* ast_create_array_literal(ASTNode** elements, int element_count, int line, int column);
ASTNode* ast_create_address_of(ASTNode* operand, int line, int column);
ASTNode* ast_create_pointer_dereference(ASTNode* operand, int line, int column);

// Add these declarations to ast.h
ASTNode* ast_create_function_pointer(DataType return_type, const char* name,
                                    ASTNode** param_types, int param_count);
ASTNode* ast_create_sizeof_expr(ASTNode* operand);
ASTNode* ast_create_cast_expr(DataType target_type, ASTNode* operand);
ASTNode* ast_create_char_literal(char value);
ASTNode* ast_create_float_literal(float value);
ASTNode* ast_create_double_literal(double value);
ASTNode* ast_create_long_literal(long value);

// Add enhanced type checking
bool parser_is_type_specifier_extended(TokenType type);

ASTNode *ast_create_switch_stmt(ASTNode *expression);
ASTNode *ast_create_case_stmt(ASTNode *value, bool is_default);
void ast_add_case_to_switch(ASTNode *switch_node, ASTNode *case_node);
void ast_add_statement_to_case(ASTNode *case_node, ASTNode *stmt);

// Add these to ast.h
ASTNode* ast_create_arc_var_decl(DataType type, const char* name,
                                 ASTNode* initializer, ARCQualifier qualifier);
ASTNode* ast_create_bridge_cast(ARCBridgeCast bridge_type, ASTNode* operand,
                                DataType target_type);
#endif // AST_H