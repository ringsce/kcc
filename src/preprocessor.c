#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#include "preprocessor.h"
#include "kcc.h"

Preprocessor *preprocessor_create(void) {
    Preprocessor *pp = malloc(sizeof(Preprocessor));
    if (!pp) {
        error_fatal("Memory allocation failed for preprocessor");
        return NULL;
    }

    memset(pp, 0, sizeof(Preprocessor));

    pp->output_capacity = 4096;
    pp->output = malloc(pp->output_capacity);
    if (!pp->output) {
        free(pp);
        error_fatal("Memory allocation failed for preprocessor output");
        return NULL;
    }

    pp->output[0] = '\0';
    pp->output_size = 0;
    pp->current_line = 1;
    pp->skip_lines = false;

    // Add predefined macros
    preprocessor_add_predefined_macros(pp);

    return pp;
}

void preprocessor_destroy(Preprocessor *pp) {
    if (!pp) return;

    // Free include stack
    for (int i = 0; i < pp->include_depth; i++) {
        free(pp->include_stack[i].filename);
        free(pp->include_stack[i].content);
    }

    // Free macro file_defined strings (if they exist in the struct)
    // Note: Currently commented out due to missing field in header
    // for (int i = 0; i < pp->macro_count; i++) {
    //     if (pp->macros[i].file_defined) {
    //         free(pp->macros[i].file_defined);
    //     }
    // }

    free(pp->output);
    free(pp->current_file);
    free(pp);
}

void preprocessor_add_predefined_macros(Preprocessor *pp) {
    // Standard predefined macros
    preprocessor_define_macro(pp, "__KCC__", "1");
    preprocessor_define_macro(pp, "__VERSION__", "\"1.0.0\"");

    // Date and time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char date_str[32];
    char time_str[32];
    strftime(date_str, sizeof(date_str), "\"%b %d %Y\"", tm_info);
    strftime(time_str, sizeof(time_str), "\"%H:%M:%S\"", tm_info);

    preprocessor_define_macro(pp, "__DATE__", date_str);
    preprocessor_define_macro(pp, "__TIME__", time_str);

    // Architecture and system
    preprocessor_define_macro(pp, "__x86_64__", "1");
    preprocessor_define_macro(pp, "__unix__", "1");

    // C standard
    preprocessor_define_macro(pp, "__STDC__", "1");
    preprocessor_define_macro(pp, "__STDC_VERSION__", "201112L");

    // Mark predefined macros
    for (int i = 0; i < pp->macro_count; i++) {
        pp->macros[i].is_predefined = true;
    }
}

char* preprocessor_process_file(Preprocessor* pp, const char* filename) {
    (void)pp; // Suppress unused parameter warning

    char* content = read_file(filename);
    if (!content) {
        return NULL;
    }

    size_t content_len = strlen(content);
    // Allocate extra space for potential additional newlines and safety margin
    size_t result_capacity = content_len * 2 + 1024; // Double size plus safety margin
    char* result = malloc(result_capacity);
    if (!result) {
        free(content);
        return NULL;
    }
    result[0] = '\0';
    size_t result_len = 0;

    // Make a copy for strtok since it modifies the string
    char* content_copy = strdup(content);
    if (!content_copy) {
        free(content);
        free(result);
        return NULL;
    }

    char* line = strtok(content_copy, "\n");
    while (line != NULL) {
        size_t line_len = strlen(line);

        // Check if we have enough space (line + newline + null terminator)
        if (result_len + line_len + 2 >= result_capacity) {
            // Expand buffer
            result_capacity *= 2;
            char* new_result = realloc(result, result_capacity);
            if (!new_result) {
                free(content);
                free(content_copy);
                free(result);
                return NULL;
            }
            result = new_result;
        }

        if (line[0] == '#') {
            // Process directive - don't add to output if it's an include
            if (strncmp(line + 1, "include", 7) == 0) {
                // Skip include lines - don't add them to output
            } else {
                // Handle other directives or add them as-is
                strcpy(result + result_len, line);
                result_len += line_len;
                result[result_len++] = '\n';
                result[result_len] = '\0';
            }
        } else {
            // Regular code line - add to output
            strcpy(result + result_len, line);
            result_len += line_len;
            result[result_len++] = '\n';
            result[result_len] = '\0';
        }
        line = strtok(NULL, "\n");
    }

    free(content);
    free(content_copy);
    return result;
}

char* preprocessor_process_string(Preprocessor* pp, const char* source, const char* filename) {
    pp->current_file = strdup(filename);
    pp->current_line = 1;
    pp->output_size = 0;
    pp->output[0] = '\0';

    char line[MAX_LINE_LENGTH];
    const char* src_ptr = source;

    while (*src_ptr) {
        // Extract line
        int i = 0;
        while (*src_ptr && *src_ptr != '\n' && i < MAX_LINE_LENGTH - 1) {
            line[i++] = *src_ptr++;
        }
        line[i] = '\0';

        if (*src_ptr == '\n') {
            src_ptr++;
        }

        // Process line
        if (preprocessor_should_skip_line(pp)) {
            // Still need to process conditional directives when skipping
            if (preprocessor_is_directive(line)) {
                char *directive = preprocessor_get_directive_name(line);
                if (directive && (strcmp(directive, "endif") == 0 ||
                    strcmp(directive, "else") == 0 ||
                    strcmp(directive, "elif") == 0)) {
                    preprocessor_process_directive(pp, line);
                }
                free(directive);
            }
        } else {
            if (preprocessor_is_directive(line)) {
                if (!preprocessor_process_directive(pp, line)) {
                    preprocessor_error(pp, "Error processing directive: %s", line);
                }
            } else {
                // Expand macros and add to output
                char *expanded = preprocessor_expand_macros(pp, line);
                preprocessor_append_output(pp, expanded);
                preprocessor_append_output(pp, "\n");
                free(expanded);
            }
        }

        pp->current_line++;
    }

    // Check for unmatched conditionals
    if (pp->cond_stack_depth > 0) {
        preprocessor_error(pp, "Unmatched conditional directive");
    }

    return strdup(pp->output);
}

bool preprocessor_define_macro(Preprocessor *pp, const char *name, const char *body) {
    if (!pp || !name || !body) {
        return false;
    }

    // Check for empty name
    if (strlen(name) == 0) {
        return false;
    }

    // Check if we have space for more macros
    if (pp->macro_count >= MAX_MACROS) {
        preprocessor_error(pp, "Too many macros defined");
        return false;
    }

    // Get pointer to the macro structure in the array
    void *macro_ptr = &pp->macros[pp->macro_count++];

    // Access the macro struct directly - assuming standard macro structure
    Macro *macro = (Macro *)macro_ptr;

    // Use safe string copying with bounds checking
    if (!safe_strcpy(macro->name, sizeof(macro->name), name)) {
        pp->macro_count--; // Revert the increment
        preprocessor_error(pp, "Macro name too long: %.50s", name);
        return false;
    }

    // Check if body fits in fixed array
    if (!safe_strcpy(macro->body, sizeof(macro->body), body)) {
        pp->macro_count--; // Revert the increment
        preprocessor_warning(pp, "Macro body truncated: %.50s", name);
        // Continue anyway with truncated body
        strncpy(macro->body, body, sizeof(macro->body) - 1);
        macro->body[sizeof(macro->body) - 1] = '\0';
    }

    macro->type = MACRO_OBJECT;
    macro->param_count = 0;
    macro->is_predefined = false;
    macro->line_defined = pp->current_line;
    // Note: file_defined field removed due to missing field in header

    return true;
}

bool preprocessor_define_function_macro(Preprocessor *pp, const char *name,
                                      const char *params[], int param_count, const char *body) {
    if (pp->macro_count >= MAX_MACROS) {
        preprocessor_error(pp, "Too many macros defined");
        return false;
    }

    if (param_count > MAX_MACRO_PARAMS) {
        preprocessor_error(pp, "Too many macro parameters");
        return false;
    }

    void *macro_ptr = &pp->macros[pp->macro_count++];

    Macro *macro = (Macro *)macro_ptr;

    strncpy(macro->name, name, sizeof(macro->name) - 1);
    macro->name[sizeof(macro->name) - 1] = '\0';

    strncpy(macro->body, body, sizeof(macro->body) - 1);
    macro->body[sizeof(macro->body) - 1] = '\0';

    macro->type = MACRO_FUNCTION;
    macro->param_count = param_count;
    macro->is_predefined = false;
    macro->line_defined = pp->current_line;
    // Note: file_defined field removed due to missing field in header

    for (int i = 0; i < param_count; i++) {
        strncpy(macro->params[i].name, params[i], sizeof(macro->params[i].name) - 1);
        macro->params[i].name[sizeof(macro->params[i].name) - 1] = '\0';
    }

    return true;
}

bool preprocessor_undefine_macro(Preprocessor *pp, const char *name) {
    for (int i = 0; i < pp->macro_count; i++) {
        // Access macro fields through anonymous struct
        void *macro_ptr = &pp->macros[i];
        if (strcmp(((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->name, name) == 0) {
            // Don't allow undefining predefined macros
            if (((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->is_predefined) {
                preprocessor_warning(pp, "Cannot undefine predefined macro '%s'", name);
                return false;
            }

            // Free file_defined string
            if (((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->file_defined) {
                free(((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->file_defined);
            }

            // Shift remaining macros
            for (int j = i; j < pp->macro_count - 1; j++) {
                pp->macros[j] = pp->macros[j + 1];
            }
            pp->macro_count--;
            return true;
        }
    }
    return false;
}

Macro *preprocessor_find_macro(Preprocessor *pp, const char *name) {
    for (int i = 0; i < pp->macro_count; i++) {
        Macro *macro = &pp->macros[i];
        if (strcmp(macro->name, name) == 0) {
            return macro;
        }
    }
    return NULL;
}

bool preprocessor_is_macro_defined(Preprocessor *pp, const char *name) {
    return preprocessor_find_macro(pp, name) != NULL;
}

char *preprocessor_expand_macros(Preprocessor *pp, const char *line) {
    (void)pp; // Suppress unused parameter warning
    return strdup(line); // Simplified for now - just return a copy
}

char *preprocessor_expand_function_macro(Preprocessor *pp, Macro *macro,
                                       const char *args[], int arg_count) {
    (void)args; // Suppress unused parameter warning

    void *macro_ptr = (void *)macro;
    if (arg_count != ((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->param_count) {
        preprocessor_error(pp, "Macro '%s' expects %d arguments, got %d",
                         ((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->name, ((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->param_count, arg_count);
        return strdup(((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->body);
    }

    return strdup(((struct { char name[64]; char body[512]; int type; int param_count; bool is_predefined; int line_defined; char *file_defined; struct { char name[32]; } params[16]; } *)macro_ptr)->body); // Simplified for now
}

char *preprocessor_substitute_params(const char *body, const char *params[],
                                   const char *args[], int param_count) {
    (void)params; // Suppress unused parameter warning
    (void)args; // Suppress unused parameter warning
    (void)param_count; // Suppress unused parameter warning
    return strdup(body); // Simplified for now
}

bool preprocessor_process_directive(Preprocessor *pp, const char *line) {
    char *directive = preprocessor_get_directive_name(line);
    if (!directive) return false;

    bool result = false;

    if (strcmp(directive, "define") == 0) {
        result = preprocessor_handle_define(pp, line);
    } else if (strcmp(directive, "undef") == 0) {
        result = preprocessor_handle_undef(pp, line);
    } else if (strcmp(directive, "include") == 0) {
        result = preprocessor_handle_include(pp, line);
    } else {
        preprocessor_error(pp, "Unknown directive: #%s", directive);
    }

    free(directive);
    return result;
}

bool preprocessor_handle_define(Preprocessor *pp, const char *directive) {
    char *args = preprocessor_get_directive_args(directive);
    if (!args) {
        preprocessor_error(pp, "Invalid #define directive");
        return false;
    }

    char *name = strtok(args, " \t");
    if (!name) {
        free(args);
        preprocessor_error(pp, "Missing macro name in #define");
        return false;
    }

    // Object-like macro
    char *body = strtok(NULL, ""); // Get rest of line
    if (!body) body = "";

    // Skip leading whitespace in body
    while (*body && isspace(*body)) body++;

    bool result = preprocessor_define_macro(pp, name, body);
    free(args);
    return result;
}

bool preprocessor_handle_undef(Preprocessor *pp, const char *directive) {
    char *args = preprocessor_get_directive_args(directive);
    if (!args) {
        preprocessor_error(pp, "Invalid #undef directive");
        return false;
    }

    char *name = strtok(args, " \t\n");
    if (!name) {
        free(args);
        preprocessor_error(pp, "Missing macro name in #undef");
        return false;
    }

    bool result = preprocessor_undefine_macro(pp, name);
    free(args);
    return result;
}

bool preprocessor_handle_include(Preprocessor *pp, const char *directive) {
    (void)pp; // Suppress unused parameter warning
    (void)directive; // Suppress unused parameter warning
    // Simplified - just return true for now
    return true;
}

bool preprocessor_is_directive(const char *line) {
    const char *ptr = line;
    while (*ptr && isspace(*ptr)) ptr++;
    return *ptr == '#';
}

char *preprocessor_get_directive_name(const char *line) {
    const char *ptr = line;
    while (*ptr && isspace(*ptr)) ptr++;
    if (*ptr != '#') return NULL;

    ptr++; // skip '#'
    while (*ptr && isspace(*ptr)) ptr++;

    const char *start = ptr;
    while (*ptr && (isalnum(*ptr) || *ptr == '_')) ptr++;

    if (ptr == start) return NULL;

    char *name = malloc(ptr - start + 1);
    strncpy(name, start, ptr - start);
    name[ptr - start] = '\0';

    return name;
}

char *preprocessor_get_directive_args(const char *line) {
    const char *ptr = line;
    while (*ptr && isspace(*ptr)) ptr++;
    if (*ptr != '#') return NULL;

    ptr++; // skip '#'
    while (*ptr && isspace(*ptr)) ptr++;

    // Skip directive name
    while (*ptr && (isalnum(*ptr) || *ptr == '_')) ptr++;
    while (*ptr && isspace(*ptr)) ptr++;

    return strdup(ptr);
}

char *preprocessor_trim_whitespace(char *str) {
    // Trim leading whitespace
    while (*str && isspace(*str)) str++;

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end = '\0';
        end--;
    }

    return str;
}

char *preprocessor_stringify(const char *text) {
    size_t len = strlen(text);
    char *result = malloc(len * 2 + 3); // Worst case: every char needs escaping + quotes
    char *ptr = result;

    *ptr++ = '"';

    for (size_t i = 0; i < len; i++) {
        if (text[i] == '"' || text[i] == '\\') {
            *ptr++ = '\\';
        }
        *ptr++ = text[i];
    }

    *ptr++ = '"';
    *ptr = '\0';

    return result;
}

void preprocessor_append_output(Preprocessor *pp, const char *text) {
    size_t text_len = strlen(text);
    size_t needed = pp->output_size + text_len + 1;

    if (needed > pp->output_capacity) {
        pp->output_capacity = needed * 2;
        pp->output = realloc(pp->output, pp->output_capacity);
        if (!pp->output) {
            error_fatal("Memory allocation failed for preprocessor output");
            return;
        }
    }

    strcpy(pp->output + pp->output_size, text);
    pp->output_size += text_len;
}

bool preprocessor_should_skip_line(Preprocessor *pp) {
    return pp->skip_lines;
}

void preprocessor_push_conditional(Preprocessor *pp, ConditionalType type, bool condition) {
    if (pp->cond_stack_depth >= 32) {
        preprocessor_error(pp, "Conditional nesting too deep");
        return;
    }

    ConditionalState *cond = &pp->cond_stack[pp->cond_stack_depth++];
    cond->type = type;
    cond->condition_met = condition;
    cond->else_taken = false;
    cond->line_number = pp->current_line;

    pp->skip_lines = !condition;
}

bool preprocessor_pop_conditional(Preprocessor *pp) {
    if (pp->cond_stack_depth == 0) {
        return false;
    }

    pp->cond_stack_depth--;
    pp->skip_lines = preprocessor_should_skip_line(pp);

    return true;
}

int preprocessor_error(Preprocessor* pp, const char* format, ...) {
    fprintf(stderr, "Preprocessor error in %s:%d: ",
            pp->current_file ? pp->current_file : "unknown", pp->current_line);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");

    return 0;
}

int preprocessor_warning(Preprocessor* pp, const char* format, ...) {
    fprintf(stderr, "Preprocessor warning in %s:%d: ",
            pp->current_file ? pp->current_file : "unknown", pp->current_line);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");

    return 0;
}