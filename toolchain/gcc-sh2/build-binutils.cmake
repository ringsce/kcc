# toolchain/gcc-sh2/build-binutils.cmake
# Build script for Binutils

cmake_minimum_required(VERSION 3.15)

set(BINUTILS_VERSION "2.41" CACHE STRING "Binutils version")
set(TARGET_TRIPLET "sh-elf")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
set(SOURCE_DIR "${BUILD_DIR}/src/binutils-${BINUTILS_VERSION}")
set(BUILD_BINUTILS_DIR "${BUILD_DIR}/build-binutils")
set(INSTALL_PREFIX "${BUILD_DIR}/../toolchain")

# Download if needed
set(BINUTILS_URL "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz")
set(BINUTILS_ARCHIVE "${BUILD_DIR}/downloads/binutils-${BINUTILS_VERSION}.tar.xz")

if(NOT EXISTS ${SOURCE_DIR})
    message(STATUS "Downloading Binutils ${BINUTILS_VERSION}...")
    file(DOWNLOAD ${BINUTILS_URL} ${BINUTILS_ARCHIVE}
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download Binutils")
    endif()

    message(STATUS "Extracting Binutils...")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${BINUTILS_ARCHIVE}
            WORKING_DIRECTORY ${BUILD_DIR}/src
    )
endif()

# Create build directory
file(MAKE_DIRECTORY ${BUILD_BINUTILS_DIR})

# Configure with detailed output
message(STATUS "Configuring Binutils...")
message(STATUS "  Source: ${SOURCE_DIR}")
message(STATUS "  Build: ${BUILD_BINUTILS_DIR}")
message(STATUS "  Install: ${INSTALL_PREFIX}")
message(STATUS "  Target: ${TARGET_TRIPLET}")

# Set up environment for macOS
set(ENV{CFLAGS} "-Wno-error")
set(ENV{CXXFLAGS} "-Wno-error")

execute_process(
        COMMAND ${SOURCE_DIR}/configure
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --disable-nls
        --disable-werror
        --enable-lto
        --with-cpu=m2
        WORKING_DIRECTORY ${BUILD_BINUTILS_DIR}
        RESULT_VARIABLE CONFIG_RESULT
        OUTPUT_FILE ${BUILD_BINUTILS_DIR}/configure.log
        ERROR_FILE ${BUILD_BINUTILS_DIR}/configure.log
)

if(NOT CONFIG_RESULT EQUAL 0)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Binutils Configuration Failed!")
    message(STATUS "========================================")
    message(STATUS "Check the log file:")
    message(STATUS "  ${BUILD_BINUTILS_DIR}/configure.log")
    message(STATUS "")

    # Try to display last 50 lines of the log
    if(EXISTS ${BUILD_BINUTILS_DIR}/configure.log)
        execute_process(
                COMMAND tail -n 50 ${BUILD_BINUTILS_DIR}/configure.log
                OUTPUT_VARIABLE LOG_TAIL
        )
        message(STATUS "Last 50 lines of configure output:")
        message(STATUS "${LOG_TAIL}")
    endif()

    message(FATAL_ERROR "Binutils configuration failed with code: ${CONFIG_RESULT}")
endif()

# Build
message(STATUS "Building Binutils...")
include(ProcessorCount)
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
    set(NPROC 1)
endif()

execute_process(
        COMMAND make -j${NPROC}
        WORKING_DIRECTORY ${BUILD_BINUTILS_DIR}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_FILE ${BUILD_BINUTILS_DIR}/build.log
        ERROR_FILE ${BUILD_BINUTILS_DIR}/build.log
)

if(NOT BUILD_RESULT EQUAL 0)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Binutils Build Failed!")
    message(STATUS "========================================")
    message(STATUS "Check the log file:")
    message(STATUS "  ${BUILD_BINUTILS_DIR}/build.log")
    message(STATUS "")

    if(EXISTS ${BUILD_BINUTILS_DIR}/build.log)
        execute_process(
                COMMAND tail -n 50 ${BUILD_BINUTILS_DIR}/build.log
                OUTPUT_VARIABLE LOG_TAIL
        )
        message(STATUS "Last 50 lines of build output:")
        message(STATUS "${LOG_TAIL}")
    endif()

    message(FATAL_ERROR "Binutils build failed with code: ${BUILD_RESULT}")
endif()

# Install
message(STATUS "Installing Binutils...")
execute_process(
        COMMAND make install
        WORKING_DIRECTORY ${BUILD_BINUTILS_DIR}
        RESULT_VARIABLE INSTALL_RESULT
        OUTPUT_FILE ${BUILD_BINUTILS_DIR}/install.log
        ERROR_FILE ${BUILD_BINUTILS_DIR}/install.log
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Binutils Installation Failed!")
    message(STATUS "========================================")
    message(STATUS "Check the log file:")
    message(STATUS "  ${BUILD_BINUTILS_DIR}/install.log")
    message(FATAL_ERROR "Binutils installation failed with code: ${INSTALL_RESULT}")
endif()

message(STATUS "Binutils build complete!")
message(STATUS "Installed to: ${INSTALL_PREFIX}")