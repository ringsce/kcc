#!/bin/bash

# Multi-Architecture Build Script for KCC
echo "=== KCC Multi-Architecture Compiler Build ==="

# Create build directory
mkdir -p build
cd build || exit 1

# Configure with CMake
echo "Configuring build..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the compiler
echo "Building KCC..."
make -j"$(nproc)"

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ KCC built successfully!"

# Test multi-architecture code generation
echo ""
echo "=== Testing Multi-Architecture Code Generation ==="

# Create output directories
for arch in x86_64-linux x86_64-macos arm64-linux arm64-macos; do
    mkdir -p "output/$arch"
done

echo ""
echo "Generating assembly for different targets..."

# Test with demo program
DEMO="../example/multiarch_demo.c"
if [ -f "$DEMO" ]; then
    echo "📝 Compiling multiarch_demo.c for different targets:"

    # x86_64 Linux
    echo "  → x86_64-linux"
    ./kcc --target=x86_64-linux -S -o output/x86_64-linux/demo.s "$DEMO"

    # x86_64 macOS
    echo "  → x86_64-macos"
    ./kcc --target=x86_64-macos -S -o output/x86_64-macos/demo.s "$DEMO"

    # ARM64 Linux
    echo "  → arm64-linux"
    ./kcc --target=arm64-linux -S -o output/arm64-linux/demo.s "$DEMO"

    # ARM64 macOS (Apple Silicon)
    echo "  → arm64-macos"
    ./kcc --target=arm64-macos -S -o output/arm64-macos/demo.s "$DEMO"
else
    echo "⚠️ Demo program not found at $DEMO"
fi

echo ""
echo "✅ Multi-Architecture code generation tests complete!"
