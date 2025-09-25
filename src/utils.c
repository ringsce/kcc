#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "kcc.h"

// Read entire file into memory
char* read_file(const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Error: Could not determine size of file '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Allocate buffer
    char* content = malloc(size + 1);
    if (!content) {
        fprintf(stderr, "Error: Memory allocation failed for file '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Read file content
    size_t bytes_read = fread(content, 1, size, file);
    content[bytes_read] = '\0';

    fclose(file);

    if (bytes_read != (size_t)size) {
        fprintf(stderr, "Warning: Expected to read %ld bytes from '%s', but read %zu\n",
                size, filename, bytes_read);
    }

    return content;
}

// Safe memory allocation
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed: %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Safe string copy
bool safe_strcpy(char* dest, size_t dest_size, const char* src) {
    if (!dest || !src || dest_size == 0) {
        return false;
    }

    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return false; // Not enough space
    }

    strcpy(dest, src);
    return true;
}

// Safe string duplication
char* safe_strdup(const char* src) {
    if (!src) {
        return NULL;
    }

    size_t len = strlen(src) + 1;
    char* copy = safe_malloc(len);
    strcpy(copy, src);
    return copy;
}

// Convert token type to data type
DataType token_to_data_type(TokenType type) {
    switch (type) {
        case TOKEN_VOID:
            return TYPE_VOID;
        case TOKEN_INT:
            return TYPE_INT;
        case TOKEN_FLOAT:
            return TYPE_FLOAT;
        case TOKEN_CHAR_KW:
            return TYPE_CHAR;
        default:
            return TYPE_UNKNOWN;
    }
}

// Convert data type to string
const char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_VOID:
            return "void";
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "float";
        case TYPE_CHAR:
            return "char";
        case TYPE_UNKNOWN:
        default:
            return "unknown";
    }
}

// Convert AST node type to string
const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_FUNCTION_DECLARATION:
            return "function_declaration";
        case AST_VARIABLE_DECLARATION:
            return "variable_declaration";
        case AST_PARAMETER:
            return "parameter";
        case AST_COMPOUND_STATEMENT:
            return "compound_statement";
        case AST_EXPRESSION_STATEMENT:
            return "expression_statement";
        case AST_IF_STATEMENT:
            return "if_statement";
        case AST_WHILE_STATEMENT:
            return "while_statement";
        case AST_FOR_STATEMENT:
            return "for_statement";
        case AST_RETURN_STATEMENT:
            return "return_statement";
        case AST_BREAK_STATEMENT:
            return "break_statement";
        case AST_CONTINUE_STATEMENT:
            return "continue_statement";
        case AST_BINARY_OP:
            return "binary_op";
        case AST_UNARY_OP:
            return "unary_op";
        case AST_ASSIGNMENT:
            return "assignment";
        case AST_FUNCTION_CALL:
            return "function_call";
        case AST_IDENTIFIER:
            return "identifier";
        case AST_NUMBER_LITERAL:
            return "number_literal";
        case AST_STRING_LITERAL:
            return "string_literal";
        case AST_CHAR_LITERAL:
            return "char_literal";
        case AST_PROGRAM:
            return "program";
        default:
            return "unknown";
    }
}

// Note: token_type_to_string is defined in lexer.c to avoid duplicate symbols

// Simple AST printer function
void ast_print(ASTNode *node, int indent) {
    if (!node) {
        return;
    }

    // Print indentation
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    printf("%s", ast_node_type_to_string(node->type));

    // Print node-specific information
    switch (node->type) {
        case AST_IDENTIFIER:
            if (node->data.identifier.name) {
                printf(" (%s)", node->data.identifier.name);
            }
            break;
        case AST_NUMBER_LITERAL:
            printf(" (%d)", node->data.number.value);
            break;
        case AST_STRING_LITERAL:
            if (node->data.string.value) {
                printf(" (\"%s\")", node->data.string.value);
            }
            break;
        case AST_FUNCTION_DECLARATION:
            if (node->data.function_decl.name) {
                printf(" (%s)", node->data.function_decl.name);
            }
            break;
        case AST_VARIABLE_DECLARATION:
            if (node->data.var_decl.name) {
                printf(" (%s)", node->data.var_decl.name);
            }
            break;
        case AST_BINARY_OP:
            printf(" (%s)", token_type_to_string(node->data.binary_expr.operator));
            break;
        case AST_UNARY_OP:
            printf(" (%s)", token_type_to_string(node->data.unary_expr.operator));
            break;
        default:
            break;
    }

    printf("\n");

    // Print children based on node type
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.declaration_count; i++) {
                ast_print(node->data.program.declarations[i], indent + 1);
            }
            break;
        case AST_FUNCTION_DECLARATION:
            for (int i = 0; i < node->data.function_decl.parameter_count; i++) {
                ast_print(node->data.function_decl.parameters[i], indent + 1);
            }
            if (node->data.function_decl.body) {
                ast_print(node->data.function_decl.body, indent + 1);
            }
            break;
        case AST_COMPOUND_STATEMENT:
            for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
                ast_print(node->data.compound_stmt.statements[i], indent + 1);
            }
            break;
        case AST_IF_STATEMENT:
            if (node->data.if_stmt.condition) {
                ast_print(node->data.if_stmt.condition, indent + 1);
            }
            if (node->data.if_stmt.then_stmt) {
                ast_print(node->data.if_stmt.then_stmt, indent + 1);
            }
            if (node->data.if_stmt.else_stmt) {
                ast_print(node->data.if_stmt.else_stmt, indent + 1);
            }
            break;
        case AST_WHILE_STATEMENT:
            if (node->data.while_stmt.condition) {
                ast_print(node->data.while_stmt.condition, indent + 1);
            }
            if (node->data.while_stmt.body) {
                ast_print(node->data.while_stmt.body, indent + 1);
            }
            break;
        case AST_FOR_STATEMENT:
            if (node->data.for_stmt.init) {
                ast_print(node->data.for_stmt.init, indent + 1);
            }
            if (node->data.for_stmt.condition) {
                ast_print(node->data.for_stmt.condition, indent + 1);
            }
            if (node->data.for_stmt.update) {
                ast_print(node->data.for_stmt.update, indent + 1);
            }
            if (node->data.for_stmt.body) {
                ast_print(node->data.for_stmt.body, indent + 1);
            }
            break;
        case AST_BINARY_OP:
            if (node->data.binary_expr.left) {
                ast_print(node->data.binary_expr.left, indent + 1);
            }
            if (node->data.binary_expr.right) {
                ast_print(node->data.binary_expr.right, indent + 1);
            }
            break;
        case AST_UNARY_OP:
            if (node->data.unary_expr.operand) {
                ast_print(node->data.unary_expr.operand, indent + 1);
            }
            break;
        case AST_ASSIGNMENT:
            if (node->data.assignment.value) {
                ast_print(node->data.assignment.value, indent + 1);
            }
            break;
        case AST_FUNCTION_CALL:
            for (int i = 0; i < node->data.call_expr.argument_count; i++) {
                ast_print(node->data.call_expr.arguments[i], indent + 1);
            }
            break;
        case AST_VARIABLE_DECLARATION:
            if (node->data.var_decl.initializer) {
                ast_print(node->data.var_decl.initializer, indent + 1);
            }
            break;
        case AST_EXPRESSION_STATEMENT:
            if (node->data.expression_stmt.expression) {
                ast_print(node->data.expression_stmt.expression, indent + 1);
            }
            break;
        case AST_RETURN_STATEMENT:
            if (node->data.return_stmt.expression) {
                ast_print(node->data.return_stmt.expression, indent + 1);
            }
            break;
        default:
            // No children to print for literals and other leaf nodes
            break;
    }
}