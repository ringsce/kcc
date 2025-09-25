# KCC - The Kayte C Compiler

KCC is a simple, modular C/Objective-C compiler written from scratch in C. It is designed to be a learning tool for understanding the entire compilation pipeline, from preprocessing and tokenization to parsing, abstract syntax tree (AST) generation, and final assembly code generation.

The compiler is cross-platform and can generate native assembly for both Apple Silicon (ARM64) and Intel/AMD (x86-64) architectures, with growing support for Objective-C language features.

## Current Status & Features

The compiler is functional and can compile a subset of the C language with experimental Objective-C support.

✅ **Cross-Platform Code Generation**: Automatically generates assembly for ARM64 and x86-64.

✅ **Complete Frontend**: Includes a preprocessor, lexer, and recursive descent parser.

✅ **AST Generation**: Builds a full Abstract Syntax Tree to represent the source code.

✅ **Symbol Table**: Basic implementation for variable and function scoping.

✅ **Control Flow**: Supports if/else and while statements.

✅ **Expressions**: Handles arithmetic, comparison, and unary operators.

✅ **Variables & Functions**: Supports declarations, assignments, and calls.

🆕 **Objective-C Lexical Analysis**: Enhanced lexer with support for:
- @ tokens and directives (`@interface`, `@implementation`, `@protocol`, etc.)
- Objective-C literals (`@"string"`, `@123`, `@[...]`, `@{...}`)
- Property attributes (`atomic`, `nonatomic`, `retain`, `assign`, etc.)
- Objective-C keywords (`id`, `self`, `super`, `nil`, `YES`, `NO`)
- Foundation types (`NSString`, `NSArray`, `NSDictionary`)

## Quick Start & Build Instructions

To build the compiler, you will need a C compiler (like Clang or GCC) and CMake.

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
```

This will create the `kcc` executable inside the build directory.

## Usage

You can use the KCC compiler from the command line.

```bash
# Compile a simple C program and create an executable named 'a.out'
./kcc ../examples/test.c

# Compile an Objective-C program
./kcc ../examples/test.m

# Specify an output file name
./kcc ../examples/test.c -o my_program

# Enable verbose output to see compilation stages
./kcc -v ../examples/test.c

# Enable debug mode to print tokens and the AST
./kcc -d ../examples/test.c

# Run the preprocessor only and print to standard output
./kcc -E ../examples/test.c
```

## Example Programs

### C Example

Create a file named `test.c`:

```c
// test.c
#define MAX 100

int main() {
    int x = 42;
    int y = MAX - x;
    
    if (y > 50) {
        return 1;
    } else {
        return 0;
    }
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
./kcc test.c -o test_program

# Run the compiled executable
./test_program

# Check the exit code (should be 0 for this example)
echo $?
```

## Project Architecture

The compiler follows a traditional pipeline structure with enhanced support for Objective-C.

### File Structure:

```
kcc/
├── CMakeLists.txt
├── include/              # Public headers for each module
│   ├── kcc.h             # Main project header
│   ├── types.h           # Type definitions and token enums
│   ├── lexer.h           # Enhanced lexer with Objective-C support
│   ├── parser.h
│   ├── preprocessor.h
│   ├── codegen.h
│   ├── symbol_table.h
│   ├── error.h
│   └── utils.h
├── src/                  # Source code implementation
│   ├── main.c
│   ├── lexer.c           # Enhanced with @ token recognition
│   ├── parser.c
│   ├── ast.c
│   ├── preprocessor.c
│   ├── codegen.c         # Cross-platform code generator
│   ├── symbol_table.c
│   ├── error.c
│   └── utils.c
└── examples/
    ├── test.c
    └── test.m            # Objective-C examples
```

### Compilation Pipeline:

1. **Preprocessor**: Handles `#include`, `#define`, `#import` directives
2. **Lexer**: Tokenizes C and Objective-C source code, recognizing @ symbols and Objective-C literals
3. **Parser**: Builds AST from tokens (C support complete, Objective-C in progress)
4. **Semantic Analysis**: Type checking and symbol resolution
5. **Code Generation**: Produces ARM64/x86-64 assembly

## Roadmap & Future Work

The foundation is solid with enhanced Objective-C lexical support. The next steps are to expand parsing and code generation.

### Enhance C Language Support

- [ ] Arrays and pointers
- [ ] Structs and unions
- [ ] More complex types (float, long, char)
- [ ] for loops and switch statements

### Objective-C Parser Development

- [ ] Parse `@interface` and `@implementation` declarations
- [ ] Handle Objective-C method declarations and calls
- [ ] Support for `@property` and `@synthesize`
- [ ] Message sending syntax `[object method:param]`
- [ ] Protocol declarations and conformance
- [ ] Category support
- [ ] Memory management directives

### Improve Code Generation

- [ ] Basic optimizations (e.g., constant folding)
- [ ] More robust stack management and function call conventions
- [ ] Support for global variables
- [ ] Objective-C runtime integration
- [ ] ARC (Automatic Reference Counting) support

### Advanced Preprocessor

- [x] Enhanced token recognition for Objective-C
- [ ] Conditional compilation (`#ifdef`, `#ifndef`, `#endif`)
- [ ] File inclusion (`#include`, `#import`)
- [ ] Objective-C specific preprocessor features

### Standard Library

- [ ] Basic runtime support for functions like `printf`
- [ ] Foundation framework integration
- [ ] Objective-C runtime functions

## Objective-C Feature Support

### ✅ Lexical Analysis (Complete)
- @ symbol recognition
- @interface, @implementation, @protocol directives
- @property, @synthesize, @dynamic
- Objective-C literals: @"string", @123, @[...], @{...}
- Property attributes: atomic, nonatomic, retain, assign, copy, weak, strong
- Objective-C keywords: id, self, super, nil, YES, NO, instancetype
- Foundation types: NSString, NSArray, NSDictionary, NSObject

### 🚧 Parsing (In Progress)
- Method declarations and implementations
- Interface and implementation parsing
- Property declaration parsing
- Protocol parsing

### 📋 Code Generation (Planned)
- Objective-C method calls
- Message sending mechanism
- Property accessor generation
- Memory management code

## Contributing

Contributions are welcome! To add a new feature:

1. **Lexer**: Add new token types in `include/types.h` and recognition logic in `src/lexer.c`
2. **Parser**: Add new parsing functions and grammar rules in `src/parser.c`
3. **AST**: Add new AST node types in `include/types.h`
4. **Code Generator**: Add the corresponding assembly generation for new AST nodes in `src/codegen.c`, ensuring you provide implementations for both ARM64 and x86-64

### Objective-C Contributions

When contributing Objective-C features:

- Ensure lexical tokens are properly defined in `types.h`
- Add corresponding parsing logic that builds appropriate AST nodes
- Consider both traditional Objective-C and modern syntax
- Test with various Objective-C constructs (classes, categories, protocols)
- Maintain compatibility with existing C compilation

## License

This project is open source. Please refer to the LICENSE file for details.

---

KCC aims to be an educational tool that demonstrates how modern compilers work while supporting both traditional C and modern Objective-C development paradigms.
