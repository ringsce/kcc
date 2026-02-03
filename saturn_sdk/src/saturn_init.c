// ============================================================================
// saturn_sdk/src/saturn_init.c - Saturn Hardware Initialization
// ============================================================================
#include "saturn.h"
#include "saturn_smpc.h"
#include "saturn_vdp1.h"
#include "saturn_vdp2.h"
#include "saturn_scu.h"
#include "saturn_scsp.h"
#include <stdint.h>
#include <string.h>

// Global tick counter for timing
static volatile uint32_t g_tick_counter = 0;
static volatile uint32_t g_frame_counter = 0;
static volatile bool g_vblank_flag = false;

// System state
static bool g_system_initialized = false;
static bool g_slave_cpu_enabled = false;

// ============================================================================
// Target Architecture Detection
// ============================================================================

// Only enable SH-2 code when explicitly cross-compiling with sh-elf-gcc
#if defined(__SH2__) && defined(__ELF__) && !defined(__APPLE__) && !defined(__x86_64__) && !defined(__aarch64__)
    #define TARGET_SH2 1
    #define INTERRUPT_HANDLER __attribute__((interrupt_handler))
#else
    #define TARGET_SH2 0
    #define INTERRUPT_HANDLER
#endif

// ============================================================================
// Low-Level Hardware Initialization
// ============================================================================

static void saturn_init_cache(void) {
#if TARGET_SH2
    volatile uint8_t *ccr = (volatile uint8_t*)0xFFFFFE92;
    *ccr = 0x01;
#endif
}

static void saturn_init_watchdog(void) {
#if TARGET_SH2
    volatile uint16_t *wtcsr = (volatile uint16_t*)0xFFFFFE80;
    volatile uint16_t *wtcnt = (volatile uint16_t*)0xFFFFFE80;
    *wtcsr = 0xA500;
    *wtcnt = 0xA500;
#endif
}

static void saturn_init_interrupt_priorities(void) {
#if TARGET_SH2
    SCU_IMS &= ~(1 << 0);
    SCU_IMS &= ~(1 << 1);
    SCU_IMS |= (1 << 2);
    SCU_IMS &= ~(1 << 3);
    SCU_IMS &= ~(1 << 8);
    SCU_IMS &= ~(1 << 9);
    SCU_IMS &= ~(1 << 10);
#endif
}

static void saturn_init_dma(void) {
#if TARGET_SH2
    SCU_D0EN = 0;
    volatile uint32_t *dma_regs = (volatile uint32_t*)SCU_BASE;
    for (int ch = 0; ch < 3; ch++) {
        dma_regs[ch * 8 + 5] = 0;
    }
#endif
}

static void saturn_clear_work_ram(void) {
#if TARGET_SH2
    uint32_t *ram = (uint32_t*)(WORK_RAM_LOW + 0x1000);
    uint32_t count = (0x100000 - 0x1000) / 4;
    for (uint32_t i = 0; i < count; i++) {
        ram[i] = 0;
    }
    ram = (uint32_t*)WORK_RAM_HIGH;
    count = (0x100000 - 0x10000) / 4;
    for (uint32_t i = 0; i < count; i++) {
        ram[i] = 0;
    }
#endif
}

// ============================================================================
// V-Blank Interrupt Handlers
// ============================================================================

INTERRUPT_HANDLER void vblank_in_handler(void) {
    g_tick_counter++;
    g_frame_counter++;
    g_vblank_flag = true;
#if TARGET_SH2
    SCU_IST &= ~(1 << 0);
#endif
}

INTERRUPT_HANDLER void vblank_out_handler(void) {
    g_vblank_flag = false;
#if TARGET_SH2
    SCU_IST &= ~(1 << 1);
#endif
}

// ============================================================================
// Assembly Helpers for SH-2
// ============================================================================

static inline void sh2_disable_interrupts(void) {
#if TARGET_SH2
    __asm__ volatile("mov.l sr,r0\n\t"
                     "or #0xF0,r0\n\t"
                     "ldc r0,sr\n\t"
                     ::: "r0");
#endif
}

static inline void sh2_enable_interrupts(void) {
#if TARGET_SH2
    __asm__ volatile("mov.l sr,r0\n\t"
                     "and #0x0F,r0\n\t"
                     "ldc r0,sr\n\t"
                     ::: "r0");
#endif
}

static inline void sh2_set_vbr(uint32_t addr) {
#if TARGET_SH2
    register uint32_t r0 __asm__("r0") = addr;
    __asm__ volatile("ldc %0,vbr\n\t" : : "r"(r0));
#else
    (void)addr;
#endif
}

// ============================================================================
// Public Initialization Function
// ============================================================================

void saturn_init(void) {
    if (g_system_initialized) {
        return;
    }

    sh2_disable_interrupts();
    saturn_init_watchdog();
    saturn_init_cache();
    saturn_clear_work_ram();

    smpc_init();
    smpc_master_on();

    scu_init();
    saturn_init_interrupt_priorities();
    saturn_init_dma();

    vdp1_init();
    vdp2_init();
    scsp_init();

#if TARGET_SH2
    volatile uint32_t *vectors = (volatile uint32_t*)0x06000000;
    vectors[0x40/4] = (uint32_t)(uintptr_t)vblank_in_handler;
    vectors[0x44/4] = (uint32_t)(uintptr_t)vblank_out_handler;
    sh2_set_vbr(WORK_RAM_HIGH);
#endif

    sh2_enable_interrupts();
    g_system_initialized = true;
}

void saturn_init_slave(void) {
    if (g_slave_cpu_enabled) {
        return;
    }
#if TARGET_SH2
    smpc_slave_on();
    volatile uint32_t *slave_entry = (volatile uint32_t*)0x06000310;
    extern void _slave_entry(void);
    *slave_entry = (uint32_t)(uintptr_t)_slave_entry;
#endif
    g_slave_cpu_enabled = true;
}

void saturn_wait_vblank_in(void) {
    while (!g_vblank_flag) { }
}

void saturn_wait_vblank_out(void) {
    while (g_vblank_flag) { }
}

uint32_t saturn_get_ticks(void) {
    return g_tick_counter;
}

uint32_t saturn_get_frame_count(void) {
    return g_frame_counter;
}

void saturn_delay_ms(uint32_t ms) {
    uint32_t frames = (ms * 60) / 1000;
    uint32_t start = g_frame_counter;
    while ((g_frame_counter - start) < frames) {
        saturn_wait_vblank_in();
    }
}

bool saturn_is_initialized(void) {
    return g_system_initialized;
}

bool saturn_is_pal(void) {
#if TARGET_SH2
    return (VDP2_TVSTAT & 0x0001) != 0;
#else
    return false;
#endif
}

bool saturn_is_ntsc(void) {
#if TARGET_SH2
    return (VDP2_TVSTAT & 0x0000) != 0;
#else
    return false;
#endif
}
bool saturn_is_slave_enabled(void) {
    return g_slave_cpu_enabled;
}