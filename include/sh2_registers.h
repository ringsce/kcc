// ============================================================================
// include/sh2_registers.h - SH-2 Register Definitions
// ============================================================================
#ifndef SH2_REGISTERS_H
#define SH2_REGISTERS_H

typedef enum {
    SH2_R0 = 0,   // Return value, temp
    SH2_R1,       // General purpose
    SH2_R2,       // General purpose
    SH2_R3,       // General purpose
    SH2_R4,       // First argument
    SH2_R5,       // Second argument
    SH2_R6,       // Third argument
    SH2_R7,       // Fourth argument
    SH2_R8,       // Callee-saved
    SH2_R9,       // Callee-saved
    SH2_R10,      // Callee-saved
    SH2_R11,      // Callee-saved
    SH2_R12,      // Callee-saved
    SH2_R13,      // Callee-saved
    SH2_R14,      // Frame pointer
    SH2_R15,      // Stack pointer
    SH2_PC,       // Program counter
    SH2_PR,       // Procedure register
    SH2_MACH,     // Multiply-accumulate high
    SH2_MACL,     // Multiply-accumulate low
    SH2_GBR,      // Global base register
    SH2_SR,       // Status register
    SH2_VBR       // Vector base register
} SH2Register;

#define NUM_GP_REGS 16
#define RETURN_REG SH2_R0
#define STACK_PTR SH2_R15
#define FRAME_PTR SH2_R14
#define LINK_REG SH2_PR

extern const int callee_saved_regs[];
extern const int caller_saved_regs[];
extern const int argument_regs[];

#endif // SH2_REGISTERS_H