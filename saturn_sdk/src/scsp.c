// ============================================================================
// saturn_sdk/src/scsp.c - SCSP Implementation
// ============================================================================
#include "saturn_scsp.h"
#include <string.h>

void scsp_init(void) {
    // Basic SCSP initialization
    // Clear sound RAM
    memset((void*)SCSP_RAM, 0, SCSP_RAM_SIZE);
}

void scsp_load_sample(uint32_t addr, const void *data, uint32_t size) {
    memcpy((void*)(SCSP_RAM + addr), data, size);
}

void scsp_play_sound(int slot, uint32_t addr, uint32_t loop_addr,
                     uint32_t end_addr, uint8_t volume, uint8_t pan) {
    // Set up sound slot registers
    volatile uint32_t *slot_regs = (volatile uint32_t*)(SCSP_BASE + (slot * 0x20));

    slot_regs[0] = (addr >> 16) & 0xFFFF;
    slot_regs[1] = addr & 0xFFFF;
    slot_regs[2] = loop_addr;
    slot_regs[3] = end_addr;

    // Set volume and pan
    slot_regs[6] = (volume << 8) | pan;

    // Start playback
    slot_regs[0] |= 0x8000;
}

void scsp_stop_sound(int slot) {
    volatile uint32_t *slot_regs = (volatile uint32_t*)(SCSP_BASE + (slot * 0x20));
    slot_regs[0] &= ~0x8000;
}