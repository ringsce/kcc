cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(KCC VERSION 1.10.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================================
# Saturn SDK Integration Options
# ========================================
option(BUILD_SATURN_TOOLS "Build Saturn SDK tools (IPMaker)" ON)
option(BUILD_GCC_SH2 "Build GCC SH2 cross-compiler from source" ON)
option(INSTALL_GCC_SH2 "Install GCC SH2 to system" ON)

# GCC SH2 installation prefix
set(GCC_SH2_PREFIX "${CMAKE_BINARY_DIR}/toolchain" CACHE PATH "GCC SH2 installation prefix")

# ========================================
# Target platform options
# ========================================
option(KCC_TARGET_SATURN "Build for Sega Saturn (SH-2)" OFF)
option(KCC_SATURN_DUAL_CPU "Enable dual SH-2 CPU support" OFF)

option(KCC_TARGET_DREAMCAST "Build for Sega Dreamcast (SH-4)" OFF)
option(KCC_DREAMCAST_VMU "Enable VMU (Visual Memory Unit) support" ON)

# Main file selection
if(KCC_TARGET_SATURN)
    set(KCC_MAIN_FILE "src/main_saturn.c" CACHE STRING "Main source file for Saturn")
elseif(KCC_TARGET_DREAMCAST)
    set(KCC_MAIN_FILE "src/main_dreamcast.c" CACHE STRING "Main source file for Dreamcast")
else()
    set(KCC_MAIN_FILE "src/main.c" CACHE STRING "Main source file")
endif()

# ========================================
# Output directories
# ========================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ========================================
# Detect architecture
# ========================================
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)")
    set(KCC_ARCH_ARM64 TRUE)
    if(WIN32)
        message(STATUS "Detected ARM64 architecture on Windows")
    else()
        message(STATUS "Detected ARM64 architecture")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)")
    set(KCC_ARCH_X86_64 TRUE)
    message(STATUS "Detected x86_64 architecture")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sh2|sh-2)")
    set(KCC_ARCH_SH2 TRUE)
    message(STATUS "Detected SH-2 architecture")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(sh4|sh-4)")
    set(KCC_ARCH_SH4 TRUE)
    message(STATUS "Detected SH-4 architecture")
endif()

# ========================================
# Sega Saturn specific configuration
# ========================================
if(KCC_TARGET_SATURN)
    set(KCC_ARCH_SH2 TRUE)
    message(STATUS "Building for Sega Saturn target")
    message(STATUS "  CPU: Hitachi SH-2 (32-bit RISC)")
    message(STATUS "  Clock: 28.63 MHz")
    message(STATUS "  Byte Order: Big Endian")
    message(STATUS "  Graphics: VDP1 (3D) + VDP2 (2D)")

    if(KCC_SATURN_DUAL_CPU)
        message(STATUS "  Dual CPU support: Enabled")
        add_definitions(-DSATURN_DUAL_CPU)
    endif()

    # Saturn-specific definitions
    add_definitions(-DTARGET_SATURN)
    add_definitions(-DTARGET_SH2)
    add_definitions(-D__SH2__)
    add_definitions(-D__BIG_ENDIAN__)

    # Note: We don't add -m2 -mb here because the compiler itself
    # runs on the host system. These flags are only for code generation.

    # Saturn memory map definitions (for code generation)
    add_definitions(-DSATURN_BOOT_ROM=0x00000000)
    add_definitions(-DSATURN_WRAM_LOW=0x00200000)
    add_definitions(-DSATURN_WRAM_HIGH=0x06000000)
    add_definitions(-DSATURN_VDP1_VRAM=0x05C00000)
    add_definitions(-DSATURN_VDP2_VRAM=0x05E00000)
endif()

# ========================================
# Sega Dreamcast specific configuration
# ========================================
if(KCC_TARGET_DREAMCAST)
    set(KCC_ARCH_SH4 TRUE)
    message(STATUS "Building for Sega Dreamcast target")
    message(STATUS "  CPU: Hitachi SH-4 (32-bit RISC)")
    message(STATUS "  Clock: 200 MHz")
    message(STATUS "  Byte Order: Little Endian")
    message(STATUS "  FPU: Hardware FPU included")
    message(STATUS "  Graphics: PowerVR2 CLX2 (3D/2D)")
    message(STATUS "  Sound: Yamaha AICA (ARM7)")

    if(KCC_DREAMCAST_VMU)
        message(STATUS "  VMU support: Enabled")
        add_definitions(-DDREAMCAST_VMU)
    endif()

    # Dreamcast-specific definitions
    add_definitions(-DTARGET_DREAMCAST)
    add_definitions(-DTARGET_SH4)
    add_definitions(-D__SH4__)
    add_definitions(-D__LITTLE_ENDIAN__)
    add_definitions(-D__HAVE_FPU__)

    # Note: We don't add -m4 -ml here because the compiler itself
    # runs on the host system. These flags are only for code generation.

    # Dreamcast memory map definitions (for code generation)
    add_definitions(-DDREAMCAST_RAM=0x8C000000)
    add_definitions(-DDREAMCAST_VRAM=0xA5000000)
    add_definitions(-DDREAMCAST_AICA_RAM=0x00800000)
    add_definitions(-DDREAMCAST_FLASH=0x00200000)
endif()

# ========================================
# Compiler warning flags per platform
# ========================================
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(CMAKE_C_COMPILER_ID MATCHES "Embarcadero")
    message(STATUS "Detected Embarcadero C++Builder")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w++")
    if(KCC_ARCH_ARM64)
        message(STATUS "Adding ARM64 flags for C++Builder")
    endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    # ARM64-specific optimizations
    if(KCC_ARCH_ARM64 AND UNIX AND NOT APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=native")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=native")
        message(STATUS "Added ARM64 native CPU optimizations")
    endif()

    # Note: SH-2 and SH-4 flags are NOT added here because we're building
    # a cross-compiler that runs on the host, not on the target platform.
    # The -m2, -mb, -m4, -ml, -mno-float flags are only used when
    # assembling the generated output with the SH toolchain.
endif()

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# ARM64-specific release optimizations for Linux
if(KCC_ARCH_ARM64 AND UNIX AND NOT APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ftree-vectorize")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftree-vectorize")
endif()

# Note: No SH-2 or SH-4 specific compiler flags needed here.
# The compiler (kcc) is built for the host system and generates
# SH-2/SH-4 assembly code that will be assembled by sh-elf-as.

# ========================================
# Platform-specific feature macros and linking
# ========================================
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    add_definitions(-D_GNU_SOURCE)
elseif(UNIX)
    add_definitions(-D_GNU_SOURCE)

    # ARM64 Linux specific definitions
    if(KCC_ARCH_ARM64)
        add_definitions(-DKCC_ARM64_LINUX)
        message(STATUS "Added ARM64 Linux specific definitions")
    endif()
endif()

# ========================================
# Include directories
# ========================================
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/macros)

# Saturn SDK include directories
if(KCC_TARGET_SATURN)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/saturn_sdk/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/saturn_sdk/lib)
endif()

# Dreamcast SDK include directories
if(KCC_TARGET_DREAMCAST)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dreamcast_sdk/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dreamcast_sdk/lib)
endif()

# ========================================
# Saturn SDK CMake modules
# ========================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SaturnHelpers.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SaturnHelpers.cmake)
    message(STATUS "Loaded Saturn SDK helper functions")
endif()

# ========================================
# Source file collections
# ========================================

# Shared source files (excluding main.c for tests)
set(SHARED_SOURCES
        src/lexer.c
        src/parser.c
        src/ast.c
        src/codegen.c
        src/error.c
        src/symbol_table.c
        src/preprocessor.c
        src/utils.c
        src/semantic.c
        src/builtins.c
)

# Saturn-specific source files (check which files exist)
set(SATURN_SOURCES)

# Check for each Saturn source file and add if it exists
set(SATURN_SOURCE_CANDIDATES
        src/sh2_codegen.c
        src/sh2_optimizer.c
        src/sh2_instruction_set.c
        src/sh2_register_allocator.c
        src/saturn_runtime.c
        src/saturn_scsp.c
        src/saturn_smpc.c
)

foreach(src_file ${SATURN_SOURCE_CANDIDATES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
        list(APPEND SATURN_SOURCES ${src_file})
    else()
        message(STATUS "Saturn source file not found: ${src_file}")
    endif()
endforeach()

# Saturn SDK sources
set(SATURN_SDK_SOURCES
        saturn_sdk/src/saturn_init.c
        saturn_sdk/src/cd_block.c
        saturn_sdk/src/vdp1.c
        saturn_sdk/src/vdp2.c
        saturn_sdk/src/smpc.c
        saturn_sdk/src/scu.c
        saturn_sdk/src/scsp.c
)

foreach(src_file ${SATURN_SDK_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
        list(APPEND SATURN_SOURCES ${src_file})
    else()
        message(STATUS "Saturn SDK source file not found: ${src_file}")
    endif()
endforeach()

# Dreamcast-specific source files (check which files exist)
set(DREAMCAST_SOURCES)

set(DREAMCAST_SOURCE_CANDIDATES
        src/sh4_codegen.c
        src/sh4_instruction_set.c
        src/sh4_register_allocator.c
        src/sh4_optimizer.c
        src/dreamcast_runtime.c
)

foreach(src_file ${DREAMCAST_SOURCE_CANDIDATES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
        list(APPEND DREAMCAST_SOURCES ${src_file})
    else()
        message(STATUS "Dreamcast source file not found: ${src_file}")
    endif()
endforeach()

# Dreamcast SDK sources
set(DREAMCAST_SDK_SOURCES
        dreamcast_sdk/src/dreamcast_init.c
        dreamcast_sdk/src/pvr.c
        dreamcast_sdk/src/aica.c
        dreamcast_sdk/src/gdrom.c
        dreamcast_sdk/src/maple.c
        dreamcast_sdk/src/vmu.c
)

foreach(src_file ${DREAMCAST_SDK_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
        list(APPEND DREAMCAST_SOURCES ${src_file})
    else()
        message(STATUS "Dreamcast SDK source file not found: ${src_file}")
    endif()
endforeach()

# ========================================
# Main executable source files
# ========================================
set(KCC_SOURCES
        ${KCC_MAIN_FILE}
        ${SHARED_SOURCES}
)

# Check if main file exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${KCC_MAIN_FILE}")
    message(WARNING "Main file not found: ${KCC_MAIN_FILE}, using default src/main.c")
    set(KCC_MAIN_FILE "src/main.c")
    list(REMOVE_ITEM KCC_SOURCES ${KCC_MAIN_FILE})
    list(INSERT KCC_SOURCES 0 "src/main.c")
endif()

message(STATUS "Using main file: ${KCC_MAIN_FILE}")

# Add Saturn sources if targeting Saturn
if(KCC_TARGET_SATURN)
    list(APPEND KCC_SOURCES ${SATURN_SOURCES})
endif()

# Add Dreamcast sources if targeting Dreamcast
if(KCC_TARGET_DREAMCAST)
    list(APPEND KCC_SOURCES ${DREAMCAST_SOURCES})
endif()

# ========================================
# Header files
# ========================================
set(KCC_HEADERS
        include/kcc.h
        include/lexer.h
        include/parser.h
        include/ast.h
        include/codegen.h
        include/error.h
        include/symbol_table.h
        include/preprocessor.h
        include/utils.h
        include/array_runtime.h
        stdlib/kcc_stdlib.h
        include/builtins.h

)

# Saturn-specific headers
set(SATURN_HEADERS
        include/sh2_registers.h
        include/sh2_codegen.h
        include/sh2_instruction_set.h
        include/sh2_optimizer.h
        include/sh2_register_allocator.h
        include/saturn.h
        include/saturn_cd.h
        include/saturn_vdp1.h
        include/saturn_vdp2.h
        include/saturn_smpc.h
        include/saturn_scu.h
        include/saturn_scsp.h
)

# Dreamcast-specific headers
set(DREAMCAST_HEADERS
        include/sh4_registers.h
        include/sh4_codegen.h
        include/sh4_instruction_set.h
        include/sh4_optimizer.h
        include/sh4_register_allocator.h
        include/dreamcast.h
        include/dreamcast_pvr.h
        include/dreamcast_aica.h
        include/dreamcast_gdrom.h
        include/dreamcast_maple.h
        include/dreamcast_vmu.h
)

# Add Saturn headers if targeting Saturn
if(KCC_TARGET_SATURN)
    list(APPEND KCC_HEADERS ${SATURN_HEADERS})
endif()

# Add Dreamcast headers if targeting Dreamcast
if(KCC_TARGET_DREAMCAST)
    list(APPEND KCC_HEADERS ${DREAMCAST_HEADERS})
endif()

# ========================================
# Saturn SDK Toolchain and Tools
# ========================================

# ========================================
# Saturn SDK Toolchain and Tools
# ========================================

# Add GCC SH2 toolchain build if requested
if(BUILD_GCC_SH2)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-sh2/CMakeLists.txt)
        add_subdirectory(toolchain/gcc-sh2)
        message(STATUS "Added GCC SH2 toolchain build")
    else()
        message(WARNING "GCC SH2 toolchain sources not found in toolchain/gcc-sh2/")
        message(WARNING "Clone from: https://github.com/SaturnSDK/Saturn-SDK-GCC-SH2.git")
    endif()
endif()


# Add GCC SH4 toolchain build if requested
if(BUILD_GCC_SH4)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-sh4/CMakeLists.txt)
        add_subdirectory(toolchain/gcc-sh4)
        message(STATUS "Added GCC SH4 toolchain build")
    else()
        message(WARNING "GCC SH4 toolchain sources not found in toolchain/gcc-sh4/")
    endif()
endif()


# Add IPMaker tool build if requested
if(BUILD_SATURN_TOOLS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tools/IPMaker/CMakeLists.txt)
        add_subdirectory(tools/IPMaker)
        message(STATUS "Added Saturn IPMaker tool")
    else()
        message(STATUS "IPMaker sources not found in tools/IPMaker/")
        message(STATUS "Clone from: https://github.com/SaturnSDK/Saturn-SDK-Tool-IPMaker.git")
    endif()
endif()

# ========================================
# Create the main KCC executable
# ========================================
add_executable(kcc ${KCC_SOURCES} ${KCC_HEADERS})

# Platform-specific linking
if(APPLE)
    target_link_libraries(kcc c)
    set_target_properties(kcc PROPERTIES
            MACOSX_DEPLOYMENT_TARGET "11.1"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(kcc c m)

    # ARM64 Linux specific linking
    if(KCC_ARCH_ARM64)
        message(STATUS "Configured linking for ARM64 Linux")
    endif()

    # Note: SH-2 and SH-4 C libraries are only needed when cross-compiling.
    # Since we're building a native compiler, we don't link against them.
    # The generated code will be assembled and linked separately with sh-elf tools.
endif()

# ========================================
# Install targets
# ========================================
install(TARGETS kcc DESTINATION bin)

# Install stdlib headers
install(FILES stdlib/kcc_stdlib.h DESTINATION include/kcc)

# Install Saturn SDK headers if targeting Saturn
if(KCC_TARGET_SATURN)
    install(FILES ${SATURN_HEADERS} DESTINATION include/kcc/saturn)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/saturn_sdk/saturn.ld)
        install(FILES saturn_sdk/saturn.ld DESTINATION lib/kcc)
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/saturn_sdk/crt0.s)
        install(FILES saturn_sdk/crt0.s DESTINATION lib/kcc)
    endif()
endif()

# Install Dreamcast SDK headers if targeting Dreamcast
if(KCC_TARGET_DREAMCAST)
    install(FILES ${DREAMCAST_HEADERS} DESTINATION include/kcc/dreamcast)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dreamcast_sdk/dreamcast.ld)
        install(FILES dreamcast_sdk/dreamcast.ld DESTINATION lib/kcc)
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dreamcast_sdk/crt0.s)
        install(FILES dreamcast_sdk/crt0.s DESTINATION lib/kcc)
    endif()
endif()

# ========================================
# Saturn SDK library target
# ========================================
if(KCC_TARGET_SATURN)
    # Check which Saturn SDK files exist
    set(SATURN_LIB_SOURCES)
    set(SATURN_LIB_CANDIDATES
            saturn_sdk/src/saturn_init.c
            saturn_sdk/src/vdp1.c
            saturn_sdk/src/vdp2.c
            saturn_sdk/src/smpc.c
            saturn_sdk/src/scu.c
            saturn_sdk/src/scsp.c
            saturn_sdk/src/cd_block.c
    )

    foreach(src_file ${SATURN_LIB_CANDIDATES})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
            list(APPEND SATURN_LIB_SOURCES ${src_file})
        endif()
    endforeach()

    if(SATURN_LIB_SOURCES)
        add_library(saturn_sdk STATIC ${SATURN_LIB_SOURCES})

        target_include_directories(saturn_sdk PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/saturn_sdk/include
        )

        set_target_properties(saturn_sdk PROPERTIES
                COMPILE_FLAGS "-nostdlib -ffreestanding"
        )

        install(TARGETS saturn_sdk ARCHIVE DESTINATION lib/kcc)
    else()
        message(WARNING "No Saturn SDK source files found - skipping library build")
    endif()
endif()

# ========================================
# Dreamcast SDK library target
# ========================================
if(KCC_TARGET_DREAMCAST)
    # Check which Dreamcast SDK files exist
    set(DREAMCAST_LIB_SOURCES
            include/sh4_codegen.h
            include/sh4_registers.h
            include/sh4_instruction_set.h
            include/sh4_register_allocator.h
            include/sh4_optimizer.h
            include/dreamcast.h
            include/dreamcast_pvr.h
            include/dreamcast_aica.h
            include/dreamcast_gdrom.h
            include/dreamcast_maple.h
            include/dreamcast_vmu.h
            dreamcast_sdk/src/dreamcast_init.c
            dreamcast_sdk/src/pvr.c
            dreamcast_sdk/src/aica.c
            dreamcast_sdk/src/gdrom.c
            dreamcast_sdk/src/maple.c
            dreamcast_sdk/src/vmu.c)
    set(DREAMCAST_LIB_CANDIDATES
            dreamcast_sdk/src/dreamcast_init.c
            dreamcast_sdk/src/pvr.c
            dreamcast_sdk/src/aica.c
            dreamcast_sdk/src/gdrom.c
            dreamcast_sdk/src/maple.c
            dreamcast_sdk/src/vmu.c
    )

    foreach(src_file ${DREAMCAST_LIB_CANDIDATES})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src_file}")
            list(APPEND DREAMCAST_LIB_SOURCES ${src_file})
        endif()
    endforeach()

    if(DREAMCAST_LIB_SOURCES)
        add_library(dreamcast_sdk STATIC ${DREAMCAST_LIB_SOURCES})

        target_include_directories(dreamcast_sdk PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/dreamcast_sdk/include
        )

        set_target_properties(dreamcast_sdk PROPERTIES
                COMPILE_FLAGS "-nostdlib -ffreestanding"
        )

        install(TARGETS dreamcast_sdk ARCHIVE DESTINATION lib/kcc)
    else()
        message(WARNING "No Dreamcast SDK source files found - skipping library build")
    endif()
endif()

# ========================================
# Testing
# ========================================
enable_testing()

# Test files
set(TEST_SOURCES
        tests/test_lexer.c
        tests/test_parser.c
        tests/test_main.c
)

# Saturn-specific tests
if(KCC_TARGET_SATURN)
    list(APPEND TEST_SOURCES
            tests/test_sh2_codegen.c
            tests/test_sh2_regalloc.c
            tests/test_saturn_runtime.c
    )
endif()

# Dreamcast-specific tests
if(KCC_TARGET_DREAMCAST)
    list(APPEND TEST_SOURCES
            tests/test_sh4_codegen.c
            tests/test_sh4_regalloc.c
            tests/test_dreamcast_runtime.c
    )
endif()

# Check if test files exist
set(TEST_SOURCES_EXIST TRUE)
foreach(test_file ${TEST_SOURCES})
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_file}")
        set(TEST_SOURCES_EXIST FALSE)
        message(STATUS "Test file not found: ${test_file}")
    endif()
endforeach()

# Create test executable only if all test files exist
if(TEST_SOURCES_EXIST)
    add_executable(kcc_tests ${SHARED_SOURCES} ${TEST_SOURCES})

    if(KCC_TARGET_SATURN)
        target_sources(kcc_tests PRIVATE ${SATURN_SOURCES})
    endif()

    if(KCC_TARGET_DREAMCAST)
        target_sources(kcc_tests PRIVATE ${DREAMCAST_SOURCES})
    endif()

    target_include_directories(kcc_tests PRIVATE include)

    if(APPLE)
        target_link_libraries(kcc_tests c)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(kcc_tests c m)

        if(KCC_ARCH_SH2 AND SH2_C_LIB)
            target_link_libraries(kcc_tests ${SH2_C_LIB})
        endif()

        if(KCC_ARCH_SH4 AND SH4_C_LIB)
            target_link_libraries(kcc_tests ${SH4_C_LIB})
        endif()
    endif()

    add_test(NAME KCC_Tests COMMAND kcc_tests)

    add_custom_target(test_all
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            DEPENDS kcc_tests
    )
else()
    message(STATUS "Skipping test executable creation - test files not found")
endif()

# ========================================
# Saturn ROM creation targets
# ========================================
if(KCC_TARGET_SATURN)
    find_program(OBJCOPY sh-elf-objcopy)
    find_program(MKISOFS mkisofs)

    # Check for built IPMaker or system IPMaker
    set(IPMAKER_EXECUTABLE "")
    if(TARGET IPMaker)
        set(IPMAKER_EXECUTABLE $<TARGET_FILE:IPMaker>)
        message(STATUS "Will use built IPMaker tool")
    else()
        find_program(IPMAKER_SYSTEM IPMaker)
        if(IPMAKER_SYSTEM)
            set(IPMAKER_EXECUTABLE ${IPMAKER_SYSTEM})
            message(STATUS "Found system IPMaker: ${IPMAKER_SYSTEM}")
        endif()
    endif()

    if(OBJCOPY)
        message(STATUS "Found objcopy: ${OBJCOPY}")

        add_custom_target(saturn_rom
                COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:kcc> kcc.bin
                COMMAND ${CMAKE_COMMAND} -E echo "Created Saturn binary: kcc.bin"
                DEPENDS kcc
                COMMENT "Creating Saturn ROM binary"
        )

        # Add IP.BIN generation if IPMaker is available
        if(IPMAKER_EXECUTABLE)
            add_custom_target(saturn_ipbin
                    COMMAND ${IPMAKER_EXECUTABLE} kcc.bin IP.BIN
                    COMMAND ${CMAKE_COMMAND} -E echo "Generated IP.BIN for Saturn"
                    DEPENDS saturn_rom
                    COMMENT "Generating Saturn IP.BIN"
            )
        endif()

        if(MKISOFS)
            message(STATUS "Found mkisofs: ${MKISOFS}")

            if(IPMAKER_EXECUTABLE)
                add_custom_target(saturn_iso
                        COMMAND ${CMAKE_COMMAND} -E make_directory cd
                        COMMAND ${CMAKE_COMMAND} -E copy kcc.bin cd/
                        COMMAND ${CMAKE_COMMAND} -E copy IP.BIN cd/
                        COMMAND ${MKISOFS} -sysid "SEGA SATURN" -volid "KCC"
                        -volset "KCC" -publisher "KCC PROJECT"
                        -preparer "KCC" -appid "KCC"
                        -generic-boot IP.BIN
                        -full-iso9660-filenames -o kcc.iso cd/
                        DEPENDS saturn_ipbin
                        COMMENT "Creating Saturn ISO image"
                )
            else()
                # Fallback without IP.BIN generation
                add_custom_target(saturn_iso
                        COMMAND ${CMAKE_COMMAND} -E make_directory cd
                        COMMAND ${CMAKE_COMMAND} -E copy kcc.bin cd/
                        COMMAND ${MKISOFS} -sysid "SEGA SATURN" -volid "KCC"
                        -volset "KCC" -publisher "KCC PROJECT"
                        -preparer "KCC" -appid "KCC"
                        -full-iso9660-filenames -o kcc.iso cd/
                        DEPENDS saturn_rom
                        COMMENT "Creating Saturn ISO image (without IP.BIN)"
                )
            endif()
        endif()
    endif()
endif()

# ========================================
# Dreamcast CDI creation targets
# ========================================
if(KCC_TARGET_DREAMCAST)
    find_program(OBJCOPY sh-elf-objcopy)
    find_program(MKDCDISC mkdcdisc)

    if(OBJCOPY)
        message(STATUS "Found objcopy for Dreamcast: ${OBJCOPY}")

        add_custom_target(dreamcast_bin
                COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:kcc> kcc_dc.bin
                COMMAND ${CMAKE_COMMAND} -E echo "Created Dreamcast binary: kcc_dc.bin"
                DEPENDS kcc
                COMMENT "Creating Dreamcast binary"
        )

        if(MKDCDISC)
            message(STATUS "Found mkdcdisc: ${MKDCDISC}")

            add_custom_target(dreamcast_cdi
                    COMMAND ${CMAKE_COMMAND} -E make_directory dc_disc
                    COMMAND ${CMAKE_COMMAND} -E copy kcc_dc.bin dc_disc/
                    COMMAND ${MKDCDISC} -e kcc_dc.bin -o kcc.cdi dc_disc
                    DEPENDS dreamcast_bin
                    COMMENT "Creating Dreamcast CDI image"
            )
        endif()
    endif()
endif()

# ========================================
# Build configuration summary
# ========================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "KCC C Compiler Configuration:")
message(STATUS "========================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture: ${CMAKE_SYSTEM_PROCESSOR}")

if(KCC_ARCH_ARM64)
    message(STATUS "  ARM64 optimizations: Enabled")
endif()

if(KCC_ARCH_SH2)
    message(STATUS "  SH-2 target: Enabled")
    message(STATUS "  Saturn SDK: ${KCC_TARGET_SATURN}")
endif()

if(KCC_ARCH_SH4)
    message(STATUS "  SH-4 target: Enabled")
    message(STATUS "  Dreamcast SDK: ${KCC_TARGET_DREAMCAST}")
endif()

if(APPLE OR UNIX)
    message(STATUS "  Feature macros: _GNU_SOURCE")
endif()

# Saturn SDK Tools configuration
if(BUILD_SATURN_TOOLS OR BUILD_GCC_SH2)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Saturn SDK Tools Configuration:")
    message(STATUS "========================================")

    if(BUILD_GCC_SH2)
        message(STATUS "  GCC SH2 Build: Enabled")
        message(STATUS "  Toolchain Prefix: ${GCC_SH2_PREFIX}")
        message(STATUS "  Install System-wide: ${INSTALL_GCC_SH2}")
    else()
        message(STATUS "  GCC SH2 Build: Using system toolchain")
    endif()

    if(BUILD_SATURN_TOOLS)
        message(STATUS "  IPMaker Tool: Enabled")
    else()
        message(STATUS "  IPMaker Tool: Disabled")
    endif()
endif()

# Configuration summary for Saturn
if(KCC_TARGET_SATURN)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Sega Saturn SDK Configuration:")
    message(STATUS "========================================")
    message(STATUS "  Compiler Type: CROSS-COMPILER")
    message(STATUS "  Host System: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "  Target: Sega Saturn (1994)")
    message(STATUS "  CPU: Dual Hitachi SH-2 @ 28.63 MHz")
    message(STATUS "  Architecture: 32-bit RISC")
    message(STATUS "  Byte Order: Big Endian")
    message(STATUS "  Dual CPU: ${KCC_SATURN_DUAL_CPU}")
    message(STATUS "  RAM: 2MB Work RAM")
    message(STATUS "  Graphics: VDP1 + VDP2")
    message(STATUS "  Sound: Motorola 68EC000 + SCSP")
    message(STATUS "  Media: CD-ROM")
    message(STATUS "")
    message(STATUS "Note: The compiler runs on ${CMAKE_SYSTEM_NAME}")
    message(STATUS "      and generates SH-2 assembly code.")
    message(STATUS "      Use sh-elf-as to assemble the output.")
    message(STATUS "")
    message(STATUS "Build targets:")
    message(STATUS "  kcc          - Compiler with Saturn support")
    message(STATUS "  saturn_sdk   - Saturn hardware library")
    if(OBJCOPY)
        message(STATUS "  saturn_rom   - Create ROM binary")
    endif()
    if(IPMAKER_EXECUTABLE)
        message(STATUS "  saturn_ipbin - Generate IP.BIN")
    endif()
    if(MKISOFS)
        message(STATUS "  saturn_iso   - Create bootable ISO")
    endif()
endif()

# Configuration summary for Dreamcast
if(KCC_TARGET_DREAMCAST)
    message(STATUS "")
    message(STATUS "========================================")
    message(STATUS "Sega Dreamcast SDK Configuration:")
    message(STATUS "========================================")
    message(STATUS "  Target: Sega Dreamcast (1998)")
    message(STATUS "  CPU: Hitachi SH-4 @ 200 MHz")
    message(STATUS "  Architecture: 32-bit RISC")
    message(STATUS "  Byte Order: Little Endian")
    message(STATUS "  FPU: Single-precision hardware FPU")
    message(STATUS "  RAM: 16MB Main RAM + 8MB VRAM")
    message(STATUS "  Graphics: PowerVR2 CLX2 (100 MHz)")
    message(STATUS "  Sound: Yamaha AICA (ARM7 @ 45 MHz)")
    message(STATUS "  Media: GD-ROM (1GB)")
    message(STATUS "  VMU Support: ${KCC_DREAMCAST_VMU}")
    message(STATUS "")
    message(STATUS "Build targets:")
    message(STATUS "  kcc             - Compiler with Dreamcast support")
    message(STATUS "  dreamcast_sdk   - Dreamcast hardware library")
    if(OBJCOPY)
        message(STATUS "  dreamcast_bin   - Create binary")
    endif()
    if(MKDCDISC)
        message(STATUS "  dreamcast_cdi   - Create bootable CDI")
    endif()
endif()

message(STATUS "========================================")
message(STATUS "")