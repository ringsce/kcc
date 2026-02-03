# toolchain/m68k/CMakeLists.txt
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# GCC m68k Cross-Compiler Build Configuration for Sega Genesis / Mega Drive II
# Optimized for modern macOS with SAFE_CTYPE_H fix

set(GCC_M68K_VERSION "13.2.0" CACHE STRING "GCC version to build")
set(BINUTILS_VERSION "2.41" CACHE STRING "Binutils version to build")
set(NEWLIB_VERSION "4.3.0" CACHE STRING "Newlib version to build")
