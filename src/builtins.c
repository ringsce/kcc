#include "kcc.h"
#include "builtins.h"
#include <string.h>

// Built-in function information
typedef struct {
    const char *name;
    DataType return_type;
    int param_count;
    bool is_variadic;
    bool returns_pointer;
} BuiltinFunction;

static const BuiltinFunction builtin_functions[] = {
    // stdio.h
    {"printf", TYPE_INT, 1, true, false},
    {"fprintf", TYPE_INT, 2, true, false},
    {"sprintf", TYPE_INT, 2, true, false},
    {"snprintf", TYPE_INT, 3, true, false},
    {"scanf", TYPE_INT, 1, true, false},
    {"fscanf", TYPE_INT, 2, true, false},
    {"sscanf", TYPE_INT, 2, true, false},
    {"putchar", TYPE_INT, 1, false, false},
    {"puts", TYPE_INT, 1, false, false},
    {"getchar", TYPE_INT, 0, false, false},
    {"gets", TYPE_POINTER, 1, false, true},
    {"fgets", TYPE_POINTER, 3, false, true},
    {"fputs", TYPE_INT, 2, false, false},
    {"fopen", TYPE_POINTER, 2, false, true},
    {"fclose", TYPE_INT, 1, false, false},
    {"fflush", TYPE_INT, 1, false, false},
    {"fread", TYPE_INT, 4, false, false},
    {"fwrite", TYPE_INT, 4, false, false},
    {"fseek", TYPE_INT, 3, false, false},
    {"ftell", TYPE_LONG, 1, false, false},
    {"rewind", TYPE_VOID, 1, false, false},
    {"remove", TYPE_INT, 1, false, false},
    {"rename", TYPE_INT, 2, false, false},
    
    // stdlib.h
    {"malloc", TYPE_POINTER, 1, false, true},
    {"calloc", TYPE_POINTER, 2, false, true},
    {"realloc", TYPE_POINTER, 2, false, true},
    {"free", TYPE_VOID, 1, false, false},
    {"exit", TYPE_VOID, 1, false, false},
    {"abort", TYPE_VOID, 0, false, false},
    {"atexit", TYPE_INT, 1, false, false},
    {"atoi", TYPE_INT, 1, false, false},
    {"atol", TYPE_LONG, 1, false, false},
    {"atoll", TYPE_LONG_LONG, 1, false, false},
    {"atof", TYPE_DOUBLE, 1, false, false},
    {"strtol", TYPE_LONG, 3, false, false},
    {"strtoul", TYPE_UNSIGNED_LONG, 3, false, false},
    {"strtod", TYPE_DOUBLE, 2, false, false},
    {"rand", TYPE_INT, 0, false, false},
    {"srand", TYPE_VOID, 1, false, false},
    {"abs", TYPE_INT, 1, false, false},
    {"labs", TYPE_LONG, 1, false, false},
    {"getenv", TYPE_POINTER, 1, false, true},
    {"system", TYPE_INT, 1, false, false},
    
    // string.h
    {"strlen", TYPE_INT, 1, false, false},
    {"strcpy", TYPE_POINTER, 2, false, true},
    {"strncpy", TYPE_POINTER, 3, false, true},
    {"strcat", TYPE_POINTER, 2, false, true},
    {"strncat", TYPE_POINTER, 3, false, true},
    {"strcmp", TYPE_INT, 2, false, false},
    {"strncmp", TYPE_INT, 3, false, false},
    {"strchr", TYPE_POINTER, 2, false, true},
    {"strrchr", TYPE_POINTER, 2, false, true},
    {"strstr", TYPE_POINTER, 2, false, true},
    {"strdup", TYPE_POINTER, 1, false, true},
    {"memcpy", TYPE_POINTER, 3, false, true},
    {"memmove", TYPE_POINTER, 3, false, true},
    {"memcmp", TYPE_INT, 3, false, false},
    {"memset", TYPE_POINTER, 3, false, true},
    {"memchr", TYPE_POINTER, 3, false, true},
    
    // math.h
    {"sqrt", TYPE_DOUBLE, 1, false, false},
    {"pow", TYPE_DOUBLE, 2, false, false},
    {"sin", TYPE_DOUBLE, 1, false, false},
    {"cos", TYPE_DOUBLE, 1, false, false},
    {"tan", TYPE_DOUBLE, 1, false, false},
    {"exp", TYPE_DOUBLE, 1, false, false},
    {"log", TYPE_DOUBLE, 1, false, false},
    {"log10", TYPE_DOUBLE, 1, false, false},
    {"ceil", TYPE_DOUBLE, 1, false, false},
    {"floor", TYPE_DOUBLE, 1, false, false},
    {"fabs", TYPE_DOUBLE, 1, false, false},
    
    // ctype.h
    {"isalpha", TYPE_INT, 1, false, false},
    {"isdigit", TYPE_INT, 1, false, false},
    {"isalnum", TYPE_INT, 1, false, false},
    {"isspace", TYPE_INT, 1, false, false},
    {"isupper", TYPE_INT, 1, false, false},
    {"islower", TYPE_INT, 1, false, false},
    {"toupper", TYPE_INT, 1, false, false},
    {"tolower", TYPE_INT, 1, false, false},
    
    // time.h
    {"time", TYPE_LONG, 1, false, false},
    {"ctime", TYPE_POINTER, 1, false, true},
    {"localtime", TYPE_POINTER, 1, false, true},
    {"gmtime", TYPE_POINTER, 1, false, true},
    {"mktime", TYPE_LONG, 1, false, false},
    
    {NULL, TYPE_UNKNOWN, 0, false, false} // Sentinel
};

bool is_builtin_function(const char *name) {
    for (int i = 0; builtin_functions[i].name != NULL; i++) {
        if (strcmp(builtin_functions[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

const BuiltinFunction *get_builtin_function(const char *name) {
    for (int i = 0; builtin_functions[i].name != NULL; i++) {
        if (strcmp(builtin_functions[i].name, name) == 0) {
            return &builtin_functions[i];
        }
    }
    return NULL;
}

DataType get_builtin_return_type(const char *name) {
    const BuiltinFunction *func = get_builtin_function(name);
    return func ? func->return_type : TYPE_UNKNOWN;
}

bool is_builtin_variadic(const char *name) {
    const BuiltinFunction *func = get_builtin_function(name);
    return func ? func->is_variadic : false;
}