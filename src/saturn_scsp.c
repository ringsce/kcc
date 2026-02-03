// ============================================================================
// src/saturn_scsp.c - Saturn SCSP Implementation
// ============================================================================
#include "saturn_scsp.h"
#include <string.h>

void scsp_init(void) {
    // Reset all slots
    for (int i = 0; i < 32; i++) {
        scsp_key_off(i);
    }

    // Set master volume to maximum
    scsp_set_master_volume(15);

    // Clear sound RAM (optional)
    memset((void*)SCSP_RAM, 0, SCSP_RAM_SIZE);
}

void scsp_load_sample(uint32_t addr, const void *data, uint32_t size) {
    if (addr + size > SCSP_RAM_SIZE) return;

    uint8_t *dest = (uint8_t*)(SCSP_RAM + addr);
    const uint8_t *src = (const uint8_t*)data;

    for (uint32_t i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

void* scsp_get_ram_ptr(uint32_t offset) {
    if (offset >= SCSP_RAM_SIZE) return NULL;
    return (void*)(SCSP_RAM + offset);
}

void scsp_set_slot(int slot_num, const ScspSlot *slot) {
    if (slot_num < 0 || slot_num >= 32 || !slot) return;

    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);

    // Set addresses
    base[SCSP_SLOT_SA / 2] = (slot->start_addr >> 16) & 0xFFFF;
    base[SCSP_SLOT_SA / 2 + 1] = slot->start_addr & 0xFFFF;
    base[SCSP_SLOT_LSA / 2] = slot->loop_start & 0xFFFF;
    base[SCSP_SLOT_LEA / 2] = slot->loop_end & 0xFFFF;

    // Set pitch and octave
    base[SCSP_SLOT_OCT / 2] = slot->octave & 0x0F;
    base[SCSP_SLOT_FNS / 2] = slot->pitch & 0x3FF;

    // Set volume and pan
    base[SCSP_SLOT_TL / 2] = slot->total_level;
    base[SCSP_SLOT_DIPAN / 2] = slot->pan & 0x1F;

    // Set envelope
    base[SCSP_SLOT_AR / 2] = slot->attack_rate & 0x1F;
    base[SCSP_SLOT_D1R / 2] = slot->decay_rate & 0x1F;
    base[SCSP_SLOT_D2R / 2] = slot->sustain_rate & 0x1F;
    base[SCSP_SLOT_RR / 2] = slot->release_rate & 0x1F;
    base[SCSP_SLOT_DL / 2] = slot->sustain_level & 0x1F;

    // Set loop control
    base[SCSP_SLOT_LPCTL / 2] = slot->loop_enable ? 1 : 0;

    // Key on if requested
    if (slot->key_on) {
        scsp_key_on(slot_num);
    }
}

void scsp_key_on(int slot_num) {
    if (slot_num < 0 || slot_num >= 32) return;
    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);
    base[SCSP_SLOT_KYONEX / 2] |= 0x1000;  // Set KYONEX bit
}

void scsp_key_off(int slot_num) {
    if (slot_num < 0 || slot_num >= 32) return;
    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);
    base[SCSP_SLOT_KYONEX / 2] &= ~0x1000;  // Clear KYONEX bit
}

void scsp_set_volume(int slot_num, uint8_t volume) {
    if (slot_num < 0 || slot_num >= 32) return;
    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);
    base[SCSP_SLOT_TL / 2] = volume;
}

void scsp_set_pan(int slot_num, uint8_t pan) {
    if (slot_num < 0 || slot_num >= 32) return;
    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);
    base[SCSP_SLOT_DIPAN / 2] = pan & 0x1F;
}

void scsp_set_pitch(int slot_num, uint16_t pitch, uint8_t octave) {
    if (slot_num < 0 || slot_num >= 32) return;
    volatile uint16_t *base = (volatile uint16_t*)SCSP_SLOT_BASE(slot_num);
    base[SCSP_SLOT_OCT / 2] = octave & 0x0F;
    base[SCSP_SLOT_FNS / 2] = pitch & 0x3FF;
}

void scsp_set_master_volume(uint8_t volume) {
    SCSP_MVOL = volume & 0x0F;
}

void scsp_mute(bool mute) {
    if (mute) {
        scsp_set_master_volume(0);
    } else {
        scsp_set_master_volume(15);
    }
}

void scsp_dsp_init(void) {
    // DSP initialization - stub for now
}

void scsp_dsp_run(const uint16_t *program, int length) {
    // DSP program execution - stub for now
    (void)program;
    (void)length;
}

uint16_t scsp_calc_pitch(uint32_t sample_rate) {
    // Calculate FNS value from sample rate
    // This is a simplified calculation
    // Actual formula: FNS = (sample_rate * 1024) / 44100
    return (uint16_t)((sample_rate * 1024) / 44100);
}

uint8_t scsp_calc_pan(int pan_value) {
    // Convert -15 to +15 range to 0-31 range
    // Center (0) = 15, Full left (-15) = 0, Full right (+15) = 31
    int result = pan_value + 15;
    if (result < 0) result = 0;
    if (result > 31) result = 31;
    return (uint8_t)result;
}