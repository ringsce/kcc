// ============================================================================
// include/saturn_vdp1.h - VDP1 (3D Graphics) Definitions
// ============================================================================
#ifndef SATURN_VDP1_H
#define SATURN_VDP1_H

#include <stdint.h>

// VDP1 Registers
#define VDP1_TVMR       (*(volatile uint16_t*)(VDP1_BASE + 0x000))
#define VDP1_FBCR       (*(volatile uint16_t*)(VDP1_BASE + 0x002))
#define VDP1_PTMR       (*(volatile uint16_t*)(VDP1_BASE + 0x004))
#define VDP1_EWDR       (*(volatile uint16_t*)(VDP1_BASE + 0x006))
#define VDP1_EWLR       (*(volatile uint16_t*)(VDP1_BASE + 0x008))
#define VDP1_EWRR       (*(volatile uint16_t*)(VDP1_BASE + 0x00A))
#define VDP1_ENDR       (*(volatile uint16_t*)(VDP1_BASE + 0x00C))
#define VDP1_EDSR       (*(volatile uint16_t*)(VDP1_BASE + 0x010))
#define VDP1_LOPR       (*(volatile uint16_t*)(VDP1_BASE + 0x012))
#define VDP1_COPR       (*(volatile uint16_t*)(VDP1_BASE + 0x014))
#define VDP1_MODR       (*(volatile uint16_t*)(VDP1_BASE + 0x016))

// VDP1 Command Types
typedef enum {
    VDP1_NORMAL_SPRITE = 0,
    VDP1_SCALED_SPRITE = 1,
    VDP1_DISTORTED_SPRITE = 2,
    VDP1_POLYGON = 4,
    VDP1_POLYLINE = 5,
    VDP1_LINE = 6,
    VDP1_USER_CLIP = 8,
    VDP1_SYSTEM_CLIP = 9,
    VDP1_LOCAL_COORD = 10
} VDP1CommandType;

typedef struct {
    uint16_t ctrl;
    uint16_t link;
    uint16_t pmod;
    uint16_t colr;
    uint16_t srca;
    uint16_t size;
    int16_t xa;
    int16_t ya;
    int16_t xb;
    int16_t yb;
    int16_t xc;
    int16_t yc;
    int16_t xd;
    int16_t yd;
    uint16_t grda;
    uint16_t reserved;
} __attribute__((packed)) VDP1Command;

void vdp1_init(void);
void vdp1_start(void);
void vdp1_wait_draw_end(void);
VDP1Command* vdp1_get_command(int index);

#endif // SATURN_VDP1_H