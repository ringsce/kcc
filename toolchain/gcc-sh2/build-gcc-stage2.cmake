# toolchain/gcc-sh2/build-gcc-stage2.cmake
# Build script for GCC Stage 2 (with C library support)

cmake_minimum_required(VERSION 3.15)

set(GCC_VERSION "13.2.0" CACHE STRING "GCC version")
set(TARGET_TRIPLET "sh-elf")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
set(SOURCE_DIR "${BUILD_DIR}/src/gcc-${GCC_VERSION}")
set(BUILD_GCC_DIR "${BUILD_DIR}/build-gcc-stage2")
set(INSTALL_PREFIX "${BUILD_DIR}/../toolchain")

# Set PATH to include previous builds
set(ENV{PATH} "${INSTALL_PREFIX}/bin:$ENV{PATH}")

# Create build directory
file(MAKE_DIRECTORY ${BUILD_GCC_DIR})

# Configure
message(STATUS "Configuring GCC Stage 2...")
execute_process(
    COMMAND ${SOURCE_DIR}/configure
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --with-cpu=m2
        --with-endian=big
        --enable-languages=c,c++
        --with-newlib
        --disable-nls
        --disable-shared
        --disable-threads
        --disable-libssp
        --disable-libgomp
        --disable-libmudflap
        --enable-lto
        --with-headers=${INSTALL_PREFIX}/${TARGET_TRIPLET}/include
    WORKING_DIRECTORY ${BUILD_GCC_DIR}
    RESULT_VARIABLE CONFIG_RESULT
)

if(NOT CONFIG_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 2 configuration failed")
endif()

# Build
message(STATUS "Building GCC Stage 2...")
include(ProcessorCount)
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
    set(NPROC 1)
endif()

execute_process(
    COMMAND make -j${NPROC}
    WORKING_DIRECTORY ${BUILD_GCC_DIR}
    RESULT_VARIABLE BUILD_RESULT
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 2 build failed")
endif()

# Install
message(STATUS "Installing GCC Stage 2...")
execute_process(
    COMMAND make install
    WORKING_DIRECTORY ${BUILD_GCC_DIR}
    RESULT_VARIABLE INSTALL_RESULT
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 2 installation failed")
endif()

message(STATUS "GCC Stage 2 build complete!")
message(STATUS "Toolchain installed to: ${INSTALL_PREFIX}")
