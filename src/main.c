// Standard C library includes - be very explicit
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

extern FILE *stderr;
extern FILE *stdout;
extern FILE *stdin;
extern size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
extern int fprintf(FILE *restrict stream, const char *restrict format, ...);
extern int printf(const char *restrict format, ...);
extern int fseek(FILE *stream, long offset, int whence);
extern long ftell(FILE *stream);
extern FILE *fopen(const char *restrict filename, const char *restrict mode);
extern int fclose(FILE *stream);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern int strcmp(const char *s1, const char *s2);

// Project-specific includes
#include "kcc.h"
#include "preprocessor.h"
#include "symbol_table.h"
#include "parser.h"

struct Parser *parser = NULL;

void print_usage(const char *program_name) {
    printf("KCC - Kayte C Compiler v%s\n\n", KCC_VERSION);
    printf("Usage: %s [options] <input_file>\n\n", program_name);
    printf("Options:\n");
    printf("  -o <file>     Specify output file\n");
    printf("  -v, --verbose Enable verbose output\n");
    printf("  -d, --debug   Enable debug mode\n");
    printf("  -O            Enable optimization\n");
    printf("  -S            Keep assembly output\n");
    printf("  -E            Run preprocessor only\n");
    printf("  --no-preprocess Skip preprocessing step\n");
    printf("  -h, --help    Show this help message\n");
    printf("  --version     Show version information\n");
    printf("\nExamples:\n");
    printf("  %s hello.c\n", program_name);
    printf("  %s -o hello hello.c\n", program_name);
    printf("  %s -v -O hello.c\n", program_name);
    printf("  %s -E macros.c > preprocessed.c\n", program_name);
}

void print_version(void) {
    printf("KCC (Kayte C Compiler) version %s\n", KCC_VERSION);
    printf("Copyright (c) 2025 KCC Contributors\n");
    printf("This is free software; see the source for copying conditions.\n");
}

// Forward declarations
void error_init(void);
bool error_has_errors(void);
int error_count(void);
//struct Parser *parser = parser_create(debug_lexer);



int compile_file(const char *input_file, const char *output_file, CompilerOptions *opts) {

    printf("DEBUG: Starting compilation of '%s'\n", input_file);

    // Set default output file if not provided
    const char *final_output = output_file ? output_file : "a.out";

    // Check if file exists and is readable
    FILE *test_file = fopen(input_file, "r");
    if (!test_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }
    fclose(test_file);
    printf("DEBUG: File '%s' is readable\n", input_file);

    // Read and preprocess
    printf("DEBUG: Creating preprocessor...\n");
    Preprocessor *preprocessor = preprocessor_create();
    if (!preprocessor) {
        fprintf(stderr, "Error: Failed to create preprocessor\n");
        return 1;
    }
    printf("DEBUG: Preprocessor created successfully\n");

    printf("DEBUG: Processing file...\n");
    char *preprocessed_source = preprocessor_process_file(preprocessor, input_file);
    if (!preprocessed_source) {
        fprintf(stderr, "Error: Preprocessing failed\n");
        preprocessor_destroy(preprocessor);
        return 1;
    }

    printf("DEBUG: Preprocessed source (first 500 chars):\n");
    printf("%.500s\n", preprocessed_source);
    printf("DEBUG: End of preprocessed source\n");

    printf("DEBUG: Preprocessed source length: %zu\n", strlen(preprocessed_source));

    // Create lexer
    printf("DEBUG: Creating lexer...\n");
    Lexer *lexer = lexer_create(preprocessed_source, input_file);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }
    printf("DEBUG: Lexer created successfully\n");

    // Add this after creating the lexer in compile_file() function
    printf("DEBUG: Lexer created successfully\n");

    // Optional: Debug the first few tokens to see what the lexer is producing
    if (opts && opts->debug) {
        printf("DEBUG: First few tokens from lexer:\n");
        Lexer *debug_lexer = lexer_create(preprocessed_source, input_file);
        if (debug_lexer) {
            for (int i = 0; i < 10; i++) {
                Token tok = lexer_next_token(debug_lexer);
                printf("Token %d: type=%d, value='%s', line=%d, col=%d\n",
                       i, tok.type, tok.value ? tok.value : "(null)", tok.line, tok.column);
                if (tok.type == TOKEN_EOF) break;
            }
            lexer_destroy(debug_lexer);
            printf("DEBUG: Token debugging complete\n");
        }
    }

    // Create parser
    printf("DEBUG: Creating parser...\n");
    Parser *parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }
    printf("DEBUG: Parser created successfully\n");

    // Parse AST
    printf("DEBUG: Parsing AST...\n");
    ASTNode *ast = parser_parse_program(parser);
    if (!ast) {
        fprintf(stderr, "Error: Parsing failed\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    // Print AST if verbose
    if (opts && opts->verbose) {
        printf("AST:\n");
        ast_print(ast, 0);
    }

    // Create temporary assembly file name
    char *asm_file = malloc(strlen(final_output) + 16);
    if (!asm_file) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }
    snprintf(asm_file, strlen(final_output) + 16, "%s.s", final_output);

    // Generate assembly code
    printf("DEBUG: Generating assembly code to '%s'...\n", asm_file);
    CodeGenerator *codegen = codegen_create(asm_file);
    if (!codegen) {
        fprintf(stderr, "Error: Failed to create code generator for '%s'\n", asm_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    if (!codegen_generate(codegen, ast)) {
        fprintf(stderr, "Error: Code generation failed\n");
        codegen_destroy(codegen);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    printf("DEBUG: Assembly code generation completed\n");
    codegen_destroy(codegen);

    // Stop here if user only wants assembly
    if (opts && opts->keep_asm) {
        printf("Assembly file generated: %s\n", asm_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 0;
    }

    // Now assemble and link to create executable
    printf("DEBUG: Assembling and linking to create '%s'...\n", final_output);

    // Create object file name
    char *obj_file = malloc(strlen(final_output) + 16);
    if (!obj_file) {
        fprintf(stderr, "Error: Memory allocation failed for object file name\n");
        remove(asm_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }
    snprintf(obj_file, strlen(final_output) + 16, "%s.o", final_output);

    // Step 1: Assemble (.s -> .o) - Use clang for macOS compatibility
    char *as_cmd = malloc(strlen(asm_file) + strlen(obj_file) + 64);
    if (!as_cmd) {
        fprintf(stderr, "Error: Memory allocation failed for assembler command\n");
        remove(asm_file);
        free(obj_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    // Use clang as assembler for better macOS compatibility
    snprintf(as_cmd, strlen(asm_file) + strlen(obj_file) + 64,
             "clang -c '%s' -o '%s'", asm_file, obj_file);

    printf("DEBUG: Running assembler: %s\n", as_cmd);
    int as_result = system(as_cmd);
    free(as_cmd);

    if (as_result != 0) {
        fprintf(stderr, "Error: Assembly failed (assembler returned %d)\n", as_result);
        remove(asm_file);
        free(obj_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    // Step 2: Link (.o -> executable) - Use clang as linker for macOS
    char *ld_cmd = malloc(strlen(obj_file) + strlen(final_output) + 128);
    if (!ld_cmd) {
        fprintf(stderr, "Error: Memory allocation failed for linker command\n");
        remove(obj_file);
        remove(asm_file);
        free(obj_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    // Use clang as linker with proper entry point for macOS
    snprintf(ld_cmd, strlen(obj_file) + strlen(final_output) + 128,
             "clang '%s' -o '%s' -Wl,-e,_main -nostartfiles", obj_file, final_output);

    printf("DEBUG: Running linker: %s\n", ld_cmd);
    int ld_result = system(ld_cmd);
    free(ld_cmd);

    if (ld_result != 0) {
        fprintf(stderr, "Error: Linking failed (linker returned %d)\n", ld_result);
        remove(obj_file);
        remove(asm_file);
        free(obj_file);
        free(asm_file);
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(preprocessed_source);
        preprocessor_destroy(preprocessor);
        return 1;
    }

    // Clean up temporary files (unless user wants to keep them)
    if (!opts || !opts->keep_asm) {
        remove(asm_file);
        remove(obj_file);
    }

    printf("Compilation successful: %s -> %s\n", input_file, final_output);

    // Cleanup
    free(obj_file);
    free(asm_file);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(preprocessed_source);
    preprocessor_destroy(preprocessor);

    return 0;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    CompilerOptions opts = {0};
    opts.input_file = NULL;
    opts.output_file = "a.out";
    opts.verbose = false;
    opts.debug = false;
    opts.optimize = false;
    opts.keep_asm = false;
    opts.no_preprocess = false;
    opts.preprocess_only = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            opts.verbose = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            opts.debug = true;
        } else if (strcmp(argv[i], "-O") == 0) {
            opts.optimize = true;
        } else if (strcmp(argv[i], "-S") == 0) {
            opts.keep_asm = true;
        } else if (strcmp(argv[i], "-E") == 0) {
            opts.preprocess_only = true;
        } else if (strcmp(argv[i], "--no-preprocess") == 0) {
            opts.no_preprocess = true;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o requires an output filename\n");
                return 1;
            }
            opts.output_file = argv[++i];
        }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            if (opts.input_file) {
                fprintf(stderr, "Error: Multiple input files specified\n");
                return 1;
            }
            opts.input_file = argv[i];
        }
    }

    if (!opts.input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    return compile_file(opts.input_file, opts.output_file, &opts);
}