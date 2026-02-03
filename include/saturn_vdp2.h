//
// Created by Pedro Dias Vicente on 15/01/2026.
//

// ============================================================================
// include/saturn_vdp2.h - VDP2 (2D Backgrounds) Definitions
// ============================================================================
#ifndef SATURN_VDP2_H
#define SATURN_VDP2_H

#include <stdint.h>

// VDP2 Registers
#define VDP2_TVMD       (*(volatile uint16_t*)(VDP2_BASE + 0x000))
#define VDP2_EXTEN      (*(volatile uint16_t*)(VDP2_BASE + 0x002))
#define VDP2_TVSTAT     (*(volatile uint16_t*)(VDP2_BASE + 0x004))
#define VDP2_VRSIZE     (*(volatile uint16_t*)(VDP2_BASE + 0x006))
#define VDP2_RAMCTL     (*(volatile uint16_t*)(VDP2_BASE + 0x00E))
#define VDP2_BGON       (*(volatile uint16_t*)(VDP2_BASE + 0x020))
#define VDP2_MZCTL      (*(volatile uint16_t*)(VDP2_BASE + 0x022))
#define VDP2_SFSEL      (*(volatile uint16_t*)(VDP2_BASE + 0x024))
#define VDP2_SFCODE     (*(volatile uint16_t*)(VDP2_BASE + 0x026))
#define VDP2_CHCTLA     (*(volatile uint16_t*)(VDP2_BASE + 0x028))
#define VDP2_CHCTLB     (*(volatile uint16_t*)(VDP2_BASE + 0x02A))

// Background Control
#define VDP2_SCXIN0     (*(volatile uint16_t*)(VDP2_BASE + 0x070))
#define VDP2_SCXDN0     (*(volatile uint16_t*)(VDP2_BASE + 0x072))
#define VDP2_SCYIN0     (*(volatile uint16_t*)(VDP2_BASE + 0x074))
#define VDP2_SCYDN0     (*(volatile uint16_t*)(VDP2_BASE + 0x076))

// Display Enable Bits
#define VDP2_DISP_NBG0  (1 << 0)
#define VDP2_DISP_NBG1  (1 << 1)
#define VDP2_DISP_NBG2  (1 << 2)
#define VDP2_DISP_NBG3  (1 << 3)
#define VDP2_DISP_RBG0  (1 << 4)

void vdp2_init(void);
void vdp2_enable_bg(uint16_t bg_mask);
void vdp2_disable_bg(uint16_t bg_mask);
void vdp2_set_scroll(int bg, int x, int y);

#endif // SATURN_VDP2_H
