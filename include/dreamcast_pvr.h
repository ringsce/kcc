// include/dreamcast_pvr.h
// PowerVR2 CLX2 Graphics Processor for Sega Dreamcast
// 100 MHz graphics processor with tile-based deferred rendering

#ifndef DREAMCAST_PVR_H
#define DREAMCAST_PVR_H

#include <stdint.h>

// PowerVR2 Memory Map
#define PVR_BASE            0xA05F8000  // PVR control registers
#define PVR_VRAM_BASE       0xA5000000  // 8MB video RAM
#define PVR_TA_INPUT        0x10000000  // Tile Accelerator input

// PVR Control Registers
#define PVR_ID              (*(volatile uint32_t*)(PVR_BASE + 0x00))
#define PVR_REVISION        (*(volatile uint32_t*)(PVR_BASE + 0x04))
#define PVR_RESET           (*(volatile uint32_t*)(PVR_BASE + 0x08))
#define PVR_ISP_START       (*(volatile uint32_t*)(PVR_BASE + 0x14))
#define PVR_UNK_0018        (*(volatile uint32_t*)(PVR_BASE + 0x18))
#define PVR_ISP_VERTBUF_ADDR (*(volatile uint32_t*)(PVR_BASE + 0x20))

// Display registers
#define PVR_BORDER_COL      (*(volatile uint32_t*)(PVR_BASE + 0x40))
#define PVR_FB_CFG_1        (*(volatile uint32_t*)(PVR_BASE + 0x44))
#define PVR_FB_CFG_2        (*(volatile uint32_t*)(PVR_BASE + 0x48))
#define PVR_RENDER_MODULO   (*(volatile uint32_t*)(PVR_BASE + 0x4C))
#define PVR_DISPLAY_ADDR_1  (*(volatile uint32_t*)(PVR_BASE + 0x50))
#define PVR_DISPLAY_ADDR_2  (*(volatile uint32_t*)(PVR_BASE + 0x54))
#define PVR_DISPLAY_SIZE    (*(volatile uint32_t*)(PVR_BASE + 0x5C))

// Rendering configuration
#define PVR_FB_RENDER_CFG   (*(volatile uint32_t*)(PVR_BASE + 0x60))
#define PVR_FB_RENDER_ADDR1 (*(volatile uint32_t*)(PVR_BASE + 0x64))
#define PVR_FB_RENDER_ADDR2 (*(volatile uint32_t*)(PVR_BASE + 0x68))
#define PVR_FB_CLIP_X       (*(volatile uint32_t*)(PVR_BASE + 0x6C))
#define PVR_FB_CLIP_Y       (*(volatile uint32_t*)(PVR_BASE + 0x70))

// Tile accelerator
#define PVR_TA_OPB_START    (*(volatile uint32_t*)(PVR_BASE + 0x124))
#define PVR_TA_OPB_END      (*(volatile uint32_t*)(PVR_BASE + 0x128))
#define PVR_TA_OPB_POS      (*(volatile uint32_t*)(PVR_BASE + 0x12C))
#define PVR_TA_GLOB_TILE_CLIP (*(volatile uint32_t*)(PVR_BASE + 0x138))
#define PVR_TA_ALLOC_CTRL   (*(volatile uint32_t*)(PVR_BASE + 0x140))
#define PVR_TA_LIST_INIT    (*(volatile uint32_t*)(PVR_BASE + 0x144))

// Fog configuration
#define PVR_FOG_TABLE_COLOR (*(volatile uint32_t*)(PVR_BASE + 0xB0))
#define PVR_FOG_VERTEX_COLOR (*(volatile uint32_t*)(PVR_BASE + 0xB4))
#define PVR_FOG_DENSITY     (*(volatile uint32_t*)(PVR_BASE + 0xB8))

// Background plane
#define PVR_BG_PLANE_Z      (*(volatile uint32_t*)(PVR_BASE + 0x108))
#define PVR_BG_PLANE_CFG    (*(volatile uint32_t*)(PVR_BASE + 0x10C))

// Video modes
typedef enum {
    PVR_MODE_NTSC = 0,      // 640x480 @ 60Hz
    PVR_MODE_PAL = 1,       // 640x512 @ 50Hz
    PVR_MODE_VGA = 2,       // 640x480 @ 60Hz (VGA)
} PVR_VideoMode;

// Pixel formats
typedef enum {
    PVR_PIXFMT_RGB565 = 0,  // 16-bit RGB565
    PVR_PIXFMT_RGB555 = 1,  // 15-bit RGB555
    PVR_PIXFMT_RGB888 = 2,  // 24-bit RGB888
    PVR_PIXFMT_ARGB8888 = 3,// 32-bit ARGB8888
} PVR_PixelFormat;

// Texture formats
typedef enum {
    PVR_TXRFMT_ARGB1555 = 0,
    PVR_TXRFMT_RGB565 = 1,
    PVR_TXRFMT_ARGB4444 = 2,
    PVR_TXRFMT_YUV422 = 3,
    PVR_TXRFMT_BUMP = 4,
    PVR_TXRFMT_PAL4BPP = 5,
    PVR_TXRFMT_PAL8BPP = 6,
} PVR_TextureFormat;

// List types for tile accelerator
typedef enum {
    PVR_LIST_OP_POLY = 0,       // Opaque polygons
    PVR_LIST_OP_MOD = 1,        // Opaque modifiers
    PVR_LIST_TR_POLY = 2,       // Transparent polygons
    PVR_LIST_TR_MOD = 3,        // Transparent modifiers
    PVR_LIST_PT_POLY = 4,       // Punch-through polygons
} PVR_ListType;

// Vertex structure (basic)
typedef struct {
    float x, y, z;              // Position
    float u, v;                 // Texture coordinates
    uint32_t argb;              // Color (ARGB8888)
} __attribute__((packed)) PVR_Vertex;

// Polygon header
typedef struct {
    uint32_t cmd;               // Command word
    uint32_t mode1;             // Mode word 1
    uint32_t mode2;             // Mode word 2
    uint32_t mode3;             // Mode word 3
    uint32_t d1, d2, d3, d4;   // Dummy words for alignment
} __attribute__((packed)) PVR_PolyHeader;

// Initialization and setup
void pvr_init(void);
void pvr_shutdown(void);
void pvr_set_mode(PVR_VideoMode mode, PVR_PixelFormat format);
void pvr_set_bg_color(float r, float g, float b);

// Frame buffer management
void pvr_wait_ready(void);
void pvr_scene_begin(void);
void pvr_scene_finish(void);
void pvr_flip_buffers(void);

// List management
void pvr_list_begin(PVR_ListType list);
void pvr_list_finish(void);
void pvr_list_submit(void* data, uint32_t size);

// Primitive rendering
void pvr_prim_compile_hdr(PVR_PolyHeader* hdr, uint32_t blend_mode);
void pvr_draw_quad(PVR_Vertex* vertices);
void pvr_draw_triangle(PVR_Vertex* vertices);
void pvr_draw_sprite(float x, float y, float w, float h, uint32_t color);

// Texture management
uint32_t pvr_texture_load(void* data, uint32_t width, uint32_t height, PVR_TextureFormat format);
void pvr_texture_free(uint32_t texture_addr);
void pvr_texture_bind(uint32_t texture_addr);

// Memory management
void* pvr_mem_malloc(size_t size);
void pvr_mem_free(void* ptr);
void* pvr_mem_alloc_64(size_t size); // 64-byte aligned allocation

// Utilities
void pvr_fog_table_generate(float density);
void pvr_fog_vertex_color(float r, float g, float b, float a);
void pvr_set_zclip(float near_z, float far_z);

// Stats and info
typedef struct {
    uint32_t frame_count;
    uint32_t vram_used;
    uint32_t vram_total;
    float fps;
} PVR_Stats;

void pvr_get_stats(PVR_Stats* stats);

#endif // DREAMCAST_PVR_H