# Kayte C Compiler - Test Suite

## Overview

This comprehensive test suite validates all stages of the Kayte C Compiler, from lexical analysis through code generation. The suite includes over 15 test cases across 5 major categories, ensuring your compiler correctly handles valid C code and properly rejects invalid programs.

## Test Categories

### 1. Lexical Analysis Tests
Tests the tokenization phase of compilation:
- **Keywords Recognition**: Validates that reserved words like `int`, `void`, `return`, `if`, `else`, `while` are correctly identified
- **Operators and Delimiters**: Ensures operators (`+`, `-`, `*`, `/`, `=`, `==`, etc.) and punctuation are tokenized
- **Integer Literals**: Checks parsing of numeric constants (42, 0, 999, etc.)
- **Identifiers**: Validates variable and function names with various patterns
- **Comments**: Ensures both line comments (`//`) and block comments (`/* */`) are properly ignored

### 2. Parser Tests
Tests the syntax analysis and AST construction:
- **Function Declarations**: Validates parsing of function signatures and bodies
- **Variable Declarations**: Tests both uninitialized (`int x;`) and initialized (`int y = 10;`) variables
- **Control Flow**: Ensures `if` statements and `while` loops are correctly parsed
- **Expression Parsing**: Validates operator precedence (e.g., `2 + 3 * 4` = 14, not 20)

### 3. Semantic Analysis Tests
Tests type checking and symbol table management:
- **Undeclared Variables**: Should reject code using variables before declaration
- **Redeclaration Errors**: Should catch duplicate variable declarations in the same scope
- **Type Checking**: Validates type compatibility in assignments
- **Return Type Validation**: Ensures functions return values matching their declared type

### 4. Code Generation Tests
Tests assembly/machine code output:
- **Simple Returns**: Basic return statements with constant values
- **Arithmetic Operations**: Complex expressions with multiple operators
- **Variable Storage**: Tests stack allocation and memory management
- **Control Flow**: Validates conditional jumps and branches

### 5. Integration Tests
End-to-end tests of complete programs:
- **Factorial (Iterative)**: Tests loops and arithmetic (expects return value 120 for n=5)
- **Fibonacci Function**: Tests recursive function calls
- **GCD Algorithm**: Tests while loops with complex conditions
- **Nested Control Flow**: Tests nested if-else statements

## Running the Tests

### Interactive Web Interface

The React-based test suite provides a visual interface:

1. Open the artifact in your browser
2. Click "Run All Tests" to execute the entire suite
3. View results with pass/fail indicators for each test
4. See overall statistics and success rates

The web interface simulates test execution and shows how the test framework would behave.

### Python Test Runner

For actual testing of your compiler implementation:

```bash
# Save the Python test suite as test_suite.py
python test_suite.py

# Or specify your compiler path:
python test_suite.py /path/to/kayte-compiler
```

**Requirements:**
- Python 3.6+
- Your Kayte compiler executable (built with CMake)
- Unix-like environment (Linux, macOS, WSL)

**Expected Compiler Interface:**
```bash
./kcc hello.c -o output.out
```

The test runner will:
1. Write test code to temporary files
2. Invoke your compiler
3. Run the compiled executables
4. Verify return codes and output
5. Report pass/fail for each test
6. Clean up temporary files

### Building the Kayte Compiler for Testing

Before running tests, build your compiler:

```bash
# From your project root (e.g., /Users/pedro/CLionProjects/kcc)
mkdir -p build
cd build
cmake ..
make

# The compiler executable will be at: ./build/kcc
# Run tests with:
cd ..
python test_suite.py ./build/kcc
```

**Note**: The compiler warnings you see (unused parameters in ast.c) are normal and don't affect functionality. These are placeholder parameters for future Objective-C features.

## Test Results Interpretation

### Pass Criteria
A test passes when:
- Code that should compile compiles successfully
- Code that shouldn't compile produces appropriate error messages
- Compiled programs produce correct return codes
- Output matches expected values

### Common Failure Reasons
- **Compilation Failed**: Parser error, syntax issue, or semantic error
- **Wrong Return Code**: Program logic error or incorrect code generation
- **Token Mismatch**: Lexer not recognizing tokens correctly
- **Unexpected Token**: Parser receiving tokens in wrong order
- **Type Error**: Semantic analysis incorrectly handling types

## Customizing Tests

### Adding New Tests

To add tests, modify the test data arrays:

```javascript
// Web Interface
const customTests = [
  {
    name: "Your Test Name",
    input: "int main() { return 5; }",
    expected: "Expected behavior description"
  }
];
```

```python
# Python Runner (in test_suite.py)
def get_custom_tests():
    return [
        TestCase(
            name="Your Test Name",
            input_code="int main() { return 5; }",
            should_compile=True,
            expected_return_code=5
        )
    ]
```

### Adapting Tests to Your Compiler's Interface

If your Kayte compiler uses different command-line arguments, modify the `run_compiler()` method in `test_suite.py`:

```python
def run_compiler(self, input_file: str, output_file: str = "test.out") -> Tuple[int, str, str]:
    """Run the compiler and return (return_code, stdout, stderr)"""
    try:
        # Modify this line to match your compiler's interface:
        result = subprocess.run(
            [self.compiler_path, input_file, "-o", output_file],  # Adjust args here
            capture_output=True,
            text=True,
            timeout=5
        )
        return result.returncode, result.stdout, result.stderr
    except FileNotFoundError:
        return -1, "", f"Compiler not found at {self.compiler_path}"
    except subprocess.TimeoutExpired:
        return -1, "", "Compiler timed out"
```

### File Extensions

By default, tests use `.c` extension. If your compiler expects `.c` or another extension, modify:

```python
def write_test_file(self, code: str, filename: str = "test.c") -> str:
    # Change "test.kay" to "test.c" or your preferred extension
    with open(filename, 'w') as f:
        f.write(code)
    return filename
```

### Test Case Structure

Each test includes:
- **name**: Descriptive test name
- **input**: Source code to compile
- **expected_output**: Expected stdout (optional)
- **expected_error**: Expected error message for negative tests
- **should_compile**: Boolean indicating if code should compile successfully
- **expected_return_code**: Expected program exit code

## CI/CD Integration

The Python test suite returns appropriate exit codes for CI/CD:
- **Exit 0**: All tests passed
- **Exit 1**: One or more tests failed

Example GitHub Actions integration:

```yaml
- name: Run Test Suite
  run: python test_suite.py ./kcc
```

## Test Coverage

Current coverage:
- ✅ Basic lexical analysis (tokens, keywords, operators)
- ✅ Parser functionality (declarations, expressions, statements)
- ✅ Semantic checks (scoping, types, declarations)
- ✅ Code generation (arithmetic, control flow, functions)
- ✅ Integration (complete programs with multiple features)

**Not yet covered:**
- Arrays and pointers
- Function parameters and calls
- Complex type systems
- Preprocessor directives
- Multiple translation units
- Standard library functions

## Extending the Suite

To add coverage for new language features:

1. **Add lexer tests** for new tokens/keywords
2. **Add parser tests** for new syntax constructs
3. **Add semantic tests** for new type rules
4. **Add codegen tests** for new operations
5. **Add integration tests** for realistic usage

## Troubleshooting

### Compiler Not Found
```
Error: Compiler not found at './kayte'
```
**Solution**: Specify the correct path: `python test_suite.py /correct/path/to/compiler`

For the Kayte project built with CMake:
```bash
python test_suite.py ./build/kayte
```

### Tests Timing Out
```
Compiler timed out
```
**Solution**: Check for infinite loops in your compiler or increase timeout in `run_compiler()` method

### All Tests Failing
**Solution**: Verify your compiler's command-line interface matches the expected format. Modify the `run_compiler()` method if needed.

### Compiler Warnings During Build
The warnings about unused parameters in `ast.c` (like `ivar_name`, `catch_blocks`, `finally_block`, etc.) are expected. These are placeholder parameters for Objective-C features that will be implemented later. They don't affect the C compilation tests.

### Linking Errors
If you get undefined reference errors during compilation:
- Ensure all source files are compiled (lexer.c, parser.c, ast.c, semantic.c, codegen.c)
- Check your CMakeLists.txt includes all necessary source files
- Make sure you're linking with required libraries (e.g., `-lm` for math functions)

### Wrong Output or Return Code
If tests compile but produce wrong results:
1. Check your code generation logic
2. Verify operator precedence in parser
3. Test individual components (lexer, parser, semantic analyzer) separately
4. Add debug output to see intermediate representations (tokens, AST, etc.)

## Contributing

To contribute new tests:
1. Identify untested language features
2. Write test cases covering edge cases
3. Ensure tests are deterministic and repeatable
4. Document expected behavior clearly
5. Submit with clear descriptions

## License

This test suite is provided as part of the Kayte C Compiler project for educational and development purposes.