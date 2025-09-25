#ifndef KCC_H
#define KCC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>

#define KCC_VERSION "1.10.0"
#define MAX_TOKENS 1024
#define MAX_NODES 512
#define MAX_IDENTIFIER_LENGTH 256
#define MAX_STRING_LENGTH 1024

// Include types first so all structs and enums are defined
#include "types.h"

// Include headers in correct order
#include "error.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "codegen.h"
#include "preprocessor.h"

// Essential utility functions
char* read_file(const char* filename);
void* safe_malloc(size_t size);
bool safe_strcpy(char* dest, size_t dest_size, const char* src);
char* safe_strdup(const char* src);

// Safe free macro
#define SAFE_FREE(ptr) do { \
if (ptr) { \
free(ptr); \
(ptr) = NULL; \
} \
} while(0)

// Compiler options
typedef struct {
    char *input_file;
    char *output_file;
    bool verbose;
    bool debug;
    bool optimize;
    bool keep_asm;
    bool no_preprocess;  // Skip preprocessing
    bool preprocess_only; // Only run preprocessor
    char **user_macros;   // User-defined macros from command line
    int macro_count;      // Number of user macros
    bool include_env;     // Include environment variable macros
    bool include_system;  // Include system information macros
    char *target_arch;    // Target architecture (x86_64, arm64)
    char *target_platform; // Target platform (linux, macos)
    bool use_multiarch;   // Use multi-architecture codegen

} CompilerOptions;

// Main compiler functions
int compile_file(const char *input_file, const char *output_file, CompilerOptions *opts);
void print_usage(const char *program_name);
void print_version(void);

#endif // KCC_H