# ============================================================================
# sh2-toolchain.cmake - CMake Toolchain File for SH-2 Cross-Compilation
# ============================================================================
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=sh2-toolchain.cmake ..

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR sh2)

# Specify the cross compiler
set(CMAKE_C_COMPILER sh-elf-gcc)
set(CMAKE_CXX_COMPILER sh-elf-g++)
set(CMAKE_ASM_COMPILER sh-elf-gcc)

# Specify the linker
set(CMAKE_LINKER sh-elf-ld)
set(CMAKE_AR sh-elf-ar)
set(CMAKE_RANLIB sh-elf-ranlib)
set(CMAKE_OBJCOPY sh-elf-objcopy)
set(CMAKE_OBJDUMP sh-elf-objdump)
set(CMAKE_SIZE sh-elf-size)

# SH-2 specific compiler flags
set(SH2_CPU_FLAGS "-m2 -mb")  # -m2 = SH-2, -mb = big endian

# Compiler flags for C
set(CMAKE_C_FLAGS_INIT "${SH2_CPU_FLAGS} -fno-builtin -nostdlib")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Compiler flags for C++
set(CMAKE_CXX_FLAGS_INIT "${SH2_CPU_FLAGS} -fno-builtin -nostdlib -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Assembler flags
set(CMAKE_ASM_FLAGS_INIT "${SH2_CPU_FLAGS}")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m2 -mb -nostartfiles -nostdlib -Wl,--gc-sections")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Skip compiler tests (cross-compilation)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)