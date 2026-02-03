 // ============================================================================
// saturn_sdk/src/vdp1.c - VDP1 Implementation
// ============================================================================
#include "saturn_vdp1.h"
#include <string.h>

void vdp1_init(void) {
    // Set TV mode to normal, frame buffer change mode to manual
    VDP1_TVMR = 0x0000;

    // Set frame buffer mode
    VDP1_FBCR = 0x0000;

    // Plot trigger mode
    VDP1_PTMR = 0x0002;

    // Set drawing window
    VDP1_EWDR = 0x0000;
    VDP1_EWLR = (0 << 9) | 0;
    VDP1_EWRR = (223 << 9) | 319;

    // Clear VRAM
    memset((void*)VDP1_VRAM, 0, 0x80000);
}

void vdp1_start(void) {
    VDP1_PTMR |= 0x0001;
}

void vdp1_wait_draw_end(void) {
    while ((VDP1_EDSR & 0x0002) == 0);
}

VDP1Command* vdp1_get_command(int index) {
    return (VDP1Command*)(VDP1_VRAM + (index * sizeof(VDP1Command)));
}