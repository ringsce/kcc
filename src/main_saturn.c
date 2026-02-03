// ============================================================================
// src/main_saturn.c - KCC Compiler Main Entry Point for Sega Saturn
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// KCC headers
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "error.h"

// Saturn-specific headers
#ifdef TARGET_SATURN
#include "sh2_instruction_set.h"
#include "sh2_register_allocator.h"
#endif

// ============================================================================
// Saturn-specific Configuration
// ============================================================================

typedef struct {
    bool dual_cpu;
    bool use_linear_scan;
} SaturnOptions;

static SaturnOptions saturn_opts = {
    .dual_cpu = false,
    .use_linear_scan = false
};

// ============================================================================
// Version and Help
// ============================================================================

static void print_saturn_version(void) {
    printf("KCC - Kayte C Compiler for Sega Saturn\n");
    printf("Version 1.10.0\n");
    printf("Target: Hitachi SH-2 (32-bit RISC, Big Endian)\n");
    printf("Copyright (c) 2024 KCC Project\n\n");
}

static void print_saturn_help(void) {
    print_saturn_version();
    printf("Usage: kcc [options] <input.c>\n\n");
    printf("Standard Options:\n");
    printf("  -o <file>         Write output to <file>\n");
    printf("  -S                Emit assembly code\n");
    printf("  -c                Compile to object file\n");
    printf("  -v                Verbose output\n");
    printf("  -O0, -O1, -O2     Optimization levels\n");
    printf("  --help            Display this help\n");
    printf("\n");
    printf("Saturn-Specific Options:\n");
    printf("  --dual-cpu        Enable dual SH-2 CPU code generation\n");
    printf("  --linear-scan     Use linear scan register allocation\n");
    printf("\n");
    printf("Examples:\n");
    printf("  kcc game.c -o game.s\n");
    printf("  kcc game.c --dual-cpu -o game.s\n");
    printf("\n");
}

// ============================================================================
// Argument Parsing
// ============================================================================

static bool parse_saturn_arguments(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--dual-cpu") == 0) {
            saturn_opts.dual_cpu = true;
        } else if (strcmp(argv[i], "--linear-scan") == 0) {
            saturn_opts.use_linear_scan = true;
        }
    }
    return true;
}

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char **argv) {
    // Parse Saturn-specific arguments
    parse_saturn_arguments(argc, argv);

    // Print Saturn banner
    printf("KCC for Sega Saturn - SH-2 Code Generator\n");
    if (saturn_opts.dual_cpu) {
        printf("Dual CPU mode enabled\n");
    }
    if (saturn_opts.use_linear_scan) {
        printf("Using linear scan register allocation\n");
    }
    printf("\n");

    // For now, just a simple message
    // The actual compilation will be integrated with existing KCC infrastructure
    printf("Saturn compiler ready.\n");
    printf("This is a placeholder - integrate with existing kcc main.c\n");

    return 0;
}