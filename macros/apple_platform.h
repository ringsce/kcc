/*
 * apple_platform.h - A header for detecting macOS (Apple Silicon) and iOS.
 *
 * This file uses pre-defined macros provided by Apple's Clang compiler
 * to identify the target platform at compile time. This is useful for
 * writing C code that needs to run on multiple Apple devices.
 */

// =============================================================================
//      1. Header Guard
// =============================================================================
#ifndef APPLE_PLATFORM_H
#define APPLE_PLATFORM_H

// =============================================================================
//      2. Include Apple's Target Conditionals
// =============================================================================
// This is the official Apple header that defines the TARGET_OS_* macros.
// It's the standard way to check for the target operating system.
#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// =============================================================================
//      3. Platform & Architecture Detection Macros
// =============================================================================
// We define our own set of clean, easy-to-use macros based on the official ones.

// Check for macOS
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1
    #define PLATFORM_MACOS 1
#endif

// Check for iOS or iOS Simulator
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
    #define PLATFORM_IOS 1
#endif

// Check for Apple Silicon (ARM64 architecture)
#if defined(__arm64__)
    #define PLATFORM_ARCH_ARM64 1
#endif

// A specific macro for macOS running on Apple Silicon
#if PLATFORM_MACOS && PLATFORM_ARCH_ARM64
    #define PLATFORM_MACOS_SILICON 1
#endif

// =============================================================================
//      4. API Visibility and Calling Conventions
// =============================================================================
// When building frameworks or libraries, it's good practice to control which
// functions are exported. This is a common pattern in platform headers.

#ifdef __cplusplus
    // Use C-style linkage for C++ projects to prevent name mangling.
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern
#endif

// Defines a function as being part of the public API of a library.
#define PUBLIC_API EXTERN_C __attribute__((visibility("default")))

// =============================================================================
//      5. Example of Platform-Specific Function Prototypes
// =============================================================================
// Here is how you might declare functions that have different implementations
// depending on the platform.

PUBLIC_API void initialize_platform_services();

#if PLATFORM_MACOS
// This function would only be declared and available when compiling for macOS.
PUBLIC_API const char* get_mac_serial_number_string();
#endif

#if PLATFORM_IOS
// This function would only be available when compiling for iOS.
PUBLIC_API const char* get_ios_device_model_name();
#endif


#endif // End of header guard APPLE_PLATFORM_H
