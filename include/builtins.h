//
// Created by Pedro Dias Vicente on 19/10/2025.
//

#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>
#include "types.h"

// Check if a function name is a built-in
bool is_builtin_function(const char *name);

// Get return type of a built-in function
DataType get_builtin_return_type(const char *name);

// Check if a built-in function is variadic
bool is_builtin_variadic(const char *name);

#endif // BUILTINS_H