cmake_minimum_required(VERSION 3.20)
project(KayteLang VERSION 1.0.0 LANGUAGES NONE)

# Detect macOS Silicon
if(APPLE)
    execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE MACHINE_ARCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(MACHINE_ARCH STREQUAL "arm64")
        set(IS_MACOS_SILICON TRUE)
        message(STATUS "Detected macOS Silicon (ARM64)")
    else()
        set(IS_MACOS_SILICON FALSE)
        message(STATUS "Detected macOS Intel (x86_64)")
    endif()
endif()

# Find lazbuild executable
find_program(LAZBUILD_EXECUTABLE
        NAMES lazbuild
        PATHS
        /usr/local/bin
        /usr/bin
        /opt/homebrew/bin
        /Applications/Lazarus.app/Contents/MacOS
        /Applications/Lazarus.app/Contents/Resources
        "$ENV{HOME}/Applications/Lazarus.app/Contents/MacOS"
        "$ENV{HOME}/.local/bin"
        "/Library/Application Support/Lazarus"
        DOC "Path to lazbuild executable"
)

if(NOT LAZBUILD_EXECUTABLE)
    message(FATAL_ERROR "lazbuild not found! Please install Lazarus IDE or Free Pascal Compiler suite.")
endif()

message(STATUS "Found lazbuild: ${LAZBUILD_EXECUTABLE}")

# Get lazbuild version
execute_process(
        COMMAND ${LAZBUILD_EXECUTABLE} --version
        OUTPUT_VARIABLE LAZBUILD_VERSION_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
message(STATUS "lazbuild version: ${LAZBUILD_VERSION_OUTPUT}")

# Configure build options
set(KAYTE_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../kayte-lang" CACHE PATH "Path to kayte-lang repository")
set(KAYTE_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/kayte-build" CACHE PATH "Kayte build output directory")
set(KAYTE_BUILD_MODE "Release" CACHE STRING "Build mode (Release or Debug)")
set_property(CACHE KAYTE_BUILD_MODE PROPERTY STRINGS Release Debug)

# Verify kayte-lang directory exists
if(NOT EXISTS "${KAYTE_PROJECT_DIR}")
    message(WARNING "Kayte-lang directory not found at: ${KAYTE_PROJECT_DIR}")
    message(STATUS "Please clone the repository: git clone https://github.com/ringsce/kayte-lang.git")
endif()

# Find all .lpi project files in kayte-lang
file(GLOB_RECURSE KAYTE_PROJECT_FILES
        "${KAYTE_PROJECT_DIR}/*.lpi"
)

if(NOT KAYTE_PROJECT_FILES)
    message(WARNING "No .lpi project files found in ${KAYTE_PROJECT_DIR}")
    message(STATUS "Available Lazarus project files will be discovered at build time")
endif()

# Create build directory
file(MAKE_DIRECTORY ${KAYTE_BUILD_DIR})

# Build configuration for macOS Silicon
set(LAZBUILD_FLAGS "")
if(IS_MACOS_SILICON)
    # ARM64 specific flags
    set(LAZBUILD_FLAGS
            "-Paarch64"                    # Target ARM64 architecture
            "-dCOCOA"                      # Use Cocoa widgetset for macOS
            "--cpu=aarch64"                # Specify CPU
            "--os=darwin"                  # Target macOS
    )
else()
    # Intel x86_64 flags
    set(LAZBUILD_FLAGS
            "-Px86_64"                     # Target x86_64 architecture
            "-dCOCOA"                      # Use Cocoa widgetset for macOS
            "--cpu=x86_64"                 # Specify CPU
            "--os=darwin"                  # Target macOS
    )
endif()

# Add common build flags
list(APPEND LAZBUILD_FLAGS
        "--build-mode=${KAYTE_BUILD_MODE}"
        "--build-all"
        "--recursive"
        "--no-write-project"
)

# Function to build a Lazarus project
function(build_lazarus_project PROJECT_FILE TARGET_NAME)
    get_filename_component(PROJECT_NAME ${PROJECT_FILE} NAME_WE)

    add_custom_target(${TARGET_NAME} ALL
            COMMAND ${LAZBUILD_EXECUTABLE}
            ${LAZBUILD_FLAGS}
            ${PROJECT_FILE}
            WORKING_DIRECTORY ${KAYTE_PROJECT_DIR}
            COMMENT "Building ${PROJECT_NAME} with lazbuild for macOS Silicon"
            VERBATIM
    )

    # Set output directory
    set_target_properties(${TARGET_NAME} PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}
    )
endfunction()

# Main Kayte projects to build
set(KAYTE_MAIN_PROJECTS
        "kayte.lpi"              # Main Kayte interpreter
        "kaytec.lpi"             # Kayte compiler
        "kayte_shell.lpi"        # Kayte shell
        "kayte_vm.lpi"           # Kayte VM
)

# Build all found projects
foreach(PROJECT_NAME ${KAYTE_MAIN_PROJECTS})
    set(PROJECT_PATH "${KAYTE_PROJECT_DIR}/${PROJECT_NAME}")

    if(EXISTS ${PROJECT_PATH})
        string(REPLACE ".lpi" "" TARGET_NAME ${PROJECT_NAME})
        build_lazarus_project(${PROJECT_PATH} "kayte_${TARGET_NAME}")
        message(STATUS "Added build target: kayte_${TARGET_NAME}")
    else()
        message(STATUS "Project file not found: ${PROJECT_PATH} (skipping)")
    endif()
endforeach()

# Build any additional projects found in subdirectories
foreach(PROJECT_FILE ${KAYTE_PROJECT_FILES})
    get_filename_component(PROJECT_NAME ${PROJECT_FILE} NAME_WE)

    # Skip if already added
    list(FIND KAYTE_MAIN_PROJECTS "${PROJECT_NAME}.lpi" INDEX)
    if(INDEX EQUAL -1)
        string(MAKE_C_IDENTIFIER ${PROJECT_NAME} TARGET_NAME)
        build_lazarus_project(${PROJECT_FILE} "kayte_extra_${TARGET_NAME}")
        message(STATUS "Added extra build target: kayte_extra_${TARGET_NAME}")
    endif()
endforeach()

# Custom target to clean Lazarus build artifacts
add_custom_target(clean_kayte
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${KAYTE_PROJECT_DIR}/lib"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${KAYTE_PROJECT_DIR}/backup"
        COMMAND find "${KAYTE_PROJECT_DIR}" -name "*.o" -delete
        COMMAND find "${KAYTE_PROJECT_DIR}" -name "*.ppu" -delete
        COMMAND find "${KAYTE_PROJECT_DIR}" -name "*.compiled" -delete
        WORKING_DIRECTORY ${KAYTE_PROJECT_DIR}
        COMMENT "Cleaning Kayte-lang build artifacts"
)

# Custom target to update kayte-lang from git
add_custom_target(update_kayte
        COMMAND git pull origin main
        WORKING_DIRECTORY ${KAYTE_PROJECT_DIR}
        COMMENT "Updating kayte-lang from GitHub"
)

# Installation
install(DIRECTORY ${KAYTE_PROJECT_DIR}/bin/
        DESTINATION bin
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING
        PATTERN "*"
        PATTERN "*.o" EXCLUDE
        PATTERN "*.ppu" EXCLUDE
)

# Print configuration summary
message(STATUS "")
message(STATUS "=== Kayte-Lang Build Configuration ===")
message(STATUS "  macOS Silicon: ${IS_MACOS_SILICON}")
message(STATUS "  Machine Architecture: ${MACHINE_ARCH}")
message(STATUS "  lazbuild: ${LAZBUILD_EXECUTABLE}")
message(STATUS "  Kayte Source: ${KAYTE_PROJECT_DIR}")
message(STATUS "  Build Directory: ${KAYTE_BUILD_DIR}")
message(STATUS "  Build Mode: ${KAYTE_BUILD_MODE}")
message(STATUS "  Build Flags: ${LAZBUILD_FLAGS}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  all           - Build all Kayte projects")
message(STATUS "  clean_kayte   - Clean build artifacts")
message(STATUS "  update_kayte  - Update from GitHub")
message(STATUS "======================================")
message(STATUS "")