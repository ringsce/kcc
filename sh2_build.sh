#!/bin/bash
# ============================================================================
# build-sh-elf-toolchain.sh - Build SH-ELF Toolchain for Sega Saturn
# ============================================================================
# This script builds a complete SH-2 cross-compilation toolchain
# for Sega Saturn development on macOS, Linux, or Windows (WSL/Cygwin)
#
# Usage: ./build-sh-elf-toolchain.sh
# ============================================================================

set -e  # Exit on error

# ============================================================================
# Configuration
# ============================================================================

TARGET=sh-elf
PREFIX="${HOME}/sh-elf"
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Versions (adjust as needed)
BINUTILS_VERSION=2.40
GCC_VERSION=13.2.0
NEWLIB_VERSION=4.3.0

# Download URLs
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"
NEWLIB_URL="https://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz"

# Build directory
BUILD_DIR="${PWD}/sh-elf-build"
SRC_DIR="${BUILD_DIR}/src"
BUILD_LOG="${BUILD_DIR}/build.log"

# ============================================================================
# Helper Functions
# ============================================================================

print_header() {
    echo ""
    echo "========================================================================"
    echo "$1"
    echo "========================================================================"
    echo ""
}

check_dependencies() {
    print_header "Checking Dependencies"
    
    local missing_deps=0
    
    # Required tools
    for tool in make gcc g++ wget tar xz; do
        if ! command -v $tool &> /dev/null; then
            echo "ERROR: $tool is not installed"
            missing_deps=1
        else
            echo "✓ $tool found"
        fi
    done
    
    if [ $missing_deps -eq 1 ]; then
        echo ""
        echo "Please install missing dependencies:"
        echo ""
        echo "macOS:   brew install wget xz"
        echo "Ubuntu:  sudo apt-get install build-essential wget xz-utils texinfo libgmp-dev libmpfr-dev libmpc-dev"
        echo "Fedora:  sudo dnf install gcc gcc-c++ make wget xz texinfo gmp-devel mpfr-devel libmpc-devel"
        exit 1
    fi
    
    echo ""
    echo "All dependencies satisfied!"
}

download_sources() {
    print_header "Downloading Source Code"
    
    mkdir -p "${SRC_DIR}"
    cd "${SRC_DIR}"
    
    # Download Binutils
    if [ ! -f "binutils-${BINUTILS_VERSION}.tar.xz" ]; then
        echo "Downloading Binutils ${BINUTILS_VERSION}..."
        wget -q --show-progress "${BINUTILS_URL}"
    else
        echo "✓ Binutils already downloaded"
    fi
    
    # Download GCC
    if [ ! -f "gcc-${GCC_VERSION}.tar.xz" ]; then
        echo "Downloading GCC ${GCC_VERSION}..."
        wget -q --show-progress "${GCC_URL}"
    else
        echo "✓ GCC already downloaded"
    fi
    
    # Download Newlib
    if [ ! -f "newlib-${NEWLIB_VERSION}.tar.gz" ]; then
        echo "Downloading Newlib ${NEWLIB_VERSION}..."
        wget -q --show-progress "${NEWLIB_URL}"
    else
        echo "✓ Newlib already downloaded"
    fi
}

extract_sources() {
    print_header "Extracting Source Code"
    
    cd "${SRC_DIR}"
    
    # Extract Binutils
    if [ ! -d "binutils-${BINUTILS_VERSION}" ]; then
        echo "Extracting Binutils..."
        tar xf "binutils-${BINUTILS_VERSION}.tar.xz"
    else
        echo "✓ Binutils already extracted"
    fi
    
    # Extract GCC
    if [ ! -d "gcc-${GCC_VERSION}" ]; then
        echo "Extracting GCC..."
        tar xf "gcc-${GCC_VERSION}.tar.xz"
    else
        echo "✓ GCC already extracted"
    fi
    
    # Extract Newlib
    if [ ! -d "newlib-${NEWLIB_VERSION}" ]; then
        echo "Extracting Newlib..."
        tar xf "newlib-${NEWLIB_VERSION}.tar.gz"
    else
        echo "✓ Newlib already extracted"
    fi
}

build_binutils() {
    print_header "Building Binutils ${BINUTILS_VERSION}"
    
    mkdir -p "${BUILD_DIR}/build-binutils"
    cd "${BUILD_DIR}/build-binutils"
    
    echo "Configuring Binutils..."
    "${SRC_DIR}/binutils-${BINUTILS_VERSION}/configure" \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --disable-werror \
        --with-cpu=m2 \
        2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Compiling Binutils (this may take a while)..."
    make -j${PARALLEL_JOBS} 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Installing Binutils..."
    make install 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "✓ Binutils installed successfully"
}

build_gcc_stage1() {
    print_header "Building GCC ${GCC_VERSION} - Stage 1 (Bootstrap)"
    
    # Add binutils to PATH
    export PATH="${PREFIX}/bin:${PATH}"
    
    # Download GCC prerequisites
    cd "${SRC_DIR}/gcc-${GCC_VERSION}"
    if [ ! -d gmp ] || [ ! -d mpfr ] || [ ! -d mpc ]; then
        echo "Downloading GCC prerequisites..."
        ./contrib/download_prerequisites
    fi
    
    mkdir -p "${BUILD_DIR}/build-gcc-stage1"
    cd "${BUILD_DIR}/build-gcc-stage1"
    
    echo "Configuring GCC Stage 1..."
    "${SRC_DIR}/gcc-${GCC_VERSION}/configure" \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --enable-languages=c \
        --without-headers \
        --with-newlib \
        --disable-shared \
        --disable-threads \
        --disable-libssp \
        --disable-libgomp \
        --disable-libmudflap \
        --with-cpu=m2 \
        --with-endian=big \
        2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Compiling GCC Stage 1 (this will take a while)..."
    make -j${PARALLEL_JOBS} all-gcc 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Installing GCC Stage 1..."
    make install-gcc 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "✓ GCC Stage 1 installed successfully"
}

build_newlib() {
    print_header "Building Newlib ${NEWLIB_VERSION}"
    
    export PATH="${PREFIX}/bin:${PATH}"
    
    mkdir -p "${BUILD_DIR}/build-newlib"
    cd "${BUILD_DIR}/build-newlib"
    
    echo "Configuring Newlib..."
    "${SRC_DIR}/newlib-${NEWLIB_VERSION}/configure" \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-newlib-supplied-syscalls \
        --enable-newlib-reent-small \
        --disable-malloc-debugging \
        --enable-newlib-multithread \
        --enable-target-optspace \
        2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Compiling Newlib (this may take a while)..."
    make -j${PARALLEL_JOBS} 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Installing Newlib..."
    make install 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "✓ Newlib installed successfully"
}

build_gcc_stage2() {
    print_header "Building GCC ${GCC_VERSION} - Stage 2 (Full)"
    
    export PATH="${PREFIX}/bin:${PATH}"
    
    mkdir -p "${BUILD_DIR}/build-gcc-stage2"
    cd "${BUILD_DIR}/build-gcc-stage2"
    
    echo "Configuring GCC Stage 2..."
    "${SRC_DIR}/gcc-${GCC_VERSION}/configure" \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --enable-languages=c,c++ \
        --with-newlib \
        --disable-shared \
        --disable-threads \
        --disable-libssp \
        --disable-libgomp \
        --disable-libmudflap \
        --with-cpu=m2 \
        --with-endian=big \
        --enable-cxx-flags="-fno-exceptions -fno-rtti" \
        2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Compiling GCC Stage 2 (this will take a while)..."
    make -j${PARALLEL_JOBS} 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "Installing GCC Stage 2..."
    make install 2>&1 | tee -a "${BUILD_LOG}"
    
    echo "✓ GCC Stage 2 installed successfully"
}

create_environment_script() {
    print_header "Creating Environment Setup Script"
    
    cat > "${PREFIX}/setup-env.sh" << EOF
#!/bin/bash
# SH-ELF Toolchain Environment Setup
# Source this file: source ${PREFIX}/setup-env.sh

export PATH="${PREFIX}/bin:\${PATH}"
export SH_ELF_PREFIX="${PREFIX}"

echo "SH-ELF Toolchain environment configured!"
echo "Toolchain location: ${PREFIX}"
echo ""
echo "Available tools:"
echo "  - sh-elf-gcc (C compiler)"
echo "  - sh-elf-g++ (C++ compiler)"
echo "  - sh-elf-ld (Linker)"
echo "  - sh-elf-ar (Archiver)"
echo "  - sh-elf-objcopy (Object copy)"
echo "  - sh-elf-objdump (Object dump)"
echo ""
echo "Test with: sh-elf-gcc --version"
EOF
    
    chmod +x "${PREFIX}/setup-env.sh"
    
    echo ""
    echo "Environment script created at: ${PREFIX}/setup-env.sh"
    echo ""
    echo "To use the toolchain, add this to your ~/.bashrc or ~/.zshrc:"
    echo "  export PATH=\"${PREFIX}/bin:\$PATH\""
}

verify_installation() {
    print_header "Verifying Installation"
    
    export PATH="${PREFIX}/bin:${PATH}"
    
    echo "Checking installed tools..."
    echo ""
    
    if command -v sh-elf-gcc &> /dev/null; then
        echo "✓ sh-elf-gcc found"
        sh-elf-gcc --version | head -n1
    else
        echo "✗ sh-elf-gcc not found"
        return 1
    fi
    
    echo ""
    
    if command -v sh-elf-g++ &> /dev/null; then
        echo "✓ sh-elf-g++ found"
        sh-elf-g++ --version | head -n1
    else
        echo "✗ sh-elf-g++ not found"
        return 1
    fi
    
    echo ""
    
    if command -v sh-elf-ld &> /dev/null; then
        echo "✓ sh-elf-ld found"
        sh-elf-ld --version | head -n1
    else
        echo "✗ sh-elf-ld not found"
        return 1
    fi
    
    echo ""
    echo "All tools verified successfully!"
}

# ============================================================================
# Main Build Process
# ============================================================================

main() {
    clear
    
    print_header "SH-ELF Toolchain Builder for Sega Saturn"
    echo "This will build and install the SH-2 cross-compilation toolchain"
    echo "Installation directory: ${PREFIX}"
    echo "Build directory: ${BUILD_DIR}"
    echo "Parallel jobs: ${PARALLEL_JOBS}"
    echo ""
    
    read -p "Continue? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted."
        exit 0
    fi
    
    # Create build directory
    mkdir -p "${BUILD_DIR}"
    
    # Start build log
    echo "Build started at $(date)" > "${BUILD_LOG}"
    
    # Build steps
    check_dependencies
    download_sources
    extract_sources
    build_binutils
    build_gcc_stage1
    build_newlib
    build_gcc_stage2
    create_environment_script
    verify_installation
    
    # Summary
    print_header "Build Complete!"
    
    echo "The SH-ELF toolchain has been successfully built and installed!"
    echo ""
    echo "Installation location: ${PREFIX}"
    echo "Build log: ${BUILD_LOG}"
    echo ""
    echo "To use the toolchain immediately:"
    echo "  export PATH=\"${PREFIX}/bin:\$PATH\""
    echo ""
    echo "Or source the environment script:"
    echo "  source ${PREFIX}/setup-env.sh"
    echo ""
    echo "To make it permanent, add to your shell profile (~/.bashrc or ~/.zshrc):"
    echo "  export PATH=\"${PREFIX}/bin:\$PATH\""
    echo ""
    echo "Test the installation:"
    echo "  sh-elf-gcc --version"
    echo ""
    echo "Happy Sega Saturn development!"
}

# Run main function
main "$@"
