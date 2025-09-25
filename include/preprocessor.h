#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "types.h"

// Complete Preprocessor struct definition
typedef struct Preprocessor {
    // Macro storage
    Macro macros[MAX_MACROS];
    int macro_count;

    // Conditional compilation stack
    ConditionalState cond_stack[32];
    int cond_stack_depth;

    // Include file stack
    IncludeFile include_stack[MAX_INCLUDE_DEPTH];
    int include_depth;

    // Output buffer
    char *output;
    size_t output_size;
    size_t output_capacity;

    // Current file tracking
    char *current_file;
    int current_line;
    bool skip_lines;
} Preprocessor;

// Preprocessor creation and destruction
Preprocessor *preprocessor_create(void);
void preprocessor_destroy(Preprocessor *preprocessor);

// Main preprocessing functions
char *preprocessor_process_file(Preprocessor *pp, const char *filename);
char *preprocessor_process_string(Preprocessor *pp, const char *source, const char *filename);

// Macro management
bool preprocessor_define_macro(Preprocessor *pp, const char *name, const char *body);
bool preprocessor_define_function_macro(Preprocessor *pp, const char *name,
                                      const char *params[], int param_count, const char *body);
bool preprocessor_undefine_macro(Preprocessor *pp, const char *name);
Macro *preprocessor_find_macro(Preprocessor *pp, const char *name);
bool preprocessor_is_macro_defined(Preprocessor *pp, const char *name);

// Macro expansion
char *preprocessor_expand_macros(Preprocessor *pp, const char *line);
char *preprocessor_expand_function_macro(Preprocessor *pp, Macro *macro,
                                       const char *args[], int arg_count);

// Directive processing
bool preprocessor_process_directive(Preprocessor *pp, const char *line);
bool preprocessor_handle_define(Preprocessor *pp, const char *directive);
bool preprocessor_handle_undef(Preprocessor *pp, const char *directive);
bool preprocessor_handle_include(Preprocessor *pp, const char *directive);
bool preprocessor_handle_if(Preprocessor *pp, const char *directive);
bool preprocessor_handle_ifdef(Preprocessor *pp, const char *directive);
bool preprocessor_handle_ifndef(Preprocessor *pp, const char *directive);
bool preprocessor_handle_elif(Preprocessor *pp, const char *directive);
bool preprocessor_handle_else(Preprocessor *pp, const char *directive);
bool preprocessor_handle_endif(Preprocessor *pp, const char *directive);
bool preprocessor_handle_error(Preprocessor *pp, const char *directive);
bool preprocessor_handle_warning(Preprocessor *pp, const char *directive);
bool preprocessor_handle_pragma(Preprocessor *pp, const char *directive);
bool preprocessor_handle_line(Preprocessor *pp, const char *directive);

// Conditional compilation
bool preprocessor_evaluate_condition(Preprocessor *pp, const char *condition);
bool preprocessor_should_skip_line(Preprocessor *pp);
void preprocessor_push_conditional(Preprocessor *pp, ConditionalType type, bool condition);
bool preprocessor_pop_conditional(Preprocessor *pp);

// Utility functions
char *preprocessor_read_file(const char *filename);
void preprocessor_append_output(Preprocessor *pp, const char *text);
char *preprocessor_parse_macro_args(const char *call, char *args[], int max_args);
char *preprocessor_substitute_params(const char *body, const char *params[],
                                   const char *args[], int param_count);
bool preprocessor_is_directive(const char *line);
char *preprocessor_get_directive_name(const char *line);
char *preprocessor_get_directive_args(const char *line);
void preprocessor_add_predefined_macros(Preprocessor *pp);

// String utilities for macros
char *preprocessor_stringify(const char *text);
char *preprocessor_concatenate(const char *left, const char *right);
char *preprocessor_trim_whitespace(char *str);

// Error handling for preprocessor
int preprocessor_error(Preprocessor* pp, const char* format, ...);
int preprocessor_warning(Preprocessor* pp, const char* format, ...);

// Built-in macro management
void preprocessor_add_builtin_macros(Preprocessor *pp);
void preprocessor_add_user_macros(Preprocessor *pp, const char *macro_definitions[], int count);
void preprocessor_add_environment_macros(Preprocessor *pp);
void preprocessor_add_system_macros(Preprocessor *pp);

// Cleanup
void free_macro(Macro *macro);

#endif // PREPROCESSOR_H