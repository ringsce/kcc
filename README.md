# KCC - The Kayte C Compiler

KCC is a simple, modular C/Objective-C compiler written from scratch in C. It is designed to be a learning tool for understanding the entire compilation pipeline, from preprocessing and tokenization to parsing, abstract syntax tree (AST) generation, and final assembly code generation.

The compiler is cross-platform and can generate native assembly for both Apple Silicon (ARM64) and Intel/AMD (x86-64) architectures, with growing support for Objective-C language features and comprehensive array support.

## Current Status & Features

The compiler is functional and can compile a substantial subset of the C language with experimental Objective-C support and comprehensive array functionality.

### Core Compiler Features

✅ **Cross-Platform Code Generation**: Automatically generates optimized assembly for ARM64 and x86-64 architectures with native CPU optimizations.

✅ **Complete Frontend**: Includes a robust preprocessor, enhanced lexer, and recursive descent parser.

✅ **AST Generation**: Builds a comprehensive Abstract Syntax Tree to represent source code structures.

✅ **Symbol Table**: Advanced implementation supporting variable scoping, function signatures, and complex type tracking.

✅ **Control Flow**: Supports if/else, while, and for statements with proper code generation.

✅ **Expressions**: Handles arithmetic, comparison, logical, and unary operators with correct precedence.

✅ **Variables & Functions**: Full support for declarations, assignments, parameter passing, and function calls.

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

### Objective-C Lexical Analysis

🆕 **Enhanced Lexer**: Comprehensive support for Objective-C syntax:
- @ tokens and directives (`@interface`, `@implementation`, `@protocol`, etc.)
- Objective-C literals (`@"string"`, `@123`, `@[...]`, `@{...}`)
- Property attributes (`atomic`, `nonatomic`, `retain`, `assign`, etc.)
- Objective-C keywords (`id`, `self`, `super`, `nil`, `YES`, `NO`)
- Foundation types (`NSString`, `NSArray`, `NSDictionary`)

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

### Advanced C Example with Structures

```c
// struct_test.c
typedef struct {
    int x, y;
    char name[20];
} Point;

union Data {
    int i;
    float f;
    char str[4];
};

enum Color {
    RED = 1,
    GREEN,
    BLUE
};

int main() {
    Point points[3] = {
        {0, 0, "Origin"},
        {10, 20, "Point1"}, 
        {30, 40, "Point2"}
    };
    
    union Data data;
    data.i = 42;
    
    enum Color favorite = BLUE;
    
    // Access struct members
    points[1].x = 15;
    
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
./kcc array_test.c -o array_program

# Run the compiled executable
./array_program

# Check the exit code (should be 0 for this example)
echo $?
```

## Project Architecture

The compiler follows a traditional pipeline structure with enhanced support for Objective-C and comprehensive array functionality.

### File Structure:

```
kcc/
├── CMakeLists.txt          # Enhanced with ARM64 Linux support
├── include/                # Public headers for each module
│   ├── kcc.h               # Main project header
│   ├── types.h             # Enhanced type system with arrays
│   ├── lexer.h             # Multi-platform lexer
│   ├── parser.h            # Extended parser with array support
│   ├── ast.h               # AST with complex type nodes
│   ├── preprocessor.h      # Advanced preprocessing
│   ├── codegen.h           # Cross-platform code generator
│   ├── symbol_table.h      # Advanced symbol management
│   ├── error.h             # Comprehensive error reporting
│   └── utils.h             # Utility functions
├── src/                    # Source code implementation
│   ├── main.c              # Command-line interface
│   ├── lexer.c             # Enhanced lexer with @ tokens
│   ├── parser.c            # Extended parser with arrays/structs
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
    ├── structs/            # Structure and union examples
    └── objc/               # Objective-C examples
```

### Compilation Pipeline:

1. **Preprocessor**: Handles `#include`, `#define`, `#import` directives with macro expansion
2. **Lexer**: Tokenizes C and Objective-C source code with comprehensive token recognition
3. **Parser**: Builds AST from tokens supporting arrays, structures, and Objective-C constructs
4. **Semantic Analysis**: Advanced type checking, symbol resolution, and array bounds analysis
5. **Code Generation**: Produces optimized ARM64/x86-64 assembly with runtime support

## Roadmap & Future Work

The foundation is solid with comprehensive array support and enhanced Objective-C lexical analysis. The next steps focus on expanding parsing and code generation capabilities.

### Enhance C Language Support

- [x] **Arrays and pointers** - Complete implementation with bounds checking
- [x] **Structs and unions** - Full support with member access
- [x] **Typedef and enums** - Type aliasing and named constants
- [ ] More complex types (float, long, char arrays, function pointers)
- [ ] for loops and switch statements
- [ ] Advanced pointer operations (pointer-to-pointer, function pointers)

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

## Array and Pointer Feature Details

### Static Arrays
```c
int arr[10];                    // Fixed-size array
int matrix[3][4];               // Multi-dimensional arrays
int initialized[] = {1, 2, 3};  // Array literals with inference
```

### Dynamic Arrays
```c
int size = 10;
int dynamic_arr[size];          // Variable-length arrays
```

### Array Safety Features
- Compile-time size validation
- Optional runtime bounds checking
- Memory leak detection
- Array-to-pointer decay handling

### Pointer Operations
```c
int x = 42;
int *ptr = &x;                  // Address-of operator
int value = *ptr;               // Dereference operator
ptr[0] = 100;                   // Array-style access
```

## Objective-C Feature Support

### ✅ Lexical Analysis (Complete)
- @ symbol recognition and context-aware parsing
- Complete directive support (@interface, @implementation, @protocol, etc.)
- Property attribute parsing (atomic, nonatomic, retain, assign, copy, weak, strong)
- Objective-C literal recognition (@"string", @123, @[...], @{...})
- Foundation type integration (NSString, NSArray, NSDictionary, NSObject)
- Memory management keywords (__strong, __weak, __unsafe_unretained)

### 🚧 Parsing (In Progress)
- Method declaration and implementation parsing
- Interface and implementation block parsing
- Property declaration with attribute handling
- Protocol definition parsing
- Message send syntax recognition

### 📋 Code Generation (Planned)
- Objective-C method dispatch
- Message sending mechanism with runtime integration
- Property accessor generation (getter/setter)
- Memory management code generation
- Runtime type information generation

## Performance & Optimization

### Architecture-Specific Optimizations
- **ARM64**: Native CPU optimization flags (`-mcpu=native`)
- **ARM64**: SIMD vectorization support (`-ftree-vectorize`)
- **Cross-platform**: Optimized calling conventions
- **Memory**: Efficient symbol table with hash-based lookup

### Array Performance Features
- Zero-copy array literals where possible
- Optimized multi-dimensional array indexing
- Efficient bounds checking with minimal overhead
- Memory pool allocation for dynamic arrays

## Contributing

Contributions are welcome! The codebase is well-structured for adding new features:

### Adding Language Features

1. **Lexer**: Add new token types in `include/types.h` and recognition logic in `src/lexer.c`
2. **Parser**: Add parsing functions and grammar rules in `src/parser.c`
3. **AST**: Define new AST node types in `include/types.h` and creation functions in `src/ast.c`
4. **Symbol Table**: Add symbol types and management in `src/symbol_table.c`
5. **Code Generator**: Implement assembly generation in `src/codegen.c` for both ARM64 and x86-64

### Array Feature Contributions

When extending array functionality:
- Maintain bounds checking consistency
- Support both static and dynamic allocation patterns
- Test on both ARM64 and x86-64 architectures
- Consider memory management implications
- Ensure compatibility with existing pointer arithmetic

### Objective-C Contributions

When contributing Objective-C features:
- Ensure lexical tokens are properly defined and recognized
- Build appropriate AST nodes that capture Objective-C semantics
- Consider both traditional and modern Objective-C syntax
- Test with complex Objective-C constructs (inheritance, categories, protocols)
- Maintain compatibility with existing C compilation

### Testing

The project includes comprehensive test suites:
```bash
# Run all tests
make test_all

# Run specific test categories
make test_lexer
make test_parser
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

KCC demonstrates modern compiler design principles while supporting both traditional C programming with comprehensive array functionality and modern Objective-C development paradigms. The compiler serves as both a practical tool and an educational resource for understanding compilation techniques.
