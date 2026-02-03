// ============================================================================
// saturn_sdk/src/vdp2.c - VDP2 Implementation
// ============================================================================
#include "saturn_vdp2.h"
#include <string.h>

void vdp2_init(void) {
    // Set TV mode: 320x224, non-interlace, 16-bit color
    VDP2_TVMD = 0x8000;

    // Extended register enable
    VDP2_EXTEN = 0x0000;

    // VRAM control
    VDP2_RAMCTL = 0x0300;

    // Disable all backgrounds initially
    VDP2_BGON = 0x0000;

    // Clear VRAM and CRAM
    memset((void*)VDP2_VRAM, 0, 0x80000);
    memset((void*)VDP2_CRAM, 0, 0x1000);
}

void vdp2_enable_bg(uint16_t bg_mask) {
    VDP2_BGON |= bg_mask;
}

void vdp2_disable_bg(uint16_t bg_mask) {
    VDP2_BGON &= ~bg_mask;
}

void vdp2_set_scroll(int bg, int x, int y) {
    switch (bg) {
    case 0:  // NBG0
        VDP2_SCXIN0 = (x >> 16) & 0x07FF;
        VDP2_SCXDN0 = x & 0xFFFF;
        VDP2_SCYIN0 = (y >> 16) & 0x07FF;
        VDP2_SCYDN0 = y & 0xFFFF;
        break;
        // Add other backgrounds as needed
    }
}
