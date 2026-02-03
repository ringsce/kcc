# toolchain/gcc-sh2/build-gcc-stage1.cmake
# Build script for GCC Stage 1 with macOS SAFE_CTYPE_H support

cmake_minimum_required(VERSION 3.15)

# Variables passed from parent CMake:
# GCC_VERSION, TARGET_TRIPLET, SRC_DIR, BUILD_DIR, INSTALL_PREFIX
# LOG_DIR, MAKE_CMD, NPROC, IS_MACOS

set(SOURCE_DIR "${SRC_DIR}/gcc-${GCC_VERSION}")
set(BUILD_GCC_DIR "${BUILD_DIR}/build-gcc-stage1")

# Setup environment for macOS
if(IS_MACOS)
    set(ENV{CC} "/usr/bin/clang")
    set(ENV{CXX} "/usr/bin/clang++")
    set(ENV{CFLAGS} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CXXFLAGS} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CPPFLAGS} "-DSAFE_CTYPE_H")
    set(ENV{CFLAGS_FOR_BUILD} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CXXFLAGS_FOR_BUILD} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CPPFLAGS_FOR_BUILD} "-DSAFE_CTYPE_H")
else()
    set(ENV{CC} "gcc")
    set(ENV{CXX} "g++")
    set(ENV{CFLAGS} "-O2")
    set(ENV{CXXFLAGS} "-O2")
endif()

# Add binutils to PATH
set(ENV{PATH} "${INSTALL_PREFIX}/bin:$ENV{PATH}")

# Check if already built
if(EXISTS "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-gcc")
    message(STATUS "✓ GCC Stage 1 already installed")
    return()
endif()

# Download prerequisites if needed
if(NOT EXISTS "${SOURCE_DIR}/gmp" OR NOT EXISTS "${SOURCE_DIR}/mpfr" OR NOT EXISTS "${SOURCE_DIR}/mpc")
    message(STATUS "Downloading GCC prerequisites...")
    execute_process(
            COMMAND ${SOURCE_DIR}/contrib/download_prerequisites
            WORKING_DIRECTORY ${SOURCE_DIR}
            RESULT_VARIABLE PREREQ_RESULT
            OUTPUT_FILE "${LOG_DIR}/gcc-prerequisites.log"
            ERROR_FILE "${LOG_DIR}/gcc-prerequisites.log"
    )

    if(NOT PREREQ_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to download GCC prerequisites. Check ${LOG_DIR}/gcc-prerequisites.log")
    endif()
    message(STATUS "✓ GCC prerequisites downloaded")
endif()

# Create build directory
file(REMOVE_RECURSE ${BUILD_GCC_DIR})
file(MAKE_DIRECTORY ${BUILD_GCC_DIR})

# Configure
message(STATUS "Configuring GCC Stage 1...")
message(STATUS "  Source: ${SOURCE_DIR}")
message(STATUS "  Build: ${BUILD_GCC_DIR}")
message(STATUS "  Install: ${INSTALL_PREFIX}")

execute_process(
        COMMAND ${SOURCE_DIR}/configure
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --with-cpu=m2
        --with-endian=big
        --enable-languages=c
        --without-headers
        --disable-nls
        --disable-shared
        --disable-threads
        --disable-libssp
        --disable-libgomp
        --disable-libmudflap
        --disable-libquadmath
        --disable-decimal-float
        --disable-libffi
        --disable-libstdcxx
        --with-newlib
        --with-system-zlib
        --enable-checking=release
        WORKING_DIRECTORY ${BUILD_GCC_DIR}
        RESULT_VARIABLE CONFIG_RESULT
        OUTPUT_FILE "${LOG_DIR}/gcc1-configure.log"
        ERROR_FILE "${LOG_DIR}/gcc1-configure.log"
)

if(NOT CONFIG_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 1 configuration failed. Check ${LOG_DIR}/gcc1-configure.log")
endif()

# Build
message(STATUS "Building GCC Stage 1... (30-45 minutes)")
execute_process(
        COMMAND ${MAKE_CMD} -j${NPROC} all-gcc
        CFLAGS=$ENV{CFLAGS}
        CXXFLAGS=$ENV{CXXFLAGS}
        CPPFLAGS=$ENV{CPPFLAGS}
        CFLAGS_FOR_BUILD=$ENV{CFLAGS_FOR_BUILD}
        CXXFLAGS_FOR_BUILD=$ENV{CXXFLAGS_FOR_BUILD}
        CPPFLAGS_FOR_BUILD=$ENV{CPPFLAGS_FOR_BUILD}
        WORKING_DIRECTORY ${BUILD_GCC_DIR}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_FILE "${LOG_DIR}/gcc1-build.log"
        ERROR_FILE "${LOG_DIR}/gcc1-build.log"
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 1 build failed. Check ${LOG_DIR}/gcc1-build.log")
endif()

# Install
message(STATUS "Installing GCC Stage 1...")
execute_process(
        COMMAND ${MAKE_CMD} install-gcc
        WORKING_DIRECTORY ${BUILD_GCC_DIR}
        RESULT_VARIABLE INSTALL_RESULT
        OUTPUT_FILE "${LOG_DIR}/gcc1-install.log"
        ERROR_FILE "${LOG_DIR}/gcc1-install.log"
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(WARNING "GCC Stage 1 install had errors, trying manual installation...")

    # Manual installation fallback
    file(MAKE_DIRECTORY "${INSTALL_PREFIX}/bin")
    file(MAKE_DIRECTORY "${INSTALL_PREFIX}/libexec/gcc/${TARGET_TRIPLET}/13.2.0")

    # Copy xgcc -> sh-elf-gcc
    if(EXISTS "${BUILD_GCC_DIR}/gcc/xgcc")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy
                "${BUILD_GCC_DIR}/gcc/xgcc"
                "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-gcc"
        )
        execute_process(
                COMMAND chmod +x "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-gcc"
        )
        message(STATUS "  Copied xgcc -> ${TARGET_TRIPLET}-gcc")
    endif()

    # Copy cc1
    if(EXISTS "${BUILD_GCC_DIR}/gcc/cc1")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy
                "${BUILD_GCC_DIR}/gcc/cc1"
                "${INSTALL_PREFIX}/libexec/gcc/${TARGET_TRIPLET}/13.2.0/cc1"
        )
        execute_process(
                COMMAND chmod +x "${INSTALL_PREFIX}/libexec/gcc/${TARGET_TRIPLET}/13.2.0/cc1"
        )
        message(STATUS "  Copied cc1")
    endif()

    # Copy cpp (optional)
    if(EXISTS "${BUILD_GCC_DIR}/gcc/cpp")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy
                "${BUILD_GCC_DIR}/gcc/cpp"
                "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-cpp"
        )
        execute_process(
                COMMAND chmod +x "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-cpp"
        )
        message(STATUS "  Copied cpp")
    endif()
endif()

# Verify installation
if(NOT EXISTS "${INSTALL_PREFIX}/bin/${TARGET_TRIPLET}-gcc")
    message(FATAL_ERROR "GCC Stage 1 installation failed - gcc binary not found")
endif()

message(STATUS "✓ GCC Stage 1 installed successfully")