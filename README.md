# KCC - The Kayte C Compiler

KCC is a simple, modular C/Objective-C compiler written from scratch in C. It is designed to be a learning tool for understanding the entire compilation pipeline, from preprocessing and tokenization to parsing, abstract syntax tree (AST) generation, and final assembly code generation.

The compiler is cross-platform and can generate native assembly for both Apple Silicon (ARM64) and Intel/AMD (x86-64) architectures, with growing support for Objective-C language features, comprehensive array support, and advanced type system.

## Current Status & Features

The compiler is functional and can compile a substantial subset of the C language with experimental Objective-C support, comprehensive array functionality, and advanced type system including complex numeric types and function pointers.

### Core Compiler Features

✅ **Cross-Platform Code Generation**: Automatically generates optimized assembly for ARM64 and x86-64 architectures with native CPU optimizations.

✅ **Complete Frontend**: Includes a robust preprocessor, enhanced lexer, and recursive descent parser.

✅ **AST Generation**: Builds a comprehensive Abstract Syntax Tree to represent source code structures.

✅ **Symbol Table**: Advanced implementation supporting variable scoping, function signatures, and complex type tracking.

✅ **Control Flow**: Supports if/else, while, and for statements with proper code generation.

✅ **Expressions**: Handles arithmetic, comparison, logical, and unary operators with correct precedence.

✅ **Variables & Functions**: Full support for declarations, assignments, parameter passing, and function calls.

### Advanced Type System

🆕 **Complete Numeric Types**: Full support for all C numeric types
- Integer variants: `int`, `long`, `long long`, `short`
- Unsigned variants: `unsigned int`, `unsigned long`, `unsigned short`
- Floating point: `float`, `double`, `long double`
- Character types: `char`, `signed char`, `unsigned char`
- Type suffixes: `123L`, `456UL`, `3.14f`, literals with proper type inference

🆕 **Function Pointers**: First-class function pointer support
- Declaration syntax: `int (*func)(int, int)`
- Assignment and invocation: `func = my_function; result = func(1, 2)`
- Arrays of function pointers: `int (*operations[10])(int, int)`
- Variadic function pointers: `void (*logger)(const char*, ...)`

🆕 **Type Casting & Sizeof**: Advanced type operations
- Explicit casting: `(int)3.14f`, `(float)my_integer`
- Sizeof operator: `sizeof(int)`, `sizeof(my_array)`
- Compile-time size calculation for arrays and structures
- Runtime type information preservation

🆕 **Enhanced String Handling**: Comprehensive character array support
- String literals as character arrays: `char str[] = "Hello"`
- Null terminator handling and bounds checking
- String manipulation through pointer arithmetic
- Mixed character and string operations

### Array & Pointer Support

🆕 **Static Arrays**: Full support for single and multi-dimensional arrays
- Declaration syntax: `int arr[10]`, `int matrix[3][4]`
- Array literals: `int arr[] = {1, 2, 3, 4, 5}`
- Compile-time bounds checking and size calculation

🆕 **Dynamic Arrays**: Runtime-allocated arrays with safety features
- Dynamic allocation with `malloc`/`free` integration
- Runtime bounds checking with error reporting
- Automatic memory management helpers

🆕 **Array Access**: Safe array indexing with bounds validation
- Standard bracket notation: `arr[index]`
- Multi-dimensional access: `matrix[i][j]`
- Optional runtime bounds checking

🆕 **Pointer Arithmetic**: Complete pointer manipulation support
- Address-of operator: `&variable`
- Dereference operator: `*pointer`
- Pointer arithmetic and array-pointer equivalence

### Complex Type System

🆕 **Structure Support**: User-defined types with member access
- `struct` declarations with nested members
- Member access via dot notation
- Bitfield support for memory optimization

🆕 **Union Support**: Shared memory type definitions
- `union` declarations and usage
- Memory layout optimization

🆕 **Enum Support**: Named constant definitions
- `enum` with automatic and explicit value assignment
- Type-safe constant usage

🆕 **Typedef Support**: Type aliasing and abstraction
- Custom type names for complex types
- Enhanced code readability and maintainability

### Objective-C Lexical Analysis

🆕 **Enhanced Lexer**: Comprehensive support for Objective-C syntax:
- @ tokens and directives (`@interface`, `@implementation`, `@protocol`, etc.)
- Objective-C literals (`@"string"`, `@123`, `@[...]`, `@{...}`)
- Property attributes (`atomic`, `nonatomic`, `retain`, `assign`, etc.)
- Objective-C keywords (`id`, `self`, `super`, `nil`, `YES`, `NO`)
- Foundation types (`NSString`, `NSArray`, `NSDictionary`)

## Quick Start & Build Instructions

To build the compiler, you will need a C compiler (like Clang or GCC) and CMake 3.16 or later.

```bash
# 1. Clone the repository (if you haven't already)
# git clone https://www.github.com/ringsce/kcc.git
# cd kcc

# 2. Create a build directory
mkdir build && cd build

# 3. Configure the project with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Compile the project
make

# 5. Run tests (if available)
make test_all
```

This will create the `kcc` executable inside the build directory with optimizations enabled for your target architecture.

## Usage

You can use the KCC compiler from the command line with various options.

```bash
# Compile a simple C program and create an executable named 'a.out'
./kcc ../examples/test.c

# Compile an Objective-C program
./kcc ../examples/test.m

# Compile with array runtime support
./kcc -arrays ../examples/array_test.c

# Compile with type checking enabled
./kcc -typecheck ../examples/complex_types.c

# Specify an output file name
./kcc ../examples/test.c -o my_program

# Enable verbose output to see compilation stages
./kcc -v ../examples/test.c

# Enable debug mode to print tokens and the AST
./kcc -d ../examples/test.c

# Run the preprocessor only and print to standard output
./kcc -E ../examples/test.c

# Enable bounds checking for arrays (debug builds)
./kcc -bounds-check ../examples/array_program.c
```

## Example Programs

### Complex Types Example

Create a file named `complex_types.c`:

```c
// complex_types.c - Advanced type system demonstration
#include <stdio.h>

// Function pointer types
typedef int (*BinaryOp)(int, int);
typedef void (*Logger)(const char*, ...);

// Mathematical operations
int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b ? a / b : 0; }

// Logging function
void debug_log(const char* format, ...) {
    printf("[DEBUG] ");
    // Variadic implementation would go here
}

int main() {
    // Advanced numeric types
    long long huge_number = 9223372036854775807LL;
    unsigned long positive_big = 4294967295UL;
    float precision = 3.14159f;
    long double extended = 3.141592653589793238L;
    
    // Character handling
    char message[] = "Advanced C Compiler";
    unsigned char bytes[4] = {0xFF, 0xFE, 0xFD, 0xFC};
    
    // Function pointer array (calculator operations)
    BinaryOp operations[] = {add, multiply, divide};
    const char* op_names[] = {"+", "*", "/"};
    
    // Demonstrate function pointer usage
    for (int i = 0; i < 3; i++) {
        int result = operations[i](15, 3);
        printf("15 %s 3 = %d\n", op_names[i], result);
    }
    
    // Type casting demonstrations
    float f = 42.7f;
    int truncated = (int)f;
    double promoted = (double)truncated;
    
    // Sizeof operations
    printf("Size of long long: %zu bytes\n", sizeof(long long));
    printf("Size of function pointer: %zu bytes\n", sizeof(BinaryOp));
    printf("Size of message array: %zu bytes\n", sizeof(message));
    
    // Pointer arithmetic with different types
    long* long_ptr = &huge_number;
    unsigned char* byte_ptr = bytes;
    
    printf("Huge number: %lld\n", *long_ptr);
    printf("First byte: 0x%02X\n", *byte_ptr);
    
    return 0;
}
```

### C Example with Arrays

Create a file named `array_test.c`:

```c
// array_test.c
#include <stdio.h>
#define SIZE 5

int main() {
    // Static array declaration
    int numbers[SIZE] = {10, 20, 30, 40, 50};
    int matrix[2][3] = {{1, 2, 3}, {4, 5, 6}};
    
    // Array access and manipulation
    for (int i = 0; i < SIZE; i++) {
        numbers[i] *= 2;
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }
    
    // Pointer arithmetic
    int *ptr = &numbers[0];
    *(ptr + 1) = 100;  // Equivalent to numbers[1] = 100
    
    // Multi-dimensional array access
    printf("matrix[1][2] = %d\n", matrix[1][2]);
    
    return 0;
}
```

### Advanced C Example with Structures and Function Pointers

```c
// advanced_example.c
#include <stdio.h>

// Structure with function pointer
typedef struct {
    char name[20];
    int (*calculate)(int, int);
    double precision;
} Calculator;

// Function pointer array for operations
int add_func(int a, int b) { return a + b; }
int sub_func(int a, int b) { return a - b; }
int mul_func(int a, int b) { return a * b; }

// Array of calculators
Calculator calculators[] = {
    {"Adder", add_func, 1.0},
    {"Subtractor", sub_func, 1.0},
    {"Multiplier", mul_func, 1.0}
};

int main() {
    // Demonstrate complex type interactions
    for (unsigned int i = 0; i < sizeof(calculators)/sizeof(Calculator); i++) {
        Calculator* calc = &calculators[i];
        int result = calc->calculate(10, 5);
        
        printf("%s: 10 op 5 = %d (precision: %.1f)\n", 
               calc->name, result, calc->precision);
    }
    
    // Type casting with function pointers
    typedef int (*GenericFunc)(int, int);
    GenericFunc generic = (GenericFunc)calculators[0].calculate;
    
    printf("Generic call result: %d\n", generic(7, 3));
    
    return 0;
}
```

### Objective-C Example

Create a file named `test.m`:

```c
// test.m
#import <Foundation/Foundation.h>

@interface Calculator : NSObject
@property (nonatomic, strong) NSString *name;
- (int)add:(int)a to:(int)b;
@end

@implementation Calculator
@synthesize name;

- (int)add:(int)a to:(int)b {
    return a + b;
}
@end

int main() {
    Calculator *calc = [[Calculator alloc] init];
    calc.name = @"MyCalculator";
    
    int result = [calc add:5 to:3];
    
    if (result == 8) {
        return 0;  // Success
    }
    return 1;
}
```

Compile and run:

```bash
# Compile the program
./kcc complex_types.c -o complex_program

# Run the compiled executable
./complex_program

# Check the exit code (should be 0 for this example)
echo $?
```

## Project Architecture

The compiler follows a traditional pipeline structure with enhanced support for Objective-C and comprehensive type system.

### File Structure:

```
kcc/
├── CMakeLists.txt          # Enhanced with ARM64 Linux support
├── include/                # Public headers for each module
│   ├── kcc.h               # Main project header
│   ├── types.h             # Enhanced type system with complex types
│   ├── lexer.h             # Multi-platform lexer
│   ├── parser.h            # Extended parser with type support
│   ├── ast.h               # AST with complex type nodes
│   ├── preprocessor.h      # Advanced preprocessing
│   ├── codegen.h           # Cross-platform code generator
│   ├── symbol_table.h      # Advanced symbol management
│   ├── error.h             # Comprehensive error reporting
│   └── utils.h             # Utility functions
├── src/                    # Source code implementation
│   ├── main.c              # Command-line interface
│   ├── lexer.c             # Enhanced lexer with numeric parsing
│   ├── parser.c            # Extended parser with type parsing
│   ├── ast.c               # AST construction and management
│   ├── preprocessor.c      # Macro processing and includes
│   ├── codegen.c           # Multi-platform code generation
│   ├── symbol_table.c      # Symbol resolution and scoping
│   ├── error.c             # Error handling and reporting
│   └── utils.c             # Helper functions
├── tests/                  # Test suite
│   ├── test_lexer.c        # Lexer unit tests
│   ├── test_parser.c       # Parser unit tests
│   └── test_main.c         # Integration tests
└── examples/
    ├── basic/              # Basic C examples
    ├── arrays/             # Array manipulation examples
    ├── types/              # Complex type examples
    ├── structs/            # Structure and union examples
    └── objc/               # Objective-C examples
```

### Compilation Pipeline:

1. **Preprocessor**: Handles `#include`, `#define`, `#import` directives with macro expansion
2. **Lexer**: Tokenizes C and Objective-C source code with comprehensive token recognition
3. **Parser**: Builds AST from tokens supporting arrays, structures, and complex types
4. **Semantic Analysis**: Advanced type checking, symbol resolution, and type compatibility
5. **Code Generation**: Produces optimized ARM64/x86-64 assembly with runtime support

## Roadmap & Future Work

The foundation is solid with comprehensive type system and array support. The next steps focus on expanding parsing and optimization capabilities.

### Enhance C Language Support

- [x] **Arrays and pointers** - Complete implementation with bounds checking
- [x] **Structs and unions** - Full support with member access
- [x] **Typedef and enums** - Type aliasing and named constants
- [x] **Complex numeric types** - Full integer and floating-point type system
- [x] **Function pointers** - Declaration, assignment, and invocation
- [x] **Type casting and sizeof** - Advanced type operations
- [ ] for loops and switch statements
- [ ] Advanced pointer operations (pointer-to-pointer)
- [ ] Volatile and const qualifiers

### Objective-C Parser Development

- [x] **Enhanced lexical analysis** - Complete @ token recognition
- [ ] Parse `@interface` and `@implementation` declarations
- [ ] Handle Objective-C method declarations and calls
- [ ] Support for `@property` and `@synthesize`
- [ ] Message sending syntax `[object method:param]`
- [ ] Protocol declarations and conformance
- [ ] Category support
- [ ] Memory management directives

### Advanced Code Generation

- [x] **Cross-platform assembly generation** - ARM64 and x86-64 support
- [x] **Array runtime support** - Dynamic allocation and bounds checking
- [x] **Complex type code generation** - Function pointers, casting, sizeof
- [ ] Basic optimizations (constant folding, dead code elimination)
- [ ] More robust stack management and calling conventions
- [ ] Global variable support with proper linking
- [ ] Objective-C runtime integration
- [ ] ARC (Automatic Reference Counting) support

### Advanced Preprocessor

- [x] Enhanced token recognition for Objective-C
- [ ] Conditional compilation (`#ifdef`, `#ifndef`, `#endif`)
- [ ] File inclusion with dependency tracking
- [ ] Macro functions with parameters
- [ ] Objective-C specific preprocessor features

### Standard Library Integration

- [ ] Runtime support for standard functions (`printf`, `malloc`, `free`)
- [ ] Foundation framework integration
- [ ] Objective-C runtime functions
- [ ] Custom array runtime library
- [ ] Math library integration for complex numeric operations

## Advanced Type Features

### Numeric Type System
```c
// Integer types with size guarantees
long long huge = 9223372036854775807LL;
unsigned long big = 4294967295UL;
short small = 32767;
unsigned char byte = 255U;

// Floating point with precision control
float f = 3.14159f;
double d = 3.141592653589793;
long double ld = 3.141592653589793238L;
```

### Function Pointer System
```c
// Function pointer declarations
int (*binary_op)(int, int);
void (*callback)(void);
float (*math_func)(float, float);

// Function pointer arrays
int (*operations[])(int, int) = {add, subtract, multiply};

// Complex function pointer types
typedef struct {
    char* name;
    int (*func)(int, int);
} Operation;
```

### Type Operations
```c
// Explicit type casting
int i = (int)3.14f;
float f = (float)42;
void* generic = (void*)&my_variable;

// Sizeof operations
size_t int_size = sizeof(int);
size_t array_size = sizeof(my_array);
size_t struct_size = sizeof(MyStruct);
```

### String and Character Handling
```c
// Character arrays (strings)
char str1[] = "Hello, World!";
char str2[100] = "Fixed size buffer";
char* str3 = "Pointer to string literal";

// Character manipulation
unsigned char bytes[] = {0x41, 0x42, 0x43, 0x00}; // "ABC"
signed char signed_bytes[] = {-128, -1, 0, 127};
```

## Performance & Optimization

### Architecture-Specific Optimizations
- **ARM64**: Native CPU optimization flags (`-mcpu=native`)
- **ARM64**: SIMD vectorization support (`-ftree-vectorize`)
- **Cross-platform**: Optimized calling conventions
- **Memory**: Efficient symbol table with hash-based lookup

### Type System Performance Features
- Compile-time type checking and optimization
- Efficient function pointer dispatch
- Optimized numeric type conversions
- Zero-overhead type casting where possible
- Smart pointer arithmetic optimization

## Contributing

Contributions are welcome! The codebase is well-structured for adding new features:

### Adding Language Features

1. **Lexer**: Add new token types in `include/types.h` and recognition logic in `src/lexer.c`
2. **Parser**: Add parsing functions and grammar rules in `src/parser.c`
3. **AST**: Define new AST node types in `include/types.h` and creation functions in `src/ast.c`
4. **Symbol Table**: Add symbol types and management in `src/symbol_table.c`
5. **Code Generator**: Implement assembly generation in `src/codegen.c` for both ARM64 and x86-64

### Type System Contributions

When extending the type system:
- Ensure type safety and proper error checking
- Support both compile-time and runtime type operations
- Test type compatibility across different architectures
- Consider performance implications of type conversions
- Maintain backward compatibility with existing C standards

### Testing

The project includes comprehensive test suites:
```bash
# Run all tests
make test_all

# Run specific test categories
make test_lexer
make test_parser
make test_types
make test_arrays
```

## Build Configuration

### CMake Options
- `CMAKE_BUILD_TYPE=Release` - Optimized builds with architecture-specific flags
- `CMAKE_BUILD_TYPE=Debug` - Development builds with enhanced error reporting
- ARM64 detection and optimization are automatic

### Platform Support
- **macOS**: Native ARM64 and x86-64 support
- **Linux**: ARM64 and x86-64 with GNU extensions
- **Cross-compilation**: Supported via CMake toolchain files

## License

This project is open source. Please refer to the LICENSE file for details.

---

KCC demonstrates modern compiler design principles while supporting comprehensive C programming with advanced type system, array functionality, and modern Objective-C development paradigms. The compiler serves as both a practical tool and an educational resource for understanding advanced compilation techniques and type system implementation.
