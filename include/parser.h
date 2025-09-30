#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "lexer.h"

// Core parser functions
Parser *parser_create(Lexer *lexer);
void parser_destroy(Parser *parser);
void parser_advance(Parser *parser);
bool parser_match(Parser *parser, TokenType type);
bool parser_expect(Parser *parser, TokenType type);

// Type checking and parsing utilities
bool parser_is_type_specifier(TokenType type);
bool parser_is_objc_directive(TokenType type);
DataType parser_parse_type_specifier(Parser *parser);
DataType token_type_to_data_type(TokenType type);

// Core parsing functions
ASTNode *parser_parse_program(Parser *parser);
ASTNode *parser_parse_declaration(Parser *parser);
ASTNode *parser_parse_function(Parser *parser, DataType return_type, char *name);
ASTNode *parser_parse_function_definition(Parser *parser, DataType return_type, char *name);
ASTNode *parser_parse_variable_declaration(Parser *parser, DataType var_type);

// Helper functions for AST creation
ASTNode *parser_create_function_declaration(DataType return_type, const char *name);
ASTNode *parser_create_variable_declaration(DataType type, const char *name);

// Statement parsing
ASTNode *parser_parse_statement(Parser *parser);
ASTNode *parser_parse_compound_statement(Parser *parser);
ASTNode *parser_parse_expression_statement(Parser *parser);
ASTNode *parser_parse_return_statement(Parser *parser);
ASTNode *parser_parse_if_statement(Parser *parser);
ASTNode *parser_parse_while_statement(Parser *parser);
ASTNode *parser_parse_for_statement(Parser *parser);

// Expression parsing
ASTNode *parser_parse_expression(Parser *parser);
ASTNode *parser_parse_assignment_expression(Parser *parser);
ASTNode *parser_parse_logical_or_expression(Parser *parser);
ASTNode *parser_parse_logical_and_expression(Parser *parser);
ASTNode *parser_parse_equality_expression(Parser *parser);
ASTNode *parser_parse_relational_expression(Parser *parser);
ASTNode *parser_parse_additive_expression(Parser *parser);
ASTNode *parser_parse_multiplicative_expression(Parser *parser);
ASTNode *parser_parse_unary_expression(Parser *parser);
ASTNode *parser_parse_primary_expression(Parser *parser);
ASTNode *parser_parse_call_expression(Parser *parser, ASTNode *primary);

// Objective-C parsing functions
ASTNode *parser_parse_objc_declaration(Parser *parser);

// Objective-C interface and implementation
ASTNode *parser_parse_objc_interface(Parser *parser);
ASTNode *parser_parse_objc_implementation(Parser *parser);
ASTNode *parser_parse_objc_protocol(Parser *parser);

// Objective-C method parsing
ASTNode *parser_parse_objc_method_declaration(Parser *parser);
ASTNode *parser_parse_objc_method_implementation(Parser *parser);
char *parser_parse_objc_method_selector(Parser *parser);

// Objective-C property and synthesis
ASTNode *parser_parse_objc_property(Parser *parser);
ASTNode *parser_parse_objc_synthesize(Parser *parser);
ASTNode *parser_parse_objc_dynamic(Parser *parser);
ASTNode *parser_parse_objc_class_forward(Parser *parser);

// Objective-C expressions
ASTNode *parser_parse_objc_message_send(Parser *parser);
ASTNode *parser_parse_objc_selector_expression(Parser *parser);
ASTNode *parser_parse_objc_encode_expression(Parser *parser);
ASTNode *parser_parse_property_access(Parser *parser, ASTNode *object);

// Objective-C statements
ASTNode *parser_parse_objc_try_statement(Parser *parser);
ASTNode *parser_parse_objc_throw_statement(Parser *parser);
ASTNode *parser_parse_objc_synchronized_statement(Parser *parser);
ASTNode *parser_parse_objc_autoreleasepool_statement(Parser *parser);

// AST creation functions that need to be implemented in ast.c
// These are declarations for functions you'll need to implement

// Basic AST creators (should already exist)
ASTNode *ast_create_program(void);
ASTNode *ast_create_function_decl(DataType return_type, const char *name, ASTNode **params, ASTNode *body);
ASTNode *ast_create_var_decl(DataType type, const char *name, ASTNode *initializer);
ASTNode *ast_create_compound_stmt(void);
ASTNode *ast_create_expression_stmt(ASTNode *expr);
ASTNode *ast_create_return_stmt(ASTNode *expr);
ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_stmt, ASTNode *else_stmt);
ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body);
ASTNode *ast_create_for_stmt(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body);
ASTNode *ast_create_break_stmt(void);
ASTNode *ast_create_continue_stmt(void);
ASTNode *ast_create_binary_expr(TokenType op, ASTNode *left, ASTNode *right);
ASTNode *ast_create_unary_expr(TokenType op, ASTNode *operand);
ASTNode *ast_create_assignment(const char *variable, ASTNode *value);
ASTNode *ast_create_call_expr(const char *function_name);
ASTNode *ast_create_identifier(const char *name);
ASTNode *ast_create_number(int value);
ASTNode *ast_create_string(const char *value);

// Add these to parser.h
ASTNode *parser_parse_assignment_expression(Parser *parser);
ASTNode *parser_parse_multiplicative_expression(Parser *parser);
ASTNode *parser_parse_unary_expression(Parser *parser);
ASTNode *parser_parse_while_statement(Parser *parser);
ASTNode *parser_parse_for_statement(Parser *parser);
ASTNode *parser_parse_call_expression(Parser *parser, ASTNode *primary);

// enums
// Add these to parser.h
bool parser_is_type_specifier_extended(TokenType type);
ASTNode *parser_parse_typedef(Parser *parser);
ASTNode *parser_parse_struct(Parser *parser);
ASTNode *parser_parse_union(Parser *parser);
ASTNode *parser_parse_enum(Parser *parser);
ASTNode *parser_parse_struct_member(Parser *parser);
ASTNode *parser_parse_type_declaration(Parser *parser);
ASTNode *parser_parse_declaration_extended(Parser *parser);
// Objective-C AST creators (need to be implemented)
ASTNode *ast_create_objc_interface(const char *class_name, const char *superclass_name);
ASTNode *ast_create_objc_implementation(const char *class_name, const char *category_name);
ASTNode *ast_create_objc_protocol(const char *protocol_name);
ASTNode *ast_create_objc_method(ObjCMethodType method_type, DataType return_type, const char *selector, ASTNode *body);
ASTNode *ast_create_objc_property(DataType property_type, const char *property_name, ObjCPropertyAttributes attributes);
ASTNode *ast_create_objc_synthesize(const char *property_name, const char *ivar_name);
ASTNode *ast_create_objc_dynamic(const char *property_name);
ASTNode *ast_create_objc_message_send(ASTNode *receiver, const char *selector);
ASTNode *ast_create_objc_string(const char *value);
ASTNode *ast_create_objc_boolean(bool value);
ASTNode *ast_create_objc_nil(void);
ASTNode *ast_create_objc_self(void);
ASTNode *ast_create_objc_super(void);
ASTNode *ast_create_objc_selector(const char *selector_name);
ASTNode *ast_create_objc_encode(DataType type);
ASTNode *ast_create_objc_try(ASTNode *try_body, ASTNode **catch_blocks, int catch_count, ASTNode *finally_block);
ASTNode *ast_create_objc_catch(DataType exception_type, const char *exception_var, ASTNode *catch_body);
ASTNode *ast_create_objc_throw(ASTNode *exception_expr);
ASTNode *ast_create_objc_synchronized(ASTNode *sync_object, ASTNode *sync_body);
ASTNode *ast_create_objc_autoreleasepool(ASTNode *pool_body);
ASTNode *ast_create_property_access(ASTNode *object, const char *property_name);

// AST manipulation functions (should already exist or need to be implemented)
void ast_add_declaration(ASTNode *program, ASTNode *declaration);
void ast_add_statement(ASTNode *compound, ASTNode *statement);
void ast_add_argument(ASTNode *call, ASTNode *argument);
void ast_add_objc_interface_member(ASTNode *interface, ASTNode *member);
void ast_add_objc_implementation_member(ASTNode *implementation, ASTNode *member);
void ast_add_objc_protocol_method(ASTNode *protocol, ASTNode *method);
void ast_add_objc_protocol_property(ASTNode *protocol, ASTNode *property);

/* ============================================
 * STEP 1: Add these helper function declarations to parser.h
 * ============================================ */

// Add these to parser.h
bool is_objc_object_type(DataType type);
ARCQualifier parser_parse_arc_qualifier(Parser* parser);
ASTNode* parser_parse_arc_variable_declaration(Parser* parser);
ASTNode* parser_parse_arc_bridge_cast(Parser* parser);

void ast_destroy(ASTNode *node);

#endif // PARSER_H