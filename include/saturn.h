// ============================================================================
// include/saturn.h - Sega Saturn Hardware Definitions
// ============================================================================
#ifndef SATURN_H
#define SATURN_H

#include <stdint.h>

// Memory Map
#define BOOT_ROM        0x00000000
#define WORK_RAM_LOW    0x00200000
#define WORK_RAM_HIGH   0x06000000
#define VDP1_VRAM       0x05C00000
#define VDP1_FB         0x05C80000
#define VDP2_VRAM       0x05E00000
#define VDP2_CRAM       0x05F00000

// Hardware Registers Base Addresses
#define SMPC_BASE       0x20100000
#define SCU_BASE        0x25FE0000
#define VDP1_BASE       0x25C00000
#define VDP2_BASE       0x25F00000
#define SCSP_BASE       0x25B00000

// System Manager & Peripheral Control (SMPC)
#define SMPC_IREG(n)    (*(volatile uint8_t*)(SMPC_BASE + 0x01 + (n)))
#define SMPC_COMREG     (*(volatile uint8_t*)(SMPC_BASE + 0x1F))
#define SMPC_SR         (*(volatile uint8_t*)(SMPC_BASE + 0x61))
#define SMPC_SF         (*(volatile uint8_t*)(SMPC_BASE + 0x63))

// SMPC Commands
#define SMPC_CMD_MSHON      0x00
#define SMPC_CMD_SSHON      0x02
#define SMPC_CMD_SSHOFF     0x03
#define SMPC_CMD_SNDON      0x06
#define SMPC_CMD_SNDOFF     0x07
#define SMPC_CMD_INTBACK    0x10

// System Control Unit (SCU)
#define SCU_D0R         (*(volatile uint32_t*)(SCU_BASE + 0x000))
#define SCU_D0W         (*(volatile uint32_t*)(SCU_BASE + 0x004))
#define SCU_D0C         (*(volatile uint32_t*)(SCU_BASE + 0x008))
#define SCU_D0AD        (*(volatile uint32_t*)(SCU_BASE + 0x00C))
#define SCU_D0EN        (*(volatile uint32_t*)(SCU_BASE + 0x010))
#define SCU_D0MD        (*(volatile uint32_t*)(SCU_BASE + 0x014))

// Interrupt Control
#define SCU_IMS         (*(volatile uint32_t*)(SCU_BASE + 0x060))
#define SCU_IST         (*(volatile uint32_t*)(SCU_BASE + 0x064))

// Function Prototypes
void saturn_init(void);
void saturn_wait_vblank_in(void);
void saturn_wait_vblank_out(void);
uint32_t saturn_get_ticks(void);

#endif // SATURN_H
