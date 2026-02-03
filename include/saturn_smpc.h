// ============================================================================
// include/saturn_smpc.h - Saturn System Management & Peripheral Control
// ============================================================================
#ifndef SATURN_SMPC_H
#define SATURN_SMPC_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// SMPC Register Addresses
// ============================================================================

#define SMPC_BASE       0x20100000

// Command registers
#define SMPC_COMREG     (*(volatile uint8_t*)(SMPC_BASE + 0x1F))
#define SMPC_SR         (*(volatile uint8_t*)(SMPC_BASE + 0x61))

// Input registers (IREG0-IREG6)
#ifndef SMPC_IREG
#define SMPC_IREG(n)    (*(volatile uint8_t*)(SMPC_BASE + 0x01 + (n) * 2))
#endif

// Output registers (OREG0-OREG31)
#define SMPC_OREG(n)    (*(volatile uint8_t*)(SMPC_BASE + 0x21 + (n) * 2))

// ============================================================================
// SMPC Commands
// ============================================================================

#define SMPC_CMD_MSHON      0x00  // Master SH2 ON
#define SMPC_CMD_SSHON      0x02  // Slave SH2 ON
#define SMPC_CMD_SSHOFF     0x03  // Slave SH2 OFF
#define SMPC_CMD_SNDON      0x06  // Sound ON
#define SMPC_CMD_SNDOFF     0x07  // Sound OFF
#define SMPC_CMD_CDON       0x08  // CD ON
#define SMPC_CMD_CDOFF      0x09  // CD OFF
#define SMPC_CMD_INTBACK    0x10  // Interrupt back (get peripheral data)
#define SMPC_CMD_SETTIME    0x16  // Set time
#define SMPC_CMD_SETSMEM    0x17  // Set SMEM
#define SMPC_CMD_NMIREQ     0x18  // NMI request
#define SMPC_CMD_RESENAB    0x19  // Reset enable
#define SMPC_CMD_RESDISA    0x1A  // Reset disable

// ============================================================================
// Status Flags
// ============================================================================

#define SMPC_SR_PDL     0x40  // Peripheral data location
#define SMPC_SR_NPE     0x20  // NP data enable
#define SMPC_SR_SF      0x01  // Status flag

// ============================================================================
// Function Prototypes
// ============================================================================

// Initialize SMPC
void smpc_init(void);

// Send command to SMPC
void smpc_command(uint8_t cmd);

// Wait for command completion
void smpc_wait_ready(void);

// CPU control
void smpc_master_on(void);
void smpc_slave_on(void);
void smpc_slave_off(void);

// Sound control
void smpc_sound_on(void);
void smpc_sound_off(void);

// CD control
void smpc_cd_on(void);
void smpc_cd_off(void);

// Peripheral input
void smpc_intback(void);
void smpc_get_peripheral_data(uint8_t *data, int length);

// RTC functions
typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day_of_week;
} SmpcDateTime;

void smpc_get_time(SmpcDateTime *dt);
void smpc_set_time(const SmpcDateTime *dt);

// System reset
void smpc_reset_enable(void);
void smpc_reset_disable(void);
void smpc_nmi_request(void);

#endif // SATURN_SMPC_H