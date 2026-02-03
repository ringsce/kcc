// include/sh4_registers.h
// Hitachi SH-4 Register Definitions for Sega Dreamcast

#ifndef SH4_REGISTERS_H
#define SH4_REGISTERS_H

// SH-4 General Purpose Registers (32-bit)
// Little Endian byte order on Dreamcast
typedef enum {
    SH4_REG_R0 = 0,   // Return value, scratch
    SH4_REG_R1 = 1,   // Argument 1, scratch
    SH4_REG_R2 = 2,   // Argument 2, scratch
    SH4_REG_R3 = 3,   // Argument 3, scratch
    SH4_REG_R4 = 4,   // Argument 4, callee-saved
    SH4_REG_R5 = 5,   // Argument 5, callee-saved
    SH4_REG_R6 = 6,   // Argument 6, callee-saved
    SH4_REG_R7 = 7,   // Argument 7, callee-saved
    SH4_REG_R8 = 8,   // Callee-saved
    SH4_REG_R9 = 9,   // Callee-saved
    SH4_REG_R10 = 10, // Callee-saved
    SH4_REG_R11 = 11, // Callee-saved
    SH4_REG_R12 = 12, // Callee-saved
    SH4_REG_R13 = 13, // Callee-saved
    SH4_REG_R14 = 14, // Frame pointer (FP)
    SH4_REG_R15 = 15, // Stack pointer (SP)
    SH4_REG_COUNT = 16
} SH4GeneralRegister;

// SH-4 Floating Point Registers (single-precision on Dreamcast)
// Dreamcast uses -m4-single-only mode
typedef enum {
    SH4_FREG_FR0 = 0,
    SH4_FREG_FR1 = 1,
    SH4_FREG_FR2 = 2,
    SH4_FREG_FR3 = 3,
    SH4_FREG_FR4 = 4,
    SH4_FREG_FR5 = 5,
    SH4_FREG_FR6 = 6,
    SH4_FREG_FR7 = 7,
    SH4_FREG_FR8 = 8,
    SH4_FREG_FR9 = 9,
    SH4_FREG_FR10 = 10,
    SH4_FREG_FR11 = 11,
    SH4_FREG_FR12 = 12,
    SH4_FREG_FR13 = 13,
    SH4_FREG_FR14 = 14,
    SH4_FREG_FR15 = 15,
    SH4_FREG_COUNT = 16
} SH4FloatRegister;

// SH-4 System Registers
typedef enum {
    SH4_SR,    // Status Register
    SH4_GBR,   // Global Base Register
    SH4_VBR,   // Vector Base Register
    SH4_SSR,   // Saved Status Register
    SH4_SPC,   // Saved Program Counter
    SH4_MACH,  // Multiply-Accumulate High
    SH4_MACL,  // Multiply-Accumulate Low
    SH4_PR,    // Procedure Register (return address)
    SH4_FPSCR, // Floating Point Status/Control Register
    SH4_FPUL   // Floating Point Communication Register
} SH4SystemRegister;

// Register usage conventions
#define SH4_RETURN_REG SH4_REG_R0
#define SH4_FRAME_POINTER SH4_REG_R14
#define SH4_STACK_POINTER SH4_REG_R15

// Argument registers
#define SH4_ARG1_REG SH4_REG_R4
#define SH4_ARG2_REG SH4_REG_R5
#define SH4_ARG3_REG SH4_REG_R6
#define SH4_ARG4_REG SH4_REG_R7

// Callee-saved registers (must preserve)
#define SH4_CALLEE_SAVED_START SH4_REG_R8
#define SH4_CALLEE_SAVED_END SH4_REG_R14

// Scratch registers (caller-saved)
#define SH4_SCRATCH_START SH4_REG_R0
#define SH4_SCRATCH_END SH4_REG_R3

// Register properties
typedef struct {
    int number;
    const char* name;
    int is_callee_saved;
    int is_special; // SP, FP, etc.
} SH4RegisterInfo;

// Get register information
const char* sh4_register_name(int reg);
int sh4_is_callee_saved(int reg);
int sh4_is_scratch_register(int reg);

#endif // SH4_REGISTERS_H