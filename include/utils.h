#ifndef UTILS_H
#define UTILS_H

#include "kcc.h" // Provides access to types like DataType, ASTNodeType, etc.

// This header declares truly generic utility functions that can be used
// across the entire compiler.

// --- Memory Management Wrappers ---
void* safe_malloc(size_t size);
char* safe_strdup(const char *s);
char* safe_strcpy(char *dest, const char *src, size_t size);


// --- File I/O ---
char* read_file(const char *filename);


// --- Type Conversion & Debugging ---
DataType token_to_data_type(TokenType type);
const char* ast_node_type_to_string(ASTNodeType type);
void ast_print(ASTNode *node, int indent);


#endif // UTILS_H

