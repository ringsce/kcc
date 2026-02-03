// ============================================================================
// include/saturn_scsp.h - Saturn Custom Sound Processor
// ============================================================================
#ifndef SATURN_SCSP_H
#define SATURN_SCSP_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// SCSP Register Addresses
// ============================================================================

#define SCSP_BASE       0x25B00000
#define SCSP_RAM        0x25A00000
#define SCSP_RAM_SIZE   0x80000     // 512KB

// Common control registers
#define SCSP_MEM4MB     (*(volatile uint16_t*)(SCSP_BASE + 0x400))
#define SCSP_MVOL       (*(volatile uint16_t*)(SCSP_BASE + 0x402))
#define SCSP_RBL        (*(volatile uint16_t*)(SCSP_BASE + 0x408))
#define SCSP_RBP        (*(volatile uint16_t*)(SCSP_BASE + 0x40A))
#define SCSP_MSLC       (*(volatile uint16_t*)(SCSP_BASE + 0x412))
#define SCSP_CA         (*(volatile uint16_t*)(SCSP_BASE + 0x416))
#define SCSP_DMEAL      (*(volatile uint16_t*)(SCSP_BASE + 0x418))
#define SCSP_DMEAH      (*(volatile uint16_t*)(SCSP_BASE + 0x41A))
#define SCSP_DRGA       (*(volatile uint16_t*)(SCSP_BASE + 0x41C))
#define SCSP_DTLG       (*(volatile uint16_t*)(SCSP_BASE + 0x424))

// Slot registers (32 slots)
#define SCSP_SLOT_BASE(n)   (SCSP_BASE + ((n) * 0x20))

// Per-slot register offsets
#define SCSP_SLOT_KYONEX    0x00
#define SCSP_SLOT_SA        0x04
#define SCSP_SLOT_LSA       0x08
#define SCSP_SLOT_LEA       0x0C
#define SCSP_SLOT_D1R       0x10
#define SCSP_SLOT_D2R       0x11
#define SCSP_SLOT_RR        0x12
#define SCSP_SLOT_AR        0x14
#define SCSP_SLOT_DL        0x15
#define SCSP_SLOT_KRS       0x16
#define SCSP_SLOT_LPCTL     0x17
#define SCSP_SLOT_OCT       0x18
#define SCSP_SLOT_FNS       0x1A
#define SCSP_SLOT_LFORE     0x1C
#define SCSP_SLOT_LFOF      0x1D
#define SCSP_SLOT_PLFOW     0x1E
#define SCSP_SLOT_PLFOWS    0x1F
#define SCSP_SLOT_ALFOW     0x20
#define SCSP_SLOT_ALFOWS    0x21
#define SCSP_SLOT_DISDL     0x28
#define SCSP_SLOT_DIPAN     0x2A
#define SCSP_SLOT_TL        0x2C
#define SCSP_SLOT_MDL       0x2D
#define SCSP_SLOT_MDXSL     0x2E
#define SCSP_SLOT_MDYSL     0x2F

// ============================================================================
// SCSP Slot Structure
// ============================================================================

typedef struct {
    uint32_t start_addr;      // Sample start address
    uint32_t loop_start;      // Loop start address
    uint32_t loop_end;        // Loop end address
    uint16_t pitch;           // Pitch (FNS)
    uint8_t octave;           // Octave
    uint8_t total_level;      // Volume (0-255)
    uint8_t pan;              // Pan (0-31)
    uint8_t attack_rate;      // Attack rate
    uint8_t decay_rate;       // Decay rate
    uint8_t sustain_rate;     // Sustain rate
    uint8_t release_rate;     // Release rate
    uint8_t sustain_level;    // Sustain level
    bool loop_enable;         // Loop on/off
    bool key_on;              // Key on/off
} ScspSlot;

// ============================================================================
// Function Prototypes
// ============================================================================

// Initialize SCSP
void scsp_init(void);

// Sound RAM access
void scsp_load_sample(uint32_t addr, const void *data, uint32_t size);
void* scsp_get_ram_ptr(uint32_t offset);

// Slot control
void scsp_set_slot(int slot_num, const ScspSlot *slot);
void scsp_key_on(int slot_num);
void scsp_key_off(int slot_num);
void scsp_set_volume(int slot_num, uint8_t volume);
void scsp_set_pan(int slot_num, uint8_t pan);
void scsp_set_pitch(int slot_num, uint16_t pitch, uint8_t octave);

// Master controls
void scsp_set_master_volume(uint8_t volume);
void scsp_mute(bool mute);

// DSP functions (if needed)
void scsp_dsp_init(void);
void scsp_dsp_run(const uint16_t *program, int length);

// Utility functions
uint16_t scsp_calc_pitch(uint32_t sample_rate);
uint8_t scsp_calc_pan(int pan_value);  // -15 (left) to +15 (right)

#endif // SATURN_SCSP_H