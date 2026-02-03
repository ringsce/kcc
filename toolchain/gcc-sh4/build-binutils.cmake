# toolchain/gcc-sh4/build-binutils.cmake
# Build script for Binutils (SH4 target)

cmake_minimum_required(VERSION 3.15)

message(STATUS "========================================")
message(STATUS "Building Binutils ${BINUTILS_VERSION}")
message(STATUS "========================================")

set(BINUTILS_SRC "${SRC_DIR}/binutils-${BINUTILS_VERSION}")
set(BINUTILS_BUILD "${BUILD_DIR}/build-binutils")
set(BINUTILS_LOG "${LOG_DIR}/binutils.log")

# Create build directory
file(MAKE_DIRECTORY ${BINUTILS_BUILD})

# Configure Binutils
message(STATUS "Configuring Binutils...")
execute_process(
        COMMAND ${BINUTILS_SRC}/configure
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --enable-install-libbfd
        --disable-werror
        --disable-nls
        WORKING_DIRECTORY ${BINUTILS_BUILD}
        RESULT_VARIABLE CONFIGURE_RESULT
        OUTPUT_FILE ${BINUTILS_LOG}
        ERROR_FILE ${BINUTILS_LOG}
)

if(NOT CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "Binutils configure failed. Check ${BINUTILS_LOG}")
endif()

message(STATUS "✓ Binutils configured")

# Build Binutils
message(STATUS "Building Binutils (this may take a while)...")
execute_process(
        COMMAND ${MAKE_CMD} -j${NPROC}
        WORKING_DIRECTORY ${BINUTILS_BUILD}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_FILE ${BINUTILS_LOG}
        ERROR_FILE ${BINUTILS_LOG}
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "Binutils build failed. Check ${BINUTILS_LOG}")
endif()

message(STATUS "✓ Binutils built")

# Install Binutils
message(STATUS "Installing Binutils...")
execute_process(
        COMMAND ${MAKE_CMD} install
        WORKING_DIRECTORY ${BINUTILS_BUILD}
        RESULT_VARIABLE INSTALL_RESULT
        OUTPUT_FILE ${BINUTILS_LOG}
        ERROR_FILE ${BINUTILS_LOG}
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "Binutils install failed. Check ${BINUTILS_LOG}")
endif()

message(STATUS "✓ Binutils installed to ${INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "")