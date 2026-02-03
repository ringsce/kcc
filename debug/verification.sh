#!/bin/bash
# verify-toolchain.sh - Verify the built GCC SH2 toolchain works

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAIN_DIR="${SCRIPT_DIR}/build/toolchain"
TARGET="sh-elf"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}GCC SH2 Toolchain Verification${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if toolchain directory exists
if [ ! -d "${TOOLCHAIN_DIR}" ]; then
    echo -e "${RED}✗ Toolchain directory not found: ${TOOLCHAIN_DIR}${NC}"
    echo ""
    echo "Run build-toolchain-direct.sh first to build the toolchain"
    exit 1
fi

# Add toolchain to PATH
export PATH="${TOOLCHAIN_DIR}/bin:${PATH}"

echo "Toolchain location: ${TOOLCHAIN_DIR}"
echo ""

# Check for required tools
echo -e "${BLUE}Checking toolchain binaries:${NC}"

REQUIRED_TOOLS=(
    "sh-elf-gcc"
    "sh-elf-g++"
    "sh-elf-as"
    "sh-elf-ld"
    "sh-elf-ar"
    "sh-elf-objcopy"
    "sh-elf-objdump"
    "sh-elf-size"
)

ALL_PRESENT=1
for tool in "${REQUIRED_TOOLS[@]}"; do
    if [ -f "${TOOLCHAIN_DIR}/bin/${tool}" ]; then
        echo -e "  ${GREEN}✓${NC} ${tool}"
    else
        echo -e "  ${RED}✗${NC} ${tool} - NOT FOUND"
        ALL_PRESENT=0
    fi
done

if [ $ALL_PRESENT -eq 0 ]; then
    echo ""
    echo -e "${RED}Some tools are missing. Rebuild the toolchain.${NC}"
    exit 1
fi

echo ""

# Check versions
echo -e "${BLUE}Tool versions:${NC}"

if command -v sh-elf-gcc &> /dev/null; then
    GCC_VERSION=$(sh-elf-gcc --version | head -n1)
    echo "  GCC: ${GCC_VERSION}"
else
    echo -e "  ${RED}✗ sh-elf-gcc not in PATH${NC}"
    exit 1
fi

if command -v sh-elf-g++ &> /dev/null; then
    GXX_VERSION=$(sh-elf-g++ --version | head -n1)
    echo "  G++: ${GXX_VERSION}"
else
    echo -e "  ${YELLOW}⚠ sh-elf-g++ not found (C++ support missing)${NC}"
fi

if command -v sh-elf-as &> /dev/null; then
    AS_VERSION=$(sh-elf-as --version | head -n1)
    echo "  AS: ${AS_VERSION}"
fi

echo ""

# Test C compilation
echo -e "${BLUE}Testing C compilation:${NC}"

TEST_DIR=$(mktemp -d)
cd "${TEST_DIR}"

cat > test.c << 'EOF'
int add(int a, int b) {
    return a + b;
}

int main(void) {
    int result = add(5, 3);
    return result;
}
EOF

echo "  Compiling test.c..."
if sh-elf-gcc -m2 -mb -O2 -c test.c -o test.o 2>&1; then
    echo -e "  ${GREEN}✓${NC} Compilation successful"
else
    echo -e "  ${RED}✗${NC} Compilation failed"
    cd "${SCRIPT_DIR}"
    rm -rf "${TEST_DIR}"
    exit 1
fi

echo "  Linking..."
if sh-elf-gcc -m2 -mb -nostdlib test.o -o test.elf 2>&1; then
    echo -e "  ${GREEN}✓${NC} Linking successful"
else
    echo -e "  ${RED}✗${NC} Linking failed"
    cd "${SCRIPT_DIR}"
    rm -rf "${TEST_DIR}"
    exit 1
fi

echo "  Checking output..."
if sh-elf-objdump -h test.elf > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓${NC} ELF file valid"
else
    echo -e "  ${RED}✗${NC} Invalid ELF file"
fi

# Show object details
echo ""
echo "  Object file info:"
sh-elf-size test.o
sh-elf-objdump -t test.o | grep -E "(add|main)" || true

cd "${SCRIPT_DIR}"
rm -rf "${TEST_DIR}"

echo ""

# Test C++ compilation if available
if command -v sh-elf-g++ &> /dev/null; then
    echo -e "${BLUE}Testing C++ compilation:${NC}"
    
    TEST_DIR=$(mktemp -d)
    cd "${TEST_DIR}"
    
    cat > test.cpp << 'EOF'
class Calculator {
public:
    int add(int a, int b) {
        return a + b;
    }
};

int main(void) {
    Calculator calc;
    int result = calc.add(5, 3);
    return result;
}
EOF
    
    echo "  Compiling test.cpp..."
    if sh-elf-g++ -m2 -mb -O2 -fno-exceptions -fno-rtti -c test.cpp -o test.o 2>&1; then
        echo -e "  ${GREEN}✓${NC} C++ compilation successful"
    else
        echo -e "  ${YELLOW}⚠${NC} C++ compilation failed (may need libstdc++)"
    fi
    
    cd "${SCRIPT_DIR}"
    rm -rf "${TEST_DIR}"
    echo ""
fi

# Check for standard libraries
echo -e "${BLUE}Checking standard libraries:${NC}"

if [ -f "${TOOLCHAIN_DIR}/${TARGET}/lib/libc.a" ]; then
    echo -e "  ${GREEN}✓${NC} libc.a (Newlib C library)"
    LIBC_SIZE=$(du -h "${TOOLCHAIN_DIR}/${TARGET}/lib/libc.a" | cut -f1)
    echo "    Size: ${LIBC_SIZE}"
else
    echo -e "  ${RED}✗${NC} libc.a missing"
fi

if [ -f "${TOOLCHAIN_DIR}/${TARGET}/lib/libm.a" ]; then
    echo -e "  ${GREEN}✓${NC} libm.a (Math library)"
else
    echo -e "  ${YELLOW}⚠${NC} libm.a missing"
fi

if [ -f "${TOOLCHAIN_DIR}/${TARGET}/lib/libg.a" ]; then
    echo -e "  ${GREEN}✓${NC} libg.a (Debug library)"
else
    echo -e "  ${YELLOW}⚠${NC} libg.a missing (optional)"
fi

echo ""

# Check for headers
echo -e "${BLUE}Checking standard headers:${NC}"

HEADER_DIR="${TOOLCHAIN_DIR}/${TARGET}/include"
if [ -d "${HEADER_DIR}" ]; then
    echo -e "  ${GREEN}✓${NC} Header directory exists: ${HEADER_DIR}"
    
    IMPORTANT_HEADERS=(
        "stdio.h"
        "stdlib.h"
        "string.h"
        "stdint.h"
        "stdbool.h"
    )
    
    for header in "${IMPORTANT_HEADERS[@]}"; do
        if [ -f "${HEADER_DIR}/${header}" ]; then
            echo -e "    ${GREEN}✓${NC} ${header}"
        else
            echo -e "    ${YELLOW}⚠${NC} ${header} missing"
        fi
    done
else
    echo -e "  ${RED}✗${NC} Header directory missing"
fi

echo ""

# Final summary
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Verification Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Your GCC SH2 toolchain is working!"
echo ""
echo "Next steps:"
echo "  1. Add to PATH:"
echo "     export PATH=${TOOLCHAIN_DIR}/bin:\$PATH"
echo ""
echo "  2. Compile for Sega Saturn:"
echo "     sh-elf-gcc -m2 -mb -O2 myfile.c -o myfile.elf"
echo ""
echo "  3. Create binary:"
echo "     sh-elf-objcopy -O binary myfile.elf myfile.bin"
echo ""
echo "  4. Build KCC with Saturn support:"
echo "     cd ${SCRIPT_DIR}"
echo "     mkdir build && cd build"
echo "     cmake .. -DKCC_TARGET_SATURN=ON -DBUILD_SATURN_TOOLS=ON"
echo "     make"
echo ""
