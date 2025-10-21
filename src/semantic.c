#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "kcc.h"

// Simple semantic checking without full symbol table
// This is a placeholder implementation

bool semantic_check_const_assignment(ASTNode *node) {
    if (node->type == AST_ASSIGNMENT) {
        // For now, just add a placeholder
        // A full implementation would need a symbol table to track const variables
        // fprintf(stderr, "Warning: Const checking not fully implemented\n");
        return true;
    }
    return true;
}

// Recursive semantic check for an AST tree
bool semantic_check_tree(ASTNode *node) {
    if (!node) return true;

    // Check this node
    if (!semantic_check_const_assignment(node)) {
        return false;
    }

    // Recursively check children based on node type
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.declaration_count; i++) {
                if (!semantic_check_tree(node->data.program.declarations[i])) {
                    return false;
                }
            }
            break;

        case AST_FUNCTION_DECLARATION:
            if (node->data.function_decl.body) {
                if (!semantic_check_tree(node->data.function_decl.body)) {
                    return false;
                }
            }
            break;

        case AST_COMPOUND_STATEMENT:
            for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
                if (!semantic_check_tree(node->data.compound_stmt.statements[i])) {
                    return false;
                }
            }
            break;

        case AST_IF_STATEMENT:
            if (!semantic_check_tree(node->data.if_stmt.condition)) return false;
            if (!semantic_check_tree(node->data.if_stmt.then_stmt)) return false;
            if (node->data.if_stmt.else_stmt) {
                if (!semantic_check_tree(node->data.if_stmt.else_stmt)) return false;
            }
            break;

        case AST_WHILE_STATEMENT:
            if (!semantic_check_tree(node->data.while_stmt.condition)) return false;
            if (!semantic_check_tree(node->data.while_stmt.body)) return false;
            break;

        case AST_RETURN_STATEMENT:
            if (node->data.return_stmt.expression) {
                if (!semantic_check_tree(node->data.return_stmt.expression)) return false;
            }
            break;

        case AST_BINARY_OP:
            if (!semantic_check_tree(node->data.binary_expr.left)) return false;
            if (!semantic_check_tree(node->data.binary_expr.right)) return false;
            break;

        default:
            // For other node types, just return true
            break;
    }

    return true;
}