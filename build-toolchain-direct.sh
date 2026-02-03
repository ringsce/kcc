#!/bin/bash
# build-toolchain-direct.sh - Direct shell script to build GCC SH2 toolchain
# This bypasses CMake for the toolchain build and uses traditional configure/make
# Optimized for modern macOS with proper SAFE_CTYPE_H handling

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
GCC_VERSION="13.2.0"
BINUTILS_VERSION="2.41"
NEWLIB_VERSION="4.3.0"
TARGET="sh-elf"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}GCC SH2 Toolchain Build Script${NC}"
echo -e "${BLUE}For macOS (with SAFE_CTYPE_H fix)${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Detect system
if [[ "$(uname -s)" == "Darwin" ]]; then
    echo -e "${GREEN}Detected macOS: $(sw_vers -productVersion)${NC}"
    IS_MACOS=1

    # Detect make command
    if command -v gmake &> /dev/null; then
        MAKE_CMD="gmake"
        echo -e "${GREEN}✓ Using GNU Make${NC}"
    else
        MAKE_CMD="make"
        echo -e "${YELLOW}⚠ Using BSD Make (GNU Make recommended: brew install make)${NC}"
    fi

    NPROC=$(sysctl -n hw.ncpu)
    echo -e "${GREEN}✓ CPU Cores: ${NPROC}${NC}"
else
    IS_MACOS=0
    MAKE_CMD="make"
    NPROC=$(nproc 2>/dev/null || echo 4)
fi

# Directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_ROOT="${SCRIPT_DIR}/build/toolchain-build"
PREFIX="${SCRIPT_DIR}/build/toolchain"
DOWNLOADS="${BUILD_ROOT}/downloads"
SRC="${BUILD_ROOT}/src"
LOG_DIR="${BUILD_ROOT}/logs"

echo ""
echo "Configuration:"
echo "  GCC: ${GCC_VERSION}"
echo "  Binutils: ${BINUTILS_VERSION}"
echo "  Newlib: ${NEWLIB_VERSION}"
echo "  Target: ${TARGET}"
echo "  Jobs: ${NPROC}"
echo "  Build: ${BUILD_ROOT}"
echo "  Install: ${PREFIX}"
echo "  Logs: ${LOG_DIR}"
echo ""

# Create directories
mkdir -p "${DOWNLOADS}"
mkdir -p "${SRC}"
mkdir -p "${PREFIX}"
mkdir -p "${LOG_DIR}"

# macOS-specific compiler setup
if [ $IS_MACOS -eq 1 ]; then
    # Use system clang
    export CC="/usr/bin/clang"
    export CXX="/usr/bin/clang++"

    # CRITICAL: Flags to fix SAFE_CTYPE_H collision with Apple's libc++
    # The key is -DSAFE_CTYPE_H which prevents safe-ctype.h from redefining ctype macros
    export CFLAGS="-O2 -DSAFE_CTYPE_H"
    export CXXFLAGS="-O2 -DSAFE_CTYPE_H"
    export CPPFLAGS="-DSAFE_CTYPE_H"

    # These ensure the flags are used for all stages
    export CFLAGS_FOR_BUILD="-O2 -DSAFE_CTYPE_H"
    export CXXFLAGS_FOR_BUILD="-O2 -DSAFE_CTYPE_H"
    export CPPFLAGS_FOR_BUILD="-DSAFE_CTYPE_H"

    echo -e "${BLUE}macOS Build Flags:${NC}"
    echo "  CC: $CC"
    echo "  CXX: $CXX"
    echo "  CFLAGS: $CFLAGS"
    echo "  CXXFLAGS: $CXXFLAGS"
    echo ""
else
    export CC="gcc"
    export CXX="g++"
    export CFLAGS="-O2"
    export CXXFLAGS="-O2"
fi

# Download function
download_file() {
    local url=$1
    local output=$2

    if [ -f "${output}" ]; then
        echo -e "${GREEN}✓${NC} Already downloaded: $(basename ${output})"
        return 0
    fi

    echo -e "${YELLOW}Downloading: $(basename ${output})${NC}"
    if command -v wget &> /dev/null; then
        wget -c "${url}" -O "${output}"
    elif command -v curl &> /dev/null; then
        curl -L "${url}" -o "${output}"
    else
        echo -e "${RED}Error: Neither wget nor curl found${NC}"
        exit 1
    fi
}

# Extract function
extract_file() {
    local archive=$1
    local dest=$2

    echo "Extracting: $(basename ${archive})"
    case "${archive}" in
        *.tar.xz)
            tar -xJf "${archive}" -C "${dest}"
            ;;
        *.tar.gz)
            tar -xzf "${archive}" -C "${dest}"
            ;;
        *.tar.bz2)
            tar -xjf "${archive}" -C "${dest}"
            ;;
        *)
            echo -e "${RED}Unknown archive format${NC}"
            exit 1
            ;;
    esac
}

# ========================================
# Step 1: Download sources
# ========================================
echo -e "${BLUE}Step 1: Downloading sources...${NC}"

download_file \
    "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz" \
    "${DOWNLOADS}/binutils-${BINUTILS_VERSION}.tar.xz"

download_file \
    "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz" \
    "${DOWNLOADS}/gcc-${GCC_VERSION}.tar.xz"

# Try multiple mirrors for Newlib
echo -e "${YELLOW}Downloading Newlib (trying multiple sources)...${NC}"
NEWLIB_DOWNLOADED=0
NEWLIB_URLS=(
    "https://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz"
    "https://mirrors.kernel.org/sourceware/newlib/newlib-${NEWLIB_VERSION}.tar.gz"
    "ftp://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz"
)

for url in "${NEWLIB_URLS[@]}"; do
    if download_file "${url}" "${DOWNLOADS}/newlib-${NEWLIB_VERSION}.tar.gz"; then
        NEWLIB_DOWNLOADED=1
        break
    fi
done

if [ $NEWLIB_DOWNLOADED -eq 0 ]; then
    echo -e "${RED}Failed to download Newlib from any mirror${NC}"
    echo ""
    echo "Please manually download newlib-${NEWLIB_VERSION}.tar.gz from:"
    echo "  https://sourceware.org/newlib/"
    echo ""
    echo "And place it in:"
    echo "  ${DOWNLOADS}/newlib-${NEWLIB_VERSION}.tar.gz"
    exit 1
fi

echo ""

# ========================================
# Step 2: Extract sources
# ========================================
echo -e "${BLUE}Step 2: Extracting sources...${NC}"

if [ ! -d "${SRC}/binutils-${BINUTILS_VERSION}" ]; then
    extract_file "${DOWNLOADS}/binutils-${BINUTILS_VERSION}.tar.xz" "${SRC}"
fi

if [ ! -d "${SRC}/gcc-${GCC_VERSION}" ]; then
    extract_file "${DOWNLOADS}/gcc-${GCC_VERSION}.tar.xz" "${SRC}"
fi

if [ ! -d "${SRC}/newlib-${NEWLIB_VERSION}" ]; then
    extract_file "${DOWNLOADS}/newlib-${NEWLIB_VERSION}.tar.gz" "${SRC}"
fi

echo ""

# ========================================
# Step 3: Build Binutils
# ========================================
echo -e "${BLUE}Step 3: Building Binutils ${BINUTILS_VERSION}...${NC}"

BINUTILS_BUILD="${BUILD_ROOT}/build-binutils"
if [ ! -f "${PREFIX}/bin/${TARGET}-as" ]; then
    mkdir -p "${BINUTILS_BUILD}"
    cd "${BINUTILS_BUILD}"

    echo "Configuring Binutils..."
    "${SRC}/binutils-${BINUTILS_VERSION}/configure" \
        --target=${TARGET} \
        --prefix="${PREFIX}" \
        --disable-nls \
        --disable-werror \
        --with-cpu=m2 \
        2>&1 | tee configure.log

    echo "Building Binutils..."
    make -j${NPROC} 2>&1 | tee build.log

    echo "Installing Binutils..."
    make install 2>&1 | tee install.log

    echo -e "${GREEN}✓ Binutils installed${NC}"
else
    echo -e "${GREEN}✓ Binutils already installed${NC}"
fi

# Add binutils to PATH
export PATH="${PREFIX}/bin:${PATH}"

echo ""

# ========================================
# Step 4: Download GCC prerequisites
# ========================================
echo -e "${BLUE}Step 4: Downloading GCC prerequisites...${NC}"

cd "${SRC}/gcc-${GCC_VERSION}"
if [ ! -d "gmp" ] || [ ! -d "mpfr" ] || [ ! -d "mpc" ]; then
    ./contrib/download_prerequisites \
        > "${LOG_DIR}/gcc-prerequisites.log" 2>&1
fi

echo ""

# ========================================
# Step 5: Build GCC (Stage 1)
# ========================================
echo -e "${BLUE}Step 5: Building GCC ${GCC_VERSION} (Stage 1)...${NC}"

GCC_BUILD_1="${BUILD_ROOT}/build-gcc-stage1"
if [ ! -f "${PREFIX}/bin/${TARGET}-gcc" ]; then
    mkdir -p "${GCC_BUILD_1}"
    cd "${GCC_BUILD_1}"

    echo "Configuring GCC Stage 1..."
    "${SRC}/gcc-${GCC_VERSION}/configure" \
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
        --disable-libmudflap \
        --disable-libquadmath \
        --disable-decimal-float \
        --disable-libffi \
        --disable-libstdcxx \
        --with-newlib \
        2>&1 | tee configure.log

    echo "Building GCC Stage 1..."
    make -j${NPROC} all-gcc 2>&1 | tee build.log

    echo "Installing GCC Stage 1..."
    make install-gcc 2>&1 | tee install.log

    echo -e "${GREEN}✓ GCC Stage 1 installed${NC}"
else
    echo -e "${GREEN}✓ GCC Stage 1 already installed${NC}"
fi

echo ""

# ========================================
# Step 6: Build Newlib
# ========================================
echo -e "${BLUE}Step 6: Building Newlib ${NEWLIB_VERSION}...${NC}"

NEWLIB_BUILD="${BUILD_ROOT}/build-newlib"
if [ ! -f "${PREFIX}/${TARGET}/lib/libc.a" ]; then
    mkdir -p "${NEWLIB_BUILD}"
    cd "${NEWLIB_BUILD}"

    echo "Configuring Newlib..."
    "${SRC}/newlib-${NEWLIB_VERSION}/configure" \
        --target=${TARGET} \
        --prefix="${PREFIX}" \
        --enable-newlib-hw-fp \
        --disable-newlib-supplied-syscalls \
        --disable-nls \
        2>&1 | tee configure.log

    echo "Building Newlib..."
    make -j${NPROC} 2>&1 | tee build.log

    echo "Installing Newlib..."
    make install 2>&1 | tee install.log

    echo -e "${GREEN}✓ Newlib installed${NC}"
else
    echo -e "${GREEN}✓ Newlib already installed${NC}"
fi

echo ""

# ========================================
# Step 7: Build GCC (Stage 2)
# ========================================
echo -e "${BLUE}Step 7: Building GCC ${GCC_VERSION} (Stage 2)...${NC}"

GCC_BUILD_2="${BUILD_ROOT}/build-gcc-stage2"
if [ ! -f "${PREFIX}/bin/${TARGET}-g++" ]; then
    mkdir -p "${GCC_BUILD_2}"
    cd "${GCC_BUILD_2}"

    echo "Configuring GCC Stage 2..."
    "${SRC}/gcc-${GCC_VERSION}/configure" \
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
        --disable-libmudflap \
        2>&1 | tee configure.log

    echo "Building GCC Stage 2..."
    make -j${NPROC} 2>&1 | tee build.log

    echo "Installing GCC Stage 2..."
    make install 2>&1 | tee install.log

    echo -e "${GREEN}✓ GCC Stage 2 installed${NC}"
else
    echo -e "${GREEN}✓ GCC Stage 2 already installed${NC}"
fi

echo ""

# ========================================
# Complete!
# ========================================
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Toolchain Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Installation: ${PREFIX}"
echo "Logs: ${LOG_DIR}"
echo ""
echo "Add to PATH (temporary):"
echo "  export PATH=${PREFIX}/bin:\$PATH"
echo ""
if [ $IS_MACOS -eq 1 ]; then
    echo "Add to PATH (permanent - zsh):"
    echo "  echo 'export PATH=${PREFIX}/bin:\$PATH' >> ~/.zshrc"
    echo "  source ~/.zshrc"
else
    echo "Add to PATH (permanent - bash):"
    echo "  echo 'export PATH=${PREFIX}/bin:\$PATH' >> ~/.bashrc"
    echo "  source ~/.bashrc"
fi
echo ""
echo "Test the toolchain:"
echo "  ${PREFIX}/bin/${TARGET}-gcc --version"
echo ""
echo "Toolchain contents:"
ls -lh "${PREFIX}/bin/${TARGET}-"* 2>/dev/null || true
echo ""

# Quick test if GCC exists
if [ -f "${PREFIX}/bin/${TARGET}-gcc" ]; then
    echo -e "${GREEN}Quick compilation test:${NC}"
    cat > /tmp/test_toolchain.c << 'EOF'
int main(void) { return 0; }
EOF

    if "${PREFIX}/bin/${TARGET}-gcc" -m2 -mb -c /tmp/test_toolchain.c -o /tmp/test_toolchain.o 2>/dev/null; then
        echo -e "${GREEN}✓ GCC can compile!${NC}"
        rm -f /tmp/test_toolchain.*
    else
        echo -e "${YELLOW}⚠ Compilation test failed${NC}"
    fi
fi

echo ""
echo -e "${YELLOW}To save disk space, you can clean build artifacts:${NC}"
echo "  rm -rf ${BUILD_ROOT}/build-*"
echo "  rm -rf ${BUILD_ROOT}/src"
echo "  (Saves ~3-4 GB, keep downloads for faster rebuilds)"
echo ""