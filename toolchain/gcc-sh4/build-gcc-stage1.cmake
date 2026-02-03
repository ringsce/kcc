# toolchain/gcc-sh4/build-gcc-stage1.cmake
# Build script for GCC Stage 1 (SH4 target)

cmake_minimum_required(VERSION 3.15)

message(STATUS "========================================")
message(STATUS "Building GCC ${GCC_VERSION} (Stage 1)")
message(STATUS "========================================")

set(GCC_SRC "${SRC_DIR}/gcc-${GCC_VERSION}")
set(GCC_BUILD "${BUILD_DIR}/build-gcc-stage1")
set(GCC_LOG "${LOG_DIR}/gcc-stage1.log")

# Download GCC prerequisites
message(STATUS "Downloading GCC prerequisites...")
execute_process(
        COMMAND ${GCC_SRC}/contrib/download_prerequisites
        WORKING_DIRECTORY ${GCC_SRC}
        RESULT_VARIABLE PREREQ_RESULT
        OUTPUT_FILE ${GCC_LOG}
        ERROR_FILE ${GCC_LOG}
)

if(NOT PREREQ_RESULT EQUAL 0)
    message(WARNING "Prerequisites download may have issues. Check ${GCC_LOG}")
endif()

# Create build directory
file(MAKE_DIRECTORY ${GCC_BUILD})

# Configure GCC Stage 1
message(STATUS "Configuring GCC Stage 1...")

set(GCC_CONFIGURE_ARGS
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --enable-languages=c
        --without-headers
        --with-newlib
        --disable-libssp
        --disable-libgomp
        --disable-libmudflap
        --disable-libquadmath
        --disable-decimal-float
        --disable-threads
        --disable-shared
        --disable-nls
        --disable-werror
        --with-multilib-list=m4-single-only,m4-nofpu
)

if(IS_MACOS)
    list(APPEND GCC_CONFIGURE_ARGS
            --with-gmp=/opt/homebrew/opt/gmp
            --with-mpfr=/opt/homebrew/opt/mpfr
            --with-mpc=/opt/homebrew/opt/mpc
    )
endif()

execute_process(
        COMMAND ${GCC_SRC}/configure ${GCC_CONFIGURE_ARGS}
        WORKING_DIRECTORY ${GCC_BUILD}
        RESULT_VARIABLE CONFIGURE_RESULT
        OUTPUT_FILE ${GCC_LOG}
        ERROR_FILE ${GCC_LOG}
)

if(NOT CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 1 configure failed. Check ${GCC_LOG}")
endif()

message(STATUS "✓ GCC Stage 1 configured")

# Build GCC Stage 1
message(STATUS "Building GCC Stage 1 (this may take a while)...")
execute_process(
        COMMAND ${MAKE_CMD} -j${NPROC} all-gcc
        WORKING_DIRECTORY ${GCC_BUILD}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_FILE ${GCC_LOG}
        ERROR_FILE ${GCC_LOG}
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 1 build failed. Check ${GCC_LOG}")
endif()

message(STATUS "✓ GCC Stage 1 built")

# Install GCC Stage 1
message(STATUS "Installing GCC Stage 1...")
execute_process(
        COMMAND ${MAKE_CMD} install-gcc
        WORKING_DIRECTORY ${GCC_BUILD}
        RESULT_VARIABLE INSTALL_RESULT
        OUTPUT_FILE ${GCC_LOG}
        ERROR_FILE ${GCC_LOG}
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "GCC Stage 1 install failed. Check ${GCC_LOG}")
endif()

message(STATUS "✓ GCC Stage 1 installed to ${INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")