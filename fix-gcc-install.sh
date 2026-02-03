#!/bin/bash
# continue-build.sh - Continue building from where we left off

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_ROOT="${SCRIPT_DIR}/build/toolchain-build"
PREFIX="${SCRIPT_DIR}/build/toolchain"
SRC="${BUILD_ROOT}/src"
LOG_DIR="${BUILD_ROOT}/logs"
TARGET="sh-elf"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Continuing Toolchain Build${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Setup environment
export CC="/usr/bin/clang"
export CXX="/usr/bin/clang++"
export CFLAGS="-O2"
export CXXFLAGS="-O2"
export PATH="${PREFIX}/bin:${PATH}"

NPROC=$(sysctl -n hw.ncpu)
MAKE_CMD="gmake"
command -v gmake &> /dev/null || MAKE_CMD="make"

mkdir -p "${LOG_DIR}"

echo "Configuration:"
echo "  Make: ${MAKE_CMD}"
echo "  Cores: ${NPROC}"
echo "  Prefix: ${PREFIX}"
echo ""

# Check what's already done
echo -e "${BLUE}Checking existing installation:${NC}"

if [ -f "${PREFIX}/bin/${TARGET}-as" ]; then
    echo -e "  ${GREEN}✓${NC} Binutils installed"
    NEED_BINUTILS=0
else
    echo -e "  ${YELLOW}⚠${NC} Binutils NOT installed"
    NEED_BINUTILS=1
fi

if [ -f "${PREFIX}/bin/${TARGET}-gcc" ]; then
    echo -e "  ${GREEN}✓${NC} GCC Stage 1 installed"
    NEED_GCC1=0
else
    echo -e "  ${YELLOW}⚠${NC} GCC Stage 1 NOT installed"
    NEED_GCC1=1
fi

if [ -f "${PREFIX}/${TARGET}/lib/libc.a" ]; then
    echo -e "  ${GREEN}✓${NC} Newlib installed"
    NEED_NEWLIB=0
else
    echo -e "  ${YELLOW}⚠${NC} Newlib NOT installed"
    NEED_NEWLIB=1
fi

if [ -f "${PREFIX}/bin/${TARGET}-g++" ]; then
    echo -e "  ${GREEN}✓${NC} GCC Stage 2 installed"
    NEED_GCC2=0
else
    echo -e "  ${YELLOW}⚠${NC} GCC Stage 2 NOT installed"
    NEED_GCC2=1
fi

echo ""

# Build what's missing
if [ $NEED_GCC1 -eq 1 ]; then
    echo -e "${BLUE}=== Building GCC Stage 1 ===${NC}"

    # Download prerequisites
    if [ ! -d "${SRC}/gcc-13.2.0/gmp" ]; then
        echo "Downloading GCC prerequisites..."
        cd "${SRC}/gcc-13.2.0"
        ./contrib/download_prerequisites | tee "${LOG_DIR}/gcc-prereq.log"
    fi

    rm -rf "${BUILD_ROOT}/build-gcc-stage1"
    mkdir -p "${BUILD_ROOT}/build-gcc-stage1"
    cd "${BUILD_ROOT}/build-gcc-stage1"

    echo "Configuring..."
    "${SRC}/gcc-13.2.0/configure" \
        --target=${TARGET} \
        --prefix="${PREFIX}" \
        --with-cpu=m2 \
        --with-endian=big \
        --enable-languages=c \
        --without-headers \
        --disable-nls \
        --disable-shared \
        --disable-threads \
        --disable-libssp \
        --disable-libgomp \
        --with-newlib \
        --with-system-zlib \
        2>&1 | tee "${LOG_DIR}/gcc1-configure.log"

    echo "Building... (30 minutes)"
    ${MAKE_CMD} -j${NPROC} all-gcc 2>&1 | tee "${LOG_DIR}/gcc1-build.log"

    echo "Installing..."
    ${MAKE_CMD} install-gcc 2>&1 | tee "${LOG_DIR}/gcc1-install.log" || {
        echo "Trying manual install..."
        mkdir -p "${PREFIX}/bin" "${PREFIX}/libexec/gcc/${TARGET}/13.2.0"
        cp gcc/xgcc "${PREFIX}/bin/${TARGET}-gcc" && chmod +x "${PREFIX}/bin/${TARGET}-gcc"
        cp gcc/cc1 "${PREFIX}/libexec/gcc/${TARGET}/13.2.0/" && chmod +x "${PREFIX}/libexec/gcc/${TARGET}/13.2.0/cc1"
        cp gcc/cpp "${PREFIX}/bin/${TARGET}-cpp" && chmod +x "${PREFIX}/bin/${TARGET}-cpp"
    }

    if [ -f "${PREFIX}/bin/${TARGET}-gcc" ]; then
        echo -e "${GREEN}✓ GCC Stage 1 installed${NC}"
    else
        echo -e "${RED}✗ Failed${NC}"
        exit 1
    fi
    echo ""
fi

if [ $NEED_NEWLIB -eq 1 ]; then
    echo -e "${BLUE}=== Building Newlib ===${NC}"

    rm -rf "${BUILD_ROOT}/build-newlib"
    mkdir -p "${BUILD_ROOT}/build-newlib"
    cd "${BUILD_ROOT}/build-newlib"

    echo "Configuring..."
    "${SRC}/newlib-4.3.0.20230120/configure" \
        --target=${TARGET} \
        --prefix="${PREFIX}" \
        --enable-newlib-hw-fp \
        --disable-newlib-supplied-syscalls \
        --disable-nls \
        2>&1 | tee "${LOG_DIR}/newlib-configure.log"

    echo "Building... (15 minutes)"
    ${MAKE_CMD} -j${NPROC} 2>&1 | tee "${LOG_DIR}/newlib-build.log"

    echo "Installing..."
    ${MAKE_CMD} install 2>&1 | tee "${LOG_DIR}/newlib-install.log"

    if [ -f "${PREFIX}/${TARGET}/lib/libc.a" ]; then
        echo -e "${GREEN}✓ Newlib installed${NC}"
    else
        echo -e "${RED}✗ Failed${NC}"
        exit 1
    fi
    echo ""
fi

if [ $NEED_GCC2 -eq 1 ]; then
    echo -e "${BLUE}=== Building GCC Stage 2 ===${NC}"

    rm -rf "${BUILD_ROOT}/build-gcc-stage2"
    mkdir -p "${BUILD_ROOT}/build-gcc-stage2"
    cd "${BUILD_ROOT}/build-gcc-stage2"

    echo "Configuring..."
    "${SRC}/gcc-13.2.0/configure" \
        --target=${TARGET} \
        --prefix="${PREFIX}" \
        --with-cpu=m2 \
        --with-endian=big \
        --enable-languages=c,c++ \
        --with-newlib \
        --disable-nls \
        --disable-shared \
        --disable-threads \
        --disable-libssp \
        --disable-libgomp \
        --with-system-zlib \
        2>&1 | tee "${LOG_DIR}/gcc2-configure.log"

    echo "Building... (30 minutes)"
    ${MAKE_CMD} -j${NPROC} 2>&1 | tee "${LOG_DIR}/gcc2-build.log" || {
        echo -e "${YELLOW}Build had some errors, continuing...${NC}"
    }

    echo "Installing..."
    ${MAKE_CMD} install-gcc 2>&1 | tee "${LOG_DIR}/gcc2-install.log" || {
        echo "Trying manual install..."
        cp gcc/xg++ "${PREFIX}/bin/${TARGET}-g++" 2>/dev/null && chmod +x "${PREFIX}/bin/${TARGET}-g++"
        cp gcc/cc1plus "${PREFIX}/libexec/gcc/${TARGET}/13.2.0/" 2>/dev/null && chmod +x "${PREFIX}/libexec/gcc/${TARGET}/13.2.0/cc1plus"
    }

    ${MAKE_CMD} install-target-libgcc 2>&1 | tee -a "${LOG_DIR}/gcc2-install.log" || true

    if [ -f "${PREFIX}/bin/${TARGET}-g++" ]; then
        echo -e "${GREEN}✓ GCC Stage 2 installed${NC}"
    else
        echo -e "${YELLOW}⚠ GCC Stage 2 partially installed${NC}"
    fi
    echo ""
fi

# Final verification
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

echo "Installed tools:"
for tool in as ld ar gcc g++ objcopy objdump; do
    if [ -f "${PREFIX}/bin/${TARGET}-${tool}" ]; then
        echo -e "  ${GREEN}✓${NC} ${TARGET}-${tool}"
    fi
done

echo ""
echo "Test the toolchain:"
echo "  export PATH=${PREFIX}/bin:\$PATH"
echo "  ${TARGET}-gcc --version"
echo ""

# Quick test
if [ -f "${PREFIX}/bin/${TARGET}-gcc" ]; then
    echo "Quick compile test:"
    cat > /tmp/test.c << 'EOF'
int main(void) { return 0; }
EOF

    if "${PREFIX}/bin/${TARGET}-gcc" -m2 -mb -c /tmp/test.c -o /tmp/test.o 2>&1; then
        echo -e "${GREEN}✓ Compilation works!${NC}"
        rm -f /tmp/test.*
    else
        echo -e "${YELLOW}⚠ Compilation test failed${NC}"
    fi
fi

echo ""