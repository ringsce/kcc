// Add these functions to your ast.c file

#include "types.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>
ASTNode *ast_create_typedef(ASTNode *base_type, const char *alias_name);
ASTNode *ast_create_struct(const char *name);
ASTNode *ast_create_union(const char *name);
ASTNode *ast_create_enum(const char *name);
ASTNode *ast_create_enum_constant(const char *name, int value);
ASTNode *ast_create_struct_member(DataType type, const char *name, int bitfield_width);
ASTNode *ast_create_basic_type(DataType type);
ASTNode *ast_create_var_decl_with_type_node(ASTNode *type_node, const char *name);

// And corresponding functions to add members/constants:
void ast_add_struct_member(ASTNode *struct_node, ASTNode *member);
void ast_add_union_member(ASTNode *union_node, ASTNode *member);
void ast_add_enum_constant(ASTNode *enum_node, ASTNode *constant);
void ast_set_member_type_node(ASTNode *member, ASTNode *type_node);

// Add this helper function right here
bool is_objc_object_type(DataType type) {
    return type == TYPE_ID ||
           type == TYPE_CLASS ||
           type == TYPE_SEL ||
           type == TYPE_POINTER;
}

// Basic AST creation functions
ASTNode *ast_create_program(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->data.program.declarations = NULL;
    node->data.program.declaration_count = 0;

    return node;
}

ASTNode *ast_create_function_decl(DataType return_type, const char *name, ASTNode **params, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_FUNCTION_DECLARATION;
    node->data.function_decl.return_type = return_type;
    node->data.function_decl.name = strdup(name);
    node->data.function_decl.parameters = params;
    node->data.function_decl.parameter_count = 0;
    node->data.function_decl.body = body;

    return node;
}

ASTNode *ast_create_var_decl(DataType type, const char *name, ASTNode *initializer) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_VARIABLE_DECLARATION;
    node->data.var_decl.var_type = type;
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.initializer = initializer;

    return node;
}

ASTNode *ast_create_compound_stmt(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_COMPOUND_STATEMENT;
    node->data.compound_stmt.statements = NULL;
    node->data.compound_stmt.statement_count = 0;

    return node;
}

ASTNode *ast_create_expression_stmt(ASTNode *expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_EXPRESSION_STATEMENT;
    node->data.expression_stmt.expression = expr;

    return node;
}

ASTNode *ast_create_return_stmt(ASTNode *expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_RETURN_STATEMENT;
    node->data.return_stmt.expression = expr;

    return node;
}

ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_stmt, ASTNode *else_stmt) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_stmt = then_stmt;
    node->data.if_stmt.else_stmt = else_stmt;

    return node;
}

ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_WHILE_STATEMENT;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;

    return node;
}

ASTNode *ast_create_for_stmt(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_FOR_STATEMENT;
    node->data.for_stmt.init = init;
    node->data.for_stmt.condition = condition;
    node->data.for_stmt.update = update;
    node->data.for_stmt.body = body;

    return node;
}

ASTNode *ast_create_break_stmt(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_BREAK_STATEMENT;

    return node;
}

ASTNode *ast_create_continue_stmt(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_CONTINUE_STATEMENT;

    return node;
}

ASTNode *ast_create_binary_expr(TokenType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->data.binary_expr.operator = op;
    node->data.binary_expr.left = left;
    node->data.binary_expr.right = right;

    return node;
}

ASTNode *ast_create_unary_expr(TokenType op, ASTNode *operand) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_UNARY_OP;
    node->data.unary_expr.operator = op;
    node->data.unary_expr.operand = operand;

    return node;
}

ASTNode *ast_create_assignment(const char *variable, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->data.assignment.variable = strdup(variable);
    node->data.assignment.value = value;

    return node;
}

ASTNode *ast_create_call_expr(const char *function_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->data.call_expr.function_name = strdup(function_name);
    node->data.call_expr.arguments = NULL;
    node->data.call_expr.argument_count = 0;

    return node;
}

ASTNode *ast_create_identifier(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = strdup(name);

    return node;
}

ASTNode *ast_create_number(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_NUMBER_LITERAL;
    node->data.number.value = value;

    return node;
}

ASTNode *ast_create_string(const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_STRING_LITERAL;
    node->data.string.value = strdup(value);

    return node;
}

// AST manipulation functions
void ast_add_declaration(ASTNode *program, ASTNode *declaration) {
    if (!program || program->type != AST_PROGRAM || !declaration) return;

    program->data.program.declaration_count++;
    program->data.program.declarations = realloc(program->data.program.declarations,
        sizeof(ASTNode*) * program->data.program.declaration_count);
    program->data.program.declarations[program->data.program.declaration_count - 1] = declaration;
}

void ast_add_statement(ASTNode *compound, ASTNode *statement) {
    if (!compound || compound->type != AST_COMPOUND_STATEMENT || !statement) return;

    compound->data.compound_stmt.statement_count++;
    compound->data.compound_stmt.statements = realloc(compound->data.compound_stmt.statements,
        sizeof(ASTNode*) * compound->data.compound_stmt.statement_count);
    compound->data.compound_stmt.statements[compound->data.compound_stmt.statement_count - 1] = statement;
}

void ast_add_argument(ASTNode *call, ASTNode *argument) {
    if (!call || call->type != AST_FUNCTION_CALL || !argument) return;

    call->data.call_expr.argument_count++;
    call->data.call_expr.arguments = realloc(call->data.call_expr.arguments,
        sizeof(ASTNode*) * call->data.call_expr.argument_count);
    call->data.call_expr.arguments[call->data.call_expr.argument_count - 1] = argument;
}

// AST destruction function
void ast_destroy(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.declaration_count; i++) {
                ast_destroy(node->data.program.declarations[i]);
            }
            free(node->data.program.declarations);
            break;

        case AST_FUNCTION_DECLARATION:
            free(node->data.function_decl.name);
            if (node->data.function_decl.parameters) {
                for (int i = 0; i < node->data.function_decl.parameter_count; i++) {
                    ast_destroy(node->data.function_decl.parameters[i]);
                }
                free(node->data.function_decl.parameters);
            }
            ast_destroy(node->data.function_decl.body);
            break;

        case AST_VARIABLE_DECLARATION:
            free(node->data.var_decl.name);
            ast_destroy(node->data.var_decl.initializer);
            break;

        case AST_COMPOUND_STATEMENT:
            for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
                ast_destroy(node->data.compound_stmt.statements[i]);
            }
            free(node->data.compound_stmt.statements);
            break;

        case AST_EXPRESSION_STATEMENT:
            ast_destroy(node->data.expression_stmt.expression);
            break;

        case AST_RETURN_STATEMENT:
            ast_destroy(node->data.return_stmt.expression);
            break;

        case AST_IF_STATEMENT:
            ast_destroy(node->data.if_stmt.condition);
            ast_destroy(node->data.if_stmt.then_stmt);
            ast_destroy(node->data.if_stmt.else_stmt);
            break;

        case AST_WHILE_STATEMENT:
            ast_destroy(node->data.while_stmt.condition);
            ast_destroy(node->data.while_stmt.body);
            break;

        case AST_FOR_STATEMENT:
            ast_destroy(node->data.for_stmt.init);
            ast_destroy(node->data.for_stmt.condition);
            ast_destroy(node->data.for_stmt.update);
            ast_destroy(node->data.for_stmt.body);
            break;

        case AST_BINARY_OP:
            ast_destroy(node->data.binary_expr.left);
            ast_destroy(node->data.binary_expr.right);
            break;

        case AST_UNARY_OP:
            ast_destroy(node->data.unary_expr.operand);
            break;

        case AST_ASSIGNMENT:
            free(node->data.assignment.variable);
            ast_destroy(node->data.assignment.value);
            break;

        case AST_FUNCTION_CALL:
            free(node->data.call_expr.function_name);
            for (int i = 0; i < node->data.call_expr.argument_count; i++) {
                ast_destroy(node->data.call_expr.arguments[i]);
            }
            free(node->data.call_expr.arguments);
            break;

        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;

        case AST_STRING_LITERAL:
            free(node->data.string.value);
            break;

        case AST_OBJC_INTERFACE:
            free(node->data.objc_interface.class_name);
            free(node->data.objc_interface.superclass_name);
            for (int i = 0; i < node->data.objc_interface.method_count; i++) {
                ast_destroy(node->data.objc_interface.methods[i]);
            }
            free(node->data.objc_interface.methods);
            for (int i = 0; i < node->data.objc_interface.property_count; i++) {
                ast_destroy(node->data.objc_interface.properties[i]);
            }
            free(node->data.objc_interface.properties);
            break;

        case AST_OBJC_IMPLEMENTATION:
            free(node->data.objc_implementation.class_name);
            free(node->data.objc_implementation.category_name);
            for (int i = 0; i < node->data.objc_implementation.method_count; i++) {
                ast_destroy(node->data.objc_implementation.methods[i]);
            }
            free(node->data.objc_implementation.methods);
            break;

        case AST_OBJC_METHOD_DECLARATION:
            free(node->data.objc_method.selector);
            ast_destroy(node->data.objc_method.body);
            break;

        case AST_OBJC_PROPERTY_DECLARATION:
            free(node->data.objc_property.property_name);
            break;

        case AST_OBJC_MESSAGE_SEND:
            ast_destroy(node->data.objc_message.receiver);
            free(node->data.objc_message.selector);
            for (int i = 0; i < node->data.objc_message.argument_count; i++) {
                ast_destroy(node->data.objc_message.arguments[i]);
            }
            free(node->data.objc_message.arguments);
            break;

        case AST_OBJC_STRING_LITERAL:
            free(node->data.objc_string.value);
            break;

        case AST_OBJC_SELECTOR_EXPR:
            free(node->data.objc_selector.selector_name);
            break;

        case AST_OBJC_PROTOCOL:
            free(node->data.objc_protocol.protocol_name);
            for (int i = 0; i < node->data.objc_protocol.method_count; i++) {
                ast_destroy(node->data.objc_protocol.methods[i]);
            }
            free(node->data.objc_protocol.methods);
            for (int i = 0; i < node->data.objc_protocol.property_count; i++) {
                ast_destroy(node->data.objc_protocol.properties[i]);
            }
            free(node->data.objc_protocol.properties);
            break;

        default:
            // Handle other node types as needed
            break;
    }

    free(node);
}

// Objective-C Interface
ASTNode *ast_create_objc_interface(const char *class_name, const char *superclass_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_INTERFACE;

    node->data.objc_interface.class_name = strdup(class_name);
    node->data.objc_interface.superclass_name = superclass_name ? strdup(superclass_name) : NULL;
    node->data.objc_interface.methods = NULL;
    node->data.objc_interface.method_count = 0;
    node->data.objc_interface.properties = NULL;
    node->data.objc_interface.property_count = 0;

    return node;
}

// Objective-C Implementation
ASTNode *ast_create_objc_implementation(const char *class_name, const char *category_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_IMPLEMENTATION;

    node->data.objc_implementation.class_name = strdup(class_name);
    node->data.objc_implementation.category_name = category_name ? strdup(category_name) : NULL;
    node->data.objc_implementation.methods = NULL;
    node->data.objc_implementation.method_count = 0;

    return node;
}

// Objective-C Protocol
ASTNode *ast_create_objc_protocol(const char *protocol_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_PROTOCOL;

    node->data.objc_protocol.protocol_name = strdup(protocol_name);
    node->data.objc_protocol.methods = NULL;
    node->data.objc_protocol.method_count = 0;

    return node;
}

// Objective-C Method
ASTNode *ast_create_objc_method(ObjCMethodType method_type, DataType return_type, const char *selector, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_METHOD_DECLARATION;

    node->data.objc_method.method_type = method_type;
    node->data.objc_method.return_type = return_type;
    node->data.objc_method.selector = strdup(selector);
    node->data.objc_method.body = body;

    return node;
}

// Objective-C Property
ASTNode *ast_create_objc_property(DataType property_type, const char *property_name, ObjCPropertyAttributes attributes) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_PROPERTY_DECLARATION;

    node->data.objc_property.property_type = property_type;
    node->data.objc_property.property_name = strdup(property_name);
    node->data.objc_property.attributes = attributes;

    return node;
}

// Objective-C Synthesize
ASTNode *ast_create_objc_synthesize(const char *property_name, const char *ivar_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_SYNTHESIZE;

    // Store property name in a simple way for now
    node->data.string.value = strdup(property_name);

    return node;
}

// Objective-C Dynamic (stub)
ASTNode *ast_create_objc_dynamic(const char *property_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_DYNAMIC;

    node->data.string.value = strdup(property_name);

    return node;
}

// Objective-C Message Send
ASTNode *ast_create_objc_message_send(ASTNode *receiver, const char *selector) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_MESSAGE_SEND;

    node->data.objc_message.receiver = receiver;
    node->data.objc_message.selector = strdup(selector);
    node->data.objc_message.arguments = NULL;
    node->data.objc_message.argument_count = 0;

    return node;
}

// Objective-C String Literal
ASTNode *ast_create_objc_string(const char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_STRING_LITERAL;

    node->data.objc_string.value = strdup(value);

    return node;
}

// Objective-C Boolean
ASTNode *ast_create_objc_boolean(bool value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_BOOLEAN_LITERAL;

    node->data.objc_boolean.value = value;

    return node;
}

// Objective-C nil
ASTNode *ast_create_objc_nil(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_IDENTIFIER; // Treat as special identifier for now

    node->data.identifier.name = strdup("nil");

    return node;
}

// Objective-C self
ASTNode *ast_create_objc_self(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_IDENTIFIER;

    node->data.identifier.name = strdup("self");

    return node;
}

// Objective-C super
ASTNode *ast_create_objc_super(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_IDENTIFIER;

    node->data.identifier.name = strdup("super");

    return node;
}

// Objective-C Selector
ASTNode *ast_create_objc_selector(const char *selector_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_SELECTOR_EXPR;

    node->data.objc_selector.selector_name = strdup(selector_name);

    return node;
}

// Objective-C Encode
ASTNode *ast_create_objc_encode(DataType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_ENCODE_EXPR;

    node->data_type = type;

    return node;
}

// Objective-C Try Statement
ASTNode *ast_create_objc_try(ASTNode *try_body, ASTNode **catch_blocks, int catch_count, ASTNode *finally_block) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_TRY_STATEMENT;

    // Simplified - store just the try body for now
    node->data.compound_stmt.statements = malloc(sizeof(ASTNode*));
    node->data.compound_stmt.statements[0] = try_body;
    node->data.compound_stmt.statement_count = 1;

    return node;
}

// Objective-C Catch Block
ASTNode *ast_create_objc_catch(DataType exception_type, const char *exception_var, ASTNode *catch_body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_CATCH_STATEMENT;

    node->data.compound_stmt.statements = malloc(sizeof(ASTNode*));
    node->data.compound_stmt.statements[0] = catch_body;
    node->data.compound_stmt.statement_count = 1;

    return node;
}

// Objective-C Throw Statement
ASTNode *ast_create_objc_throw(ASTNode *exception_expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_THROW_STATEMENT;

    node->data.expression_stmt.expression = exception_expr;

    return node;
}

// Objective-C Synchronized Statement
ASTNode *ast_create_objc_synchronized(ASTNode *sync_object, ASTNode *sync_body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_SYNCHRONIZED_STATEMENT;

    node->data.compound_stmt.statements = malloc(sizeof(ASTNode*));
    node->data.compound_stmt.statements[0] = sync_body;
    node->data.compound_stmt.statement_count = 1;

    return node;
}

// Objective-C Autoreleasepool Statement
ASTNode *ast_create_objc_autoreleasepool(ASTNode *pool_body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_OBJC_AUTORELEASEPOOL_STATEMENT;

    node->data.compound_stmt.statements = malloc(sizeof(ASTNode*));
    node->data.compound_stmt.statements[0] = pool_body;
    node->data.compound_stmt.statement_count = 1;

    return node;
}

// Property Access
ASTNode *ast_create_property_access(ASTNode *object, const char *property_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_MEMBER_ACCESS;

    // Store in a simple way - you may need to adjust based on your AST structure
    node->data.binary_expr.left = object;
    node->data.binary_expr.right = ast_create_identifier(property_name);

    return node;
}

// AST manipulation functions (if they don't exist)
void ast_add_objc_interface_member(ASTNode *interface, ASTNode *member) {
    if (!interface || interface->type != AST_OBJC_INTERFACE || !member) return;

    // Simple implementation - expand arrays as needed
    interface->data.objc_interface.method_count++;
    interface->data.objc_interface.methods = realloc(interface->data.objc_interface.methods,
        sizeof(ASTNode*) * interface->data.objc_interface.method_count);
    interface->data.objc_interface.methods[interface->data.objc_interface.method_count - 1] = member;
}

void ast_add_objc_implementation_member(ASTNode *implementation, ASTNode *member) {
    if (!implementation || implementation->type != AST_OBJC_IMPLEMENTATION || !member) return;

    implementation->data.objc_implementation.method_count++;
    implementation->data.objc_implementation.methods = realloc(implementation->data.objc_implementation.methods,
        sizeof(ASTNode*) * implementation->data.objc_implementation.method_count);
    implementation->data.objc_implementation.methods[implementation->data.objc_implementation.method_count - 1] = member;
}

void ast_add_objc_protocol_method(ASTNode *protocol, ASTNode *method) {
    if (!protocol || protocol->type != AST_OBJC_PROTOCOL || !method) return;

    protocol->data.objc_protocol.method_count++;
    protocol->data.objc_protocol.methods = realloc(protocol->data.objc_protocol.methods,
        sizeof(ASTNode*) * protocol->data.objc_protocol.method_count);
    protocol->data.objc_protocol.methods[protocol->data.objc_protocol.method_count - 1] = method;
}

void ast_add_objc_protocol_property(ASTNode *protocol, ASTNode *property) {
    if (!protocol || protocol->type != AST_OBJC_PROTOCOL || !property) return;

    protocol->data.objc_protocol.property_count++;
    protocol->data.objc_protocol.properties = realloc(protocol->data.objc_protocol.properties,
        sizeof(ASTNode*) * protocol->data.objc_protocol.property_count);
    protocol->data.objc_protocol.properties[protocol->data.objc_protocol.property_count - 1] = property;
}


// Add these implementations to your ast.c file

ASTNode *ast_create_typedef(ASTNode *base_type, const char *alias_name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_TYPEDEF;
    node->data.typedef_decl.base_type = base_type;
    node->data.typedef_decl.alias_name = strdup(alias_name);

    return node;
}

ASTNode *ast_create_struct(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_STRUCT;
    node->data.struct_decl.name = name ? strdup(name) : NULL;
    node->data.struct_decl.members = NULL;
    node->data.struct_decl.member_count = 0;

    return node;
}

ASTNode *ast_create_union(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_UNION;
    node->data.union_decl.name = name ? strdup(name) : NULL;
    node->data.union_decl.members = NULL;
    node->data.union_decl.member_count = 0;

    return node;
}

ASTNode *ast_create_enum(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_ENUM;
    node->data.enum_decl.name = name ? strdup(name) : NULL;
    node->data.enum_decl.constants = NULL;
    node->data.enum_decl.constant_count = 0;

    return node;
}

ASTNode *ast_create_enum_constant(const char *name, int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_ENUM_CONSTANT;
    node->data.enum_constant.name = strdup(name);
    node->data.enum_constant.value = value;

    return node;
}

ASTNode *ast_create_struct_member(DataType type, const char *name, int bitfield_width) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_STRUCT_MEMBER;
    node->data.struct_member.type = type;
    node->data.struct_member.name = strdup(name);
    node->data.struct_member.bitfield_width = bitfield_width;
    node->data.struct_member.type_node = NULL;

    return node;
}

ASTNode *ast_create_basic_type(DataType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_BASIC_TYPE;
    node->data.basic_type.type = type;

    return node;
}

ASTNode *ast_create_var_decl_with_type_node(ASTNode *type_node, const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    memset(node, 0, sizeof(ASTNode));
    node->type = AST_VARIABLE_DECLARATION;
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.type_node = type_node;
    node->data.var_decl.initializer = NULL;
    node->data.var_decl.var_type = TYPE_UNKNOWN; // Will be determined from type_node

    return node;
}

void ast_add_struct_member(ASTNode *struct_node, ASTNode *member) {
    if (!struct_node || struct_node->type != AST_STRUCT || !member) return;

    struct_node->data.struct_decl.member_count++;
    struct_node->data.struct_decl.members = realloc(
        struct_node->data.struct_decl.members,
        struct_node->data.struct_decl.member_count * sizeof(ASTNode*)
    );

    if (struct_node->data.struct_decl.members) {
        struct_node->data.struct_decl.members[struct_node->data.struct_decl.member_count - 1] = member;
    }
}

void ast_add_union_member(ASTNode *union_node, ASTNode *member) {
    if (!union_node || union_node->type != AST_UNION || !member) return;

    union_node->data.union_decl.member_count++;
    union_node->data.union_decl.members = realloc(
        union_node->data.union_decl.members,
        union_node->data.union_decl.member_count * sizeof(ASTNode*)
    );

    if (union_node->data.union_decl.members) {
        union_node->data.union_decl.members[union_node->data.union_decl.member_count - 1] = member;
    }
}

void ast_add_enum_constant(ASTNode *enum_node, ASTNode *constant) {
    if (!enum_node || enum_node->type != AST_ENUM || !constant) return;

    enum_node->data.enum_decl.constant_count++;
    enum_node->data.enum_decl.constants = realloc(
        enum_node->data.enum_decl.constants,
        enum_node->data.enum_decl.constant_count * sizeof(ASTNode*)
    );

    if (enum_node->data.enum_decl.constants) {
        enum_node->data.enum_decl.constants[enum_node->data.enum_decl.constant_count - 1] = constant;
    }
}

void ast_set_member_type_node(ASTNode *member, ASTNode *type_node) {
    if (!member || member->type != AST_STRUCT_MEMBER || !type_node) return;

    member->data.struct_member.type_node = type_node;
}


// arrays
// Add these functions to your ast.c file:

#include "ast.h"
#include <stdlib.h>
#include <string.h>

// Create array declaration AST node
ASTNode* ast_create_array_declaration(ASTNode* element_type, ASTNode* size_expr,
                                     int is_dynamic, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_ARRAY_DECLARATION;
    node->data_type = TYPE_ARRAY;
    node->line = line;
    node->column = column;

    node->data.array_decl.element_type = element_type;
    node->data.array_decl.size_expr = size_expr;
    node->data.array_decl.is_dynamic = is_dynamic;
    node->data.array_decl.dimension_count = 1;

    // Allocate and set single dimension
    node->data.array_decl.dimensions = malloc(sizeof(ASTNode*));
    node->data.array_decl.dimensions[0] = size_expr;

    return node;
}

// Create multidimensional array declaration
ASTNode* ast_create_multidim_array_declaration(ASTNode* element_type,
                                              ASTNode** dimensions, int dim_count,
                                              int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_ARRAY_DECLARATION;
    node->data_type = TYPE_ARRAY;
    node->line = line;
    node->column = column;

    node->data.array_decl.element_type = element_type;
    node->data.array_decl.size_expr = (dim_count > 0) ? dimensions[0] : NULL;
    node->data.array_decl.is_dynamic = 0;  // Multidim arrays are typically static
    node->data.array_decl.dimension_count = dim_count;
    node->data.array_decl.dimensions = dimensions;

    return node;
}

// Create array access AST node
ASTNode* ast_create_array_access(ASTNode* array_expr, ASTNode* index_expr,
                                int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_ARRAY_ACCESS;
    node->data_type = TYPE_UNKNOWN; // Will be inferred from array_expr
    node->line = line;
    node->column = column;

    node->data.array_access.array_expr = array_expr;
    node->data.array_access.index_expr = index_expr;

    return node;
}

// Create array literal AST node
ASTNode* ast_create_array_literal(ASTNode** elements, int element_count,
                                 int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_ARRAY_LITERAL;
    node->data_type = TYPE_ARRAY;
    node->line = line;
    node->column = column;

    node->data.array_literal.elements = elements;
    node->data.array_literal.element_count = element_count;
    node->data.array_literal.element_type = NULL; // Will be inferred

    return node;
}

// Create address-of operator AST node
ASTNode* ast_create_address_of(ASTNode* operand, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_ADDRESS_OF;
    node->data_type = TYPE_POINTER;
    node->line = line;
    node->column = column;

    node->data.address_of.operand = operand;

    return node;
}

// Create pointer dereference AST node
ASTNode* ast_create_pointer_dereference(ASTNode* operand, int line, int column) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_POINTER_DEREFERENCE;
    node->data_type = TYPE_UNKNOWN; // Will be inferred from operand
    node->line = line;
    node->column = column;

    node->data.pointer_deref.operand = operand;

    return node;
}

// Helper function to destroy array-related AST nodes
void ast_destroy_array_node(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_ARRAY_DECLARATION:
            if (node->data.array_decl.element_type) {
                ast_destroy(node->data.array_decl.element_type);
            }
            if (node->data.array_decl.size_expr) {
                ast_destroy(node->data.array_decl.size_expr);
            }
            if (node->data.array_decl.dimensions) {
                for (int i = 0; i < node->data.array_decl.dimension_count; i++) {
                    if (node->data.array_decl.dimensions[i]) {
                        ast_destroy(node->data.array_decl.dimensions[i]);
                    }
                }
                free(node->data.array_decl.dimensions);
            }
            break;

        case AST_ARRAY_ACCESS:
            if (node->data.array_access.array_expr) {
                ast_destroy(node->data.array_access.array_expr);
            }
            if (node->data.array_access.index_expr) {
                ast_destroy(node->data.array_access.index_expr);
            }
            break;

        case AST_ARRAY_LITERAL:
            if (node->data.array_literal.elements) {
                for (int i = 0; i < node->data.array_literal.element_count; i++) {
                    if (node->data.array_literal.elements[i]) {
                        ast_destroy(node->data.array_literal.elements[i]);
                    }
                }
                free(node->data.array_literal.elements);
            }
            if (node->data.array_literal.element_type) {
                ast_destroy(node->data.array_literal.element_type);
            }
            break;

        case AST_ADDRESS_OF:
            if (node->data.address_of.operand) {
                ast_destroy(node->data.address_of.operand);
            }
            break;

        case AST_POINTER_DEREFERENCE:
            if (node->data.pointer_deref.operand) {
                ast_destroy(node->data.pointer_deref.operand);
            }
            break;

        default:
            break;
    }

    free(node);
}

/* ============================================
 * STEP 4: Add these AST creation functions to ast.c
 * ============================================ */

// Add to ast.c

ASTNode* ast_create_arc_var_decl(DataType type, const char* name,
                                 ASTNode* initializer, ARCQualifier qualifier) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    node->type = AST_VAR_DECL;
    node->data_type = type;
    node->line = 0;
    node->column = 0;

    // Initialize ARC info
    node->arc_info.qualifier = qualifier;
    node->arc_info.is_objc_object = is_objc_object_type(type);
    node->arc_info.needs_retain = false;
    node->arc_info.needs_release = (qualifier == ARC_QUALIFIER_STRONG &&
                                   node->arc_info.is_objc_object);
    node->arc_info.is_parameter = false;
    node->arc_info.is_return_value = false;
    node->arc_info.retain_count = 0;

    node->data.var_decl.var_type = type;
    node->data.var_decl.name = strdup(name);
    node->data.var_decl.initializer = initializer;
    node->data.var_decl.type_node = NULL;

    return node;
}
