# toolchain/gcc-sh2/build-newlib.cmake
# Build script for Newlib C library

cmake_minimum_required(VERSION 3.15)

set(NEWLIB_VERSION "4.3.0" CACHE STRING "Newlib version")
set(TARGET_TRIPLET "sh-elf")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build")
set(SOURCE_DIR "${BUILD_DIR}/src/newlib-${NEWLIB_VERSION}")
set(BUILD_NEWLIB_DIR "${BUILD_DIR}/build-newlib")
set(INSTALL_PREFIX "${BUILD_DIR}/../toolchain")

# Set PATH to include GCC stage 1
set(ENV{PATH} "${INSTALL_PREFIX}/bin:$ENV{PATH}")

# Download if needed
set(NEWLIB_URL "ftp://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz")
set(NEWLIB_ARCHIVE "${BUILD_DIR}/downloads/newlib-${NEWLIB_VERSION}.tar.gz")

if(NOT EXISTS ${SOURCE_DIR})
    message(STATUS "Downloading Newlib ${NEWLIB_VERSION}...")
    file(DOWNLOAD ${NEWLIB_URL} ${NEWLIB_ARCHIVE}
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download Newlib")
    endif()

    message(STATUS "Extracting Newlib...")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xf ${NEWLIB_ARCHIVE}
            WORKING_DIRECTORY ${BUILD_DIR}/src
    )
endif()

# Create build directory
file(MAKE_DIRECTORY ${BUILD_NEWLIB_DIR})

# Configure
message(STATUS "Configuring Newlib...")
execute_process(
        COMMAND ${SOURCE_DIR}/configure
        --target=${TARGET_TRIPLET}
        --prefix=${INSTALL_PREFIX}
        --enable-newlib-hw-fp
        --disable-newlib-supplied-syscalls
        --disable-nls
        WORKING_DIRECTORY ${BUILD_NEWLIB_DIR}
        RESULT_VARIABLE CONFIG_RESULT
)

if(NOT CONFIG_RESULT EQUAL 0)
    message(FATAL_ERROR "Newlib configuration failed")
endif()

# Build
message(STATUS "Building Newlib...")
include(ProcessorCount)
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
    set(NPROC 1)
endif()

execute_process(
        COMMAND make -j${NPROC}
        WORKING_DIRECTORY ${BUILD_NEWLIB_DIR}
        RESULT_VARIABLE BUILD_RESULT
)

if(NOT BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "Newlib build failed")
endif()

# Install
message(STATUS "Installing Newlib...")
execute_process(
        COMMAND make install
        WORKING_DIRECTORY ${BUILD_NEWLIB_DIR}
        RESULT_VARIABLE INSTALL_RESULT
)

if(NOT INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "Newlib installation failed")
endif()

message(STATUS "Newlib build complete!")
