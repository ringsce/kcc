// dreamcast_sdk/src/dreamcast_init.c
// Dreamcast Hardware Initialization

#include "dreamcast.h"
#include "dreamcast_pvr.h"
#include "dreamcast_aica.h"
#include "dreamcast_maple.h"
#include "dreamcast_gdrom.h"
#include <stdint.h>

// Hardware base addresses
#define HOLLY_BASE          0xA05F8000
#define SYSTEM_ID           (*(volatile uint32_t*)(HOLLY_BASE + 0x0000))

// CPU control
#define CCN_BASE            0xFF000000
#define CCN_MMUCR           (*(volatile uint32_t*)(CCN_BASE + 0x000))
#define CCN_CCR             (*(volatile uint32_t*)(CCN_BASE + 0x01C))

// Store Queue
#define SQ_BASE             0xE0000000

// Initialize Dreamcast hardware
void dreamcast_init(void) {
    // Disable interrupts during initialization
    uint32_t sr;
    __asm__ volatile ("stc sr, %0" : "=r"(sr));
    __asm__ volatile ("ldc %0, sr" :: "r"(sr | 0xF0));

    // Initialize CPU cache
    CCN_CCR = 0x00000809;  // Enable instruction cache, operand cache

    // Clear store queues
    for (int i = 0; i < 64; i += 4) {
        *(volatile uint32_t*)(SQ_BASE + i) = 0;
    }

    // Initialize subsystems
    dreamcast_video_init();
    dreamcast_audio_init();
    dreamcast_controller_init();

    // Re-enable interrupts
    __asm__ volatile ("ldc %0, sr" :: "r"(sr));
}

// Video subsystem initialization
void dreamcast_video_init(void) {
    pvr_init();
    pvr_set_mode(PVR_MODE_NTSC, PVR_PIXFMT_RGB565);
    pvr_set_bg_color(0.0f, 0.0f, 0.0f);
}

// Audio subsystem initialization
void dreamcast_audio_init(void) {
    aica_init();
}

// Controller subsystem initialization
void dreamcast_controller_init(void) {
    maple_init();
}

// Shutdown Dreamcast
void dreamcast_shutdown(void) {
    // Stop audio
    aica_shutdown();

    // Stop video
    pvr_shutdown();

    // Halt CPU
    while (1) {
        __asm__ volatile ("sleep");
    }
}

// Get system information
uint32_t dreamcast_get_system_id(void) {
    return SYSTEM_ID;
}

// VBlank wait
void dreamcast_vsync(void) {
    pvr_wait_ready();
}

// Performance counter (SH4 cycle counter)
static inline uint64_t dreamcast_get_cycles(void) {
    // TODO: Access SH4 performance counter
    return 0;
}

// Microsecond delay
void dreamcast_udelay(uint32_t usec) {
    // Dreamcast runs at 200MHz
    // Very rough busy-wait implementation
    volatile uint32_t count = usec * 50; // ~200 cycles per usec
    while (count--) {
        __asm__ volatile ("nop");
    }
}

// Millisecond delay
void dreamcast_mdelay(uint32_t msec) {
    for (uint32_t i = 0; i < msec; i++) {
        dreamcast_udelay(1000);
    }
}