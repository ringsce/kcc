//
// Created by Pedro Dias Vicente on 15/01/2026.
//

// ============================================================================
// src/saturn_runtime.c - Saturn Runtime Initialization
// ============================================================================
#include "saturn.h"
#include "saturn_smpc.h"
#include "saturn_vdp1.h"
#include "saturn_vdp2.h"
#include "saturn_scu.h"
#include "saturn_scsp.h"

static volatile uint32_t tick_counter = 0;

void saturn_init(void) {
    // Initialize SMPC first
    smpc_init();

    // Turn on both CPUs
    smpc_master_on();
    smpc_slave_on();

    // Initialize SCU
    scu_init();

    // Initialize video
    vdp1_init();
    vdp2_init();

    // Initialize sound
    scsp_init();
}

void saturn_wait_vblank_in(void) {
    // Wait for VBlank-IN
    while ((VDP2_TVSTAT & 0x0008) == 0);
}

void saturn_wait_vblank_out(void) {
    // Wait for VBlank-OUT
    while ((VDP2_TVSTAT & 0x0008) != 0);
}

uint32_t saturn_get_ticks(void) {
    return tick_counter;
}

// VBlank interrupt handler
void vblank_handler(void) {
    tick_counter++;
}