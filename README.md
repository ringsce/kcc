# KCC - The Kayte C Compiler

KCC is a simple, modular C and Objective-C compiler built from the ground up in C. It is designed as an educational tool for exploring the entire compilation pipeline, from preprocessing and lexical analysis to parsing, Abstract Syntax Tree (AST) generation, and native assembly code generation.

The compiler is cross-platform, generating native assembly for both **Apple Silicon (ARM64)** and **Intel/AMD (x86-64)** architectures.

-----

## Features & Current Status

KCC is under active development. The core C compiler is functional, with a growing set of advanced features and experimental support for Objective-C.

### ✅ Core Compiler Features

  * **Cross-Platform Code Generation**: Generates assembly for ARM64 and x86-64 architectures.
  * **Complete Frontend**: Features a robust preprocessor, lexer, and a recursive descent parser.
  * **Abstract Syntax Tree (AST)**: Builds a comprehensive AST to represent the source code's structure.
  * **Symbol Table**: Manages variable scoping, function signatures, and complex type tracking.
  * **Control Flow**: Full support for `if`/`else`, `while`, and `for` statements.
  * **Expressions**: Correctly handles arithmetic, comparison, logical, and unary operators according to standard precedence.
  * **Variables & Functions**: Full support for declarations, assignments, parameter passing, and function calls.

### 🆕 Advanced Type System

  * **Complete Numeric Types**: Full support for all C numeric types (`int`, `long`, `float`, `double`, `char`, `short`, and their `unsigned` variants) and literal suffixes (`123L`, `456UL`, `3.14f`).
  * **Function Pointers**: First-class support for declaring, assigning, and invoking function pointers, including arrays of function pointers.
  * **Type Operations**: Implements explicit type casting (e.g., `(int)3.14f`) and a `sizeof` operator that works on both types and variables.
  * **Enhanced String Handling**: Treats string literals as character arrays with proper null terminator handling.

### 🆕 Array & Pointer Support

  * **Static Arrays**: Full support for single and multi-dimensional arrays, including literal initializers (`int arr[] = {1, 2, 3}`).
  * **Dynamic Arrays**: Integrates with `malloc`/`free` and includes optional runtime bounds checking.
  * **Pointer Arithmetic**: Complete support for the address-of (`&`) and dereference (`*`) operators, along with pointer arithmetic.

### 🆕 Complex Data Types

  * **Structures**: `struct` declarations with nested members and member access via dot notation.
  * **Unions**: `union` support for shared memory type definitions.
  * **Enums**: `enum` support for creating type-safe named constants.
  * **Typedef**: `typedef` for creating custom type aliases to improve code readability.

### 🚧 Objective-C Support (Experimental)

  * **Lexical Analysis**: (Complete) Recognizes Objective-C keywords (`@interface`, `id`, `self`), literals (`@"string"`), and directives.
  * **Parser Integration**: (In Progress) Can parse basic interface/implementation blocks and method declarations.
  * **Code Generation**: (Experimental) ⚠️ Code generation is limited and requires linking against an Objective-C runtime. Full compilation of Objective-C code is not yet supported.

-----

## Quick Start

### Prerequisites

  * A C compiler (e.g., Clang, GCC)
  * CMake (version 3.16 or later)

### Build Instructions

1.  **Clone the repository:**

    ```bash
    git clone https://www.github.com/ringsce/kcc.git
    cd kcc
    ```

2.  **Create a build directory:**

    ```bash
    mkdir build && cd build
    ```

3.  **Configure with CMake:**

    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

4.  **Compile the project:**

    ```bash
    make
    ```

5.  **Run tests (optional):**

    ```bash
    make test_all
    ```

### Usage Examples

```bash
# Compile a C file into an executable named 'a.out'
./kcc ../examples/test.c

# Specify an output file name
./kcc ../examples/test.c -o my_program

# Enable verbose output to view compilation stages
./kcc -v ../examples/test.c

# Run the preprocessor only
./kcc -E ../examples/test.c

# Enable runtime bounds checking for arrays
./kcc -bounds-check ../examples/array_program.c

# Debug mode: print tokens and the AST
./kcc -d ../examples/test.c
```

-----

## Code Examples

### Advanced Types (`complex_types.c`)

```c
#include <stdio.h>

// Define a function pointer type
typedef int (*BinaryOp)(int, int);

// Functions to be used with the function pointer
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }

int main() {
    // Advanced numeric types with suffixes
    long long huge_number = 9223372036854775807LL;
    float precision = 3.14159f;
    
    // String literal treated as a character array
    char message[] = "Advanced C Compiler";
    
    // Array of function pointers
    BinaryOp operations[] = {add, multiply};
    
    // Invoke a function via a pointer
    int result = operations[0](15, 3); // 15 + 3
    printf("Result: %d\n", result);
    
    // Type casting
    int truncated = (int)42.7f;
    printf("Truncated float: %d\n", truncated);
    
    // Sizeof operator
    printf("Size of long long: %lu bytes\n", (unsigned long)sizeof(long long));
    
    return 0;
}
```

### Arrays and Pointers (`array_test.c`)

```c
#include <stdio.h>
#define SIZE 5

int main() {
    // Static array with an initializer list
    int numbers[SIZE] = {10, 20, 30, 40, 50};
    
    // Multi-dimensional array
    int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};
    
    // Access and modify array elements
    for (int i = 0; i < SIZE; i++) {
        numbers[i] *= 2;
    }
    
    // Pointer arithmetic to modify an array element
    int *ptr = &numbers[0];
    *(ptr + 1) = 100; // Equivalent to numbers[1] = 100
    
    printf("numbers[1] = %d\n", numbers[1]);
    printf("matrix[1][2] = %d\n", matrix[1][2]);
    
    return 0;
}
```

-----

## Project Structure

The codebase is organized into modules for clarity and maintainability.

```
kcc/
├── CMakeLists.txt          # Build configuration
├── include/                # Public headers for each module
│   ├── kcc.h               # Main project header
│   ├── ast.h               # AST node definitions
│   ├── codegen.h           # Code generator interface
│   ├── lexer.h             # Lexer interface
│   ├── parser.h            # Parser interface
│   └── ...                 # Other headers
├── src/                    # Source code implementations
│   ├── main.c              # Command-line driver
│   ├── ast.c               # AST construction logic
│   ├── codegen.c           # ARM64 & x86-64 code generation
│   ├── lexer.c             # Tokenization logic
│   ├── parser.c            # Parsing logic
│   └── ...                 # Other source files
├── tests/                  # Unit and integration tests
└── examples/               # Example C and Objective-C programs
```

-----

## Roadmap

### High Priority (C Language)

  - [x] Complete arrays and pointers.
  - [x] Full support for `struct`, `union`, `enum`, and `typedef`.
  - [x] Full support for numeric types, function pointers, `sizeof`, and casting.
  - [ ] Implement `switch` statements.
  - [ ] Add support for `const` and `volatile` qualifiers.
  - [ ] Add support for `static` and `extern` storage classes.

### In Progress (Objective-C)

  - [ ] Complete parsing for `[object method:param]` message-sending syntax.
  - [ ] Implement property synthesis (`@synthesize`).
  - [ ] Add protocol conformance checking.
  - [ ] Begin integration with an Objective-C runtime (e.g., for `objc_msgSend`).
  - [ ] Implement basic Automatic Reference Counting (ARC).

### Future Work (Compiler Internals)

  - [ ] Add optimization passes (e.g., constant folding, dead code elimination).
  - [ ] Improve standard library integration (`printf`, `malloc`, etc.).
  - [ ] Enhance error reporting with more descriptive messages.
  - [ ] Build out a comprehensive test suite with code coverage metrics.

-----

## Known Limitations

  * **Objective-C support is highly experimental** and cannot produce a fully linked executable yet.
  * **No optimizations** are performed; the generated assembly is unoptimized.
  * **Limited standard library support.**
  * The preprocessor has limited support for complex macros.
  * Inline assembly is not supported.

## Contributing

Contributions are welcome\! To add a new feature:

1.  **Lexer**: Add new token types in `include/lexer.h` and recognition logic in `src/lexer.c`.
2.  **Parser**: Define grammar rules and parsing functions in `src/parser.c`.
3.  **AST**: Create new AST node types in `include/ast.h` and construction logic in `src/ast.c`.
4.  **Code Generator**: Implement assembly generation for both ARM64 and x86-64 in `src/codegen.c`.
5.  **Tests**: Add unit tests for your new feature in the `tests/` directory.

## License

This project is open source. Please see the `LICENSE` file for details.

