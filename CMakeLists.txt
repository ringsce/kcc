cmake_minimum_required(VERSION 3.16)
project(KCC VERSION 1.10.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warning flags per platform
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Platform-specific feature macros and linking
if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    add_definitions(-D_GNU_SOURCE)
elseif(UNIX)
    add_definitions(-D_GNU_SOURCE)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/macros)

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
)

# Main executable source files
set(KCC_SOURCES
        src/main.c
        ${SHARED_SOURCES}

)

# Header files
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
)

# Create the main executable
add_executable(kcc ${KCC_SOURCES} ${KCC_HEADERS})

# Platform-specific linking
if(APPLE)
    target_link_libraries(kcc c)
    set_target_properties(kcc PROPERTIES
            MACOSX_DEPLOYMENT_TARGET "10.15"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(kcc c m)
endif()

# Install target
install(TARGETS kcc DESTINATION bin)

# Testing
enable_testing()

# Test files
set(TEST_SOURCES
        tests/test_lexer.c
        tests/test_parser.c
        tests/test_main.c
)

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
    add_executable(kcc_tests
            ${SHARED_SOURCES}    # All shared sources including lexer.c
            ${TEST_SOURCES}      # Test files
    )

    target_include_directories(kcc_tests PRIVATE include)

    # Link the same libraries for tests
    if(APPLE)
        target_link_libraries(kcc_tests c)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(kcc_tests c m)
    endif()

    add_test(NAME KCC_Tests COMMAND kcc_tests)

    # Custom target for running tests
    add_custom_target(test_all
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
            DEPENDS kcc_tests
    )
else()
    message(STATUS "Skipping test executable creation - test files not found")
endif()

# Print build configuration
message(STATUS "KCC C Compiler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")

if(APPLE OR UNIX)
    message(STATUS "  Feature macros: _GNU_SOURCE")
endif()