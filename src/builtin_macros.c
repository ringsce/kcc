#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "preprocessor.h"

// Add built-in macros to preprocessor
void preprocessor_add_builtin_macros(Preprocessor *pp) {
    // Compiler identification
    preprocessor_define_macro(pp, "__KCC__", "1");
    preprocessor_define_macro(pp, "__KCC_VERSION__", "\"1.0.0\"");

    // Standard compliance
    preprocessor_define_macro(pp, "__STDC__", "1");
    preprocessor_define_macro(pp, "__STDC_VERSION__", "201112L");

    // Date and time macros
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char date_str[32];
    char time_str[32];
    strftime(date_str, sizeof(date_str), "\"%b %d %Y\"", tm_info);
    strftime(time_str, sizeof(time_str), "\"%H:%M:%S\"", tm_info);

    preprocessor_define_macro(pp, "__DATE__", date_str);
    preprocessor_define_macro(pp, "__TIME__", time_str);

    // Platform detection
#ifdef __linux__
    preprocessor_define_macro(pp, "__linux__", "1");
    preprocessor_define_macro(pp, "__unix__", "1");
    preprocessor_define_macro(pp, "__PLATFORM__", "\"Linux\"");
#elif defined(__APPLE__)
    preprocessor_define_macro(pp, "__APPLE__", "1");
    preprocessor_define_macro(pp, "__unix__", "1");
    preprocessor_define_macro(pp, "__PLATFORM__", "\"macOS\"");
#else
    preprocessor_define_macro(pp, "__PLATFORM__", "\"Unknown\"");
#endif

    // Architecture macros
#if defined(__x86_64__) || defined(_M_X64)
    preprocessor_define_macro(pp, "__x86_64__", "1");
    preprocessor_define_macro(pp, "__ARCH__", "\"x86_64\"");
    preprocessor_define_macro(pp, "__POINTER_SIZE__", "8");
#elif defined(__aarch64__)
    preprocessor_define_macro(pp, "__aarch64__", "1");
    preprocessor_define_macro(pp, "__ARCH__", "\"ARM64\"");
    preprocessor_define_macro(pp, "__POINTER_SIZE__", "8");
#endif

    // Common constants
    preprocessor_define_macro(pp, "NULL", "((void*)0)");
    preprocessor_define_macro(pp, "TRUE", "1");
    preprocessor_define_macro(pp, "FALSE", "0");

    // Mathematical constants
    preprocessor_define_macro(pp, "PI", "3.141592653589793");
    preprocessor_define_macro(pp, "E", "2.718281828459045");

    // Memory sizes
    preprocessor_define_macro(pp, "KB", "1024");
    preprocessor_define_macro(pp, "MB", "(1024 * 1024)");
    preprocessor_define_macro(pp, "GB", "(1024 * 1024 * 1024)");

    // Build configuration
#ifdef DEBUG
    preprocessor_define_macro(pp, "BUILD_TYPE", "\"Debug\"");
    preprocessor_define_macro(pp, "DEBUG_BUILD", "1");
#else
    preprocessor_define_macro(pp, "BUILD_TYPE", "\"Release\"");
    preprocessor_define_macro(pp, "RELEASE_BUILD", "1");
#endif

    // Mark as predefined
    for (int i = 0; i < pp->macro_count; i++) {
        pp->macros[i].is_predefined = true;
    }
}

// Add user-defined macros
void preprocessor_add_user_macros(Preprocessor *pp, const char *macro_definitions[], int count) {
    for (int i = 0; i < count; i++) {
        const char *def = macro_definitions[i];
        char *name = strdup(def);
        char *equals = strchr(name, '=');

        if (equals) {
            *equals = '\0';
            char *value = equals + 1;
            preprocessor_define_macro(pp, name, value);
        } else {
            preprocessor_define_macro(pp, name, "1");
        }

        free(name);
    }
}

// Add environment macros (simplified)
void preprocessor_add_environment_macros(Preprocessor *pp) {
    char *user = getenv("USER");
    if (user) {
        char *quoted = malloc(strlen(user) + 3);
        sprintf(quoted, "\"%s\"", user);
        preprocessor_define_macro(pp, "ENV_USER", quoted);
        free(quoted);
    }
}

// Add system macros (simplified)
void preprocessor_add_system_macros(Preprocessor *pp) {
    preprocessor_define_macro(pp, "__SYSTEM_NAME__", "\"Unix\"");
    preprocessor_define_macro(pp, "__MACHINE__", "\"Unknown\"");
}