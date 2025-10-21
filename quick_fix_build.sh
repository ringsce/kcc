#!/bin/bash

echo "=== KCC Quick Build Fix ==="

# Add missing includes to files that need them
echo "Adding missing #include statements..."

# Fix ast.c if it doesn't have stdio.h
if ! grep -q "#include <stdio.h>" src/ast.c; then
    echo "Fixing src/ast.c..."
    sed -i.bak '1i\
#include <stdio.h>' src/ast.c
fi

# Fix lexer.c if it doesn't have stdio.h  
if ! grep -q "#include <stdio.h>" src/lexer.c; then
    echo "Fixing src/lexer.c..."
    sed -i.bak '1i\
#include <stdio.h>' src/lexer.c
fi

# Fix parser.c if it doesn't have stdio.h
if ! grep -q "#include <stdio.h>" src/parser.c; then
    echo "Fixing src/parser.c..."
    sed -i.bak '1i\
#include <stdio.h>' src/parser.c
fi

# Fix codegen.c if it doesn't have stdio.h
if ! grep -q "#include <stdio.h>" src/codegen.c; then
    echo "Fixing src/codegen.c..."
    sed -i.bak '1i\
#include <stdio.h>' src/codegen.c
fi

# Fix symbol_table.c if it doesn't have stdio.h
if ! grep -q "#include <stdio.h>" src/symbol_table.c; then
    echo "Fixing src/symbol_table.c..."
    sed -i.bak '1i\
#include <stdio.h>' src/symbol_table.c
fi

# Create minimal CMakeLists.txt for guaranteed build
echo "Creating minimal build configuration..."
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(KCC VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Minimal source files that should build
set(KCC_SOURCES
    src/main.c
    src/lexer.c
    src/parser.c
    src/ast.c
    src/codegen.c
    src/error.c
    src/symbol_table.c
)

# Create the main executable
add_executable(kcc ${KCC_SOURCES})

# Set compiler flags
target_compile_options(kcc PRIVATE -Wall -Wextra)

# Install target
install(TARGETS kcc DESTINATION bin)

message(STATUS "KCC Minimal Build Configuration")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
EOF

echo "Building KCC with minimal configuration..."
mkdir -p build
cd build

# Clean any previous build
rm -rf CMakeCache.txt CMakeFiles/

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ KCC built successfully!"
    echo ""
    echo "Test the compiler:"
    echo "  echo 'int main() { return 42; }' | ./kcc -"
    echo "  ./kcc --help"
    echo ""
else
    echo ""
    echo "❌ Build still failing. Manual fixes may be needed."
    echo ""
    echo "To debug:"
    echo "  make VERBOSE=1"
    echo ""
fi
EOF
