# toolchain/gcc-sh2/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

# GCC SH2 Cross-Compiler Build Configuration
# Optimized for modern macOS with SAFE_CTYPE_H fix

set(GCC_SH2_VERSION "13.2.0" CACHE STRING "GCC version to build")
set(BINUTILS_VERSION "2.41" CACHE STRING "Binutils version to build")
set(NEWLIB_VERSION "4.3.0" CACHE STRING "Newlib version to build")

set(TARGET_TRIPLET "sh-elf")
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(INSTALL_PREFIX ${GCC_SH2_PREFIX})

# Detect system
if(APPLE)
    set(IS_MACOS TRUE)
    execute_process(
            COMMAND sw_vers -productVersion
            OUTPUT_VARIABLE MACOS_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Detected macOS: ${MACOS_VERSION}")

    # Detect GNU Make vs BSD Make
    find_program(GMAKE_EXECUTABLE gmake)
    if(GMAKE_EXECUTABLE)
        set(MAKE_CMD ${GMAKE_EXECUTABLE})
        message(STATUS "✓ Using GNU Make: ${GMAKE_EXECUTABLE}")
    else()
        set(MAKE_CMD make)
        message(STATUS "⚠ Using BSD Make (GNU Make recommended: brew install make)")
    endif()

    # Get CPU cores
    execute_process(
            COMMAND sysctl -n hw.ncpu
            OUTPUT_VARIABLE NPROC
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "✓ CPU Cores: ${NPROC}")
else()
    set(IS_MACOS FALSE)
    set(MAKE_CMD make)
    include(ProcessorCount)
    ProcessorCount(NPROC)
    if(NPROC EQUAL 0)
        set(NPROC 4)
    endif()
endif()

# Setup compiler environment for macOS
if(IS_MACOS)
    # Use system clang
    set(ENV{CC} "/usr/bin/clang")
    set(ENV{CXX} "/usr/bin/clang++")

    # CRITICAL: SAFE_CTYPE_H flag prevents macro collision with Apple's libc++
    set(ENV{CFLAGS} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CXXFLAGS} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CPPFLAGS} "-DSAFE_CTYPE_H")
    set(ENV{CFLAGS_FOR_BUILD} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CXXFLAGS_FOR_BUILD} "-O2 -DSAFE_CTYPE_H")
    set(ENV{CPPFLAGS_FOR_BUILD} "-DSAFE_CTYPE_H")

    message(STATUS "")
    message(STATUS "macOS Build Configuration:")
    message(STATUS "  CC: /usr/bin/clang")
    message(STATUS "  CXX: /usr/bin/clang++")
    message(STATUS "  CFLAGS: -O2 -DSAFE_CTYPE_H")
    message(STATUS "  CXXFLAGS: -O2 -DSAFE_CTYPE_H")
else()
    set(ENV{CC} "gcc")
    set(ENV{CXX} "g++")
    set(ENV{CFLAGS} "-O2")
    set(ENV{CXXFLAGS} "-O2")
endif()

# Create directories
set(DOWNLOADS_DIR "${BUILD_DIR}/downloads")
set(SRC_DIR "${BUILD_DIR}/src")
set(LOG_DIR "${BUILD_DIR}/logs")

file(MAKE_DIRECTORY ${DOWNLOADS_DIR})
file(MAKE_DIRECTORY ${SRC_DIR})
file(MAKE_DIRECTORY ${LOG_DIR})
file(MAKE_DIRECTORY ${INSTALL_PREFIX})

message(STATUS "")
message(STATUS "========================================")
message(STATUS "GCC SH2 Toolchain Build Configuration:")
message(STATUS "========================================")
message(STATUS "  GCC Version: ${GCC_SH2_VERSION}")
message(STATUS "  Binutils Version: ${BINUTILS_VERSION}")
message(STATUS "  Newlib Version: ${NEWLIB_VERSION}")
message(STATUS "  Target: ${TARGET_TRIPLET}")
message(STATUS "  Build Directory: ${BUILD_DIR}")
message(STATUS "  Install Prefix: ${INSTALL_PREFIX}")
message(STATUS "  Make Command: ${MAKE_CMD}")
message(STATUS "  Parallel Jobs: ${NPROC}")
message(STATUS "  Logs: ${LOG_DIR}")
message(STATUS "========================================")
message(STATUS "")

# Download URLs
set(BINUTILS_URL "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz")
set(GCC_URL "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_SH2_VERSION}/gcc-${GCC_SH2_VERSION}.tar.xz")
set(NEWLIB_URLS
        "https://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz"
        "https://mirrors.kernel.org/sourceware/newlib/newlib-${NEWLIB_VERSION}.tar.gz"
        "https://github.com/mirror/newlib-cygwin/archive/refs/tags/newlib-4.3.0.tar.gz"
)

# Function to download and extract
function(download_and_extract NAME URL ARCHIVE_PATH EXTRACT_DIR)
    if(NOT EXISTS ${ARCHIVE_PATH})
        message(STATUS "Downloading ${NAME}...")
        file(DOWNLOAD ${URL} ${ARCHIVE_PATH}
                SHOW_PROGRESS
                STATUS DOWNLOAD_STATUS
                LOG DOWNLOAD_LOG
        )

        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT STATUS_CODE EQUAL 0)
            list(GET DOWNLOAD_STATUS 1 ERROR_MSG)
            message(FATAL_ERROR "Failed to download ${NAME}: ${ERROR_MSG}")
        endif()
        message(STATUS "✓ Downloaded ${NAME}")
    else()
        message(STATUS "✓ Already have ${NAME}")
    endif()

    # Extract if not already extracted
    get_filename_component(BASENAME ${ARCHIVE_PATH} NAME_WE)
    string(REGEX REPLACE "\\.tar$" "" BASENAME ${BASENAME})

    if(NOT EXISTS "${EXTRACT_DIR}/${BASENAME}")
        message(STATUS "Extracting ${NAME}...")
        execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xf ${ARCHIVE_PATH}
                WORKING_DIRECTORY ${EXTRACT_DIR}
                RESULT_VARIABLE EXTRACT_RESULT
        )

        if(NOT EXTRACT_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${NAME}")
        endif()
        message(STATUS "✓ Extracted ${NAME}")
    endif()
endfunction()

# Download sources
download_and_extract("Binutils" ${BINUTILS_URL}
        "${DOWNLOADS_DIR}/binutils-${BINUTILS_VERSION}.tar.xz"
        ${SRC_DIR}
)

download_and_extract("GCC" ${GCC_URL}
        "${DOWNLOADS_DIR}/gcc-${GCC_SH2_VERSION}.tar.xz"
        ${SRC_DIR}
)

# Try multiple mirrors for Newlib
set(NEWLIB_DOWNLOADED FALSE)
foreach(NEWLIB_URL ${NEWLIB_URLS})
    if(NOT NEWLIB_DOWNLOADED)
        message(STATUS "Trying to download Newlib from: ${NEWLIB_URL}")
        file(DOWNLOAD ${NEWLIB_URL}
                "${DOWNLOADS_DIR}/newlib-${NEWLIB_VERSION}.tar.gz"
                STATUS DOWNLOAD_STATUS
                LOG DOWNLOAD_LOG
        )

        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(STATUS_CODE EQUAL 0)
            set(NEWLIB_DOWNLOADED TRUE)
            message(STATUS "✓ Downloaded Newlib")

            # Extract
            if(NOT EXISTS "${SRC_DIR}/newlib-${NEWLIB_VERSION}")
                execute_process(
                        COMMAND ${CMAKE_COMMAND} -E tar xf
                        "${DOWNLOADS_DIR}/newlib-${NEWLIB_VERSION}.tar.gz"
                        WORKING_DIRECTORY ${SRC_DIR}
                )
                message(STATUS "✓ Extracted Newlib")
            endif()
        endif()
    endif()
endforeach()

if(NOT NEWLIB_DOWNLOADED)
    message(FATAL_ERROR "Failed to download Newlib from any mirror")
endif()

# Custom target for building binutils
add_custom_target(binutils
        COMMAND ${CMAKE_COMMAND}
        -DBINUTILS_VERSION=${BINUTILS_VERSION}
        -DTARGET_TRIPLET=${TARGET_TRIPLET}
        -DSRC_DIR=${SRC_DIR}
        -DBUILD_DIR=${BUILD_DIR}
        -DINSTALL_PREFIX=${INSTALL_PREFIX}
        -DLOG_DIR=${LOG_DIR}
        -DMAKE_CMD=${MAKE_CMD}
        -DNPROC=${NPROC}
        -DIS_MACOS=${IS_MACOS}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/build-binutils.cmake
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Building Binutils ${BINUTILS_VERSION}..."
)

# Custom target for building GCC (stage 1)
add_custom_target(gcc-stage1
        COMMAND ${CMAKE_COMMAND}
        -DGCC_VERSION=${GCC_SH2_VERSION}
        -DTARGET_TRIPLET=${TARGET_TRIPLET}
        -DSRC_DIR=${SRC_DIR}
        -DBUILD_DIR=${BUILD_DIR}
        -DINSTALL_PREFIX=${INSTALL_PREFIX}
        -DLOG_DIR=${LOG_DIR}
        -DMAKE_CMD=${MAKE_CMD}
        -DNPROC=${NPROC}
        -DIS_MACOS=${IS_MACOS}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/build-gcc-stage1.cmake
        WORKING_DIRECTORY ${BUILD_DIR}
        DEPENDS binutils
        COMMENT "Building GCC ${GCC_SH2_VERSION} (Stage 1)..."
)

# Custom target for building Newlib
add_custom_target(newlib
        COMMAND ${CMAKE_COMMAND}
        -DNEWLIB_VERSION=${NEWLIB_VERSION}
        -DTARGET_TRIPLET=${TARGET_TRIPLET}
        -DSRC_DIR=${SRC_DIR}
        -DBUILD_DIR=${BUILD_DIR}
        -DINSTALL_PREFIX=${INSTALL_PREFIX}
        -DLOG_DIR=${LOG_DIR}
        -DMAKE_CMD=${MAKE_CMD}
        -DNPROC=${NPROC}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/build-newlib.cmake
        WORKING_DIRECTORY ${BUILD_DIR}
        DEPENDS gcc-stage1
        COMMENT "Building Newlib ${NEWLIB_VERSION}..."
)

# Custom target for building GCC (stage 2)
add_custom_target(gcc-stage2
        COMMAND ${CMAKE_COMMAND}
        -DGCC_VERSION=${GCC_SH2_VERSION}
        -DTARGET_TRIPLET=${TARGET_TRIPLET}
        -DSRC_DIR=${SRC_DIR}
        -DBUILD_DIR=${BUILD_DIR}
        -DINSTALL_PREFIX=${INSTALL_PREFIX}
        -DLOG_DIR=${LOG_DIR}
        -DMAKE_CMD=${MAKE_CMD}
        -DNPROC=${NPROC}
        -DIS_MACOS=${IS_MACOS}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/build-gcc-stage2.cmake
        WORKING_DIRECTORY ${BUILD_DIR}
        DEPENDS newlib
        COMMENT "Building GCC ${GCC_SH2_VERSION} (Stage 2)..."
)

# Main target to build entire toolchain
add_custom_target(gcc-sh2-toolchain ALL
        DEPENDS gcc-stage2
)

add_custom_command(TARGET gcc-sh2-toolchain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "GCC SH2 Toolchain Build Complete!"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "Installation: ${INSTALL_PREFIX}"
        COMMAND ${CMAKE_COMMAND} -E echo "Logs: ${LOG_DIR}"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Add to PATH:"
        COMMAND ${CMAKE_COMMAND} -E echo "  export PATH=${INSTALL_PREFIX}/bin:\$PATH"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Test the toolchain:"
        COMMAND ${CMAKE_COMMAND} -E echo "  ${INSTALL_PREFIX}/bin/sh-elf-gcc --version"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
)

# Installation
if(INSTALL_GCC_SH2)
    install(DIRECTORY ${INSTALL_PREFIX}/
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            USE_SOURCE_PERMISSIONS
    )
endif()

# Export toolchain information
set(GCC_SH2_TOOLCHAIN_FILE ${BUILD_DIR}/SaturnToolchain.cmake)
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/toolchain-template.cmake.in
        ${GCC_SH2_TOOLCHAIN_FILE}
        @ONLY
)

message(STATUS "Generated toolchain file will be: ${GCC_SH2_TOOLCHAIN_FILE}")