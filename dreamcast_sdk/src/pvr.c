// dreamcast_sdk/src/pvr.c
// PowerVR2 CLX2 Graphics Implementation

#include "dreamcast_pvr.h"
#include <string.h>

// Current video mode
static PVR_VideoMode current_mode = PVR_MODE_NTSC;
static PVR_PixelFormat current_format = PVR_PIXFMT_RGB565;

// Frame buffer addresses
static uint32_t fb_addr[2] = {0, 0};
static int current_fb = 0;

// VRAM allocator
static uint32_t vram_ptr = 0;

// Initialize PowerVR2
void pvr_init(void) {
    // Software reset
    PVR_RESET = 0xFFFFFFFF;
    for (volatile int i = 0; i < 10000; i++);
    PVR_RESET = 0x00000000;

    // Set default parameters
    PVR_ISP_VERTBUF_ADDR = 0x00000000;

    // Initialize VRAM allocator
    vram_ptr = 0x00200000; // Start after display buffers
}

// Set video mode
void pvr_set_mode(PVR_VideoMode mode, PVR_PixelFormat format) {
    current_mode = mode;
    current_format = format;

    uint32_t width, height;

    switch (mode) {
        case PVR_MODE_NTSC:
        case PVR_MODE_VGA:
            width = 640;
            height = 480;
            break;
        case PVR_MODE_PAL:
            width = 640;
            height = 512;
            break;
        default:
            width = 640;
            height = 480;
    }

    // Calculate framebuffer size
    uint32_t bytes_per_pixel = (format == PVR_PIXFMT_RGB888 ||
                                 format == PVR_PIXFMT_ARGB8888) ? 4 : 2;
    uint32_t fb_size = width * height * bytes_per_pixel;

    // Setup double buffering
    fb_addr[0] = 0x00000000;
    fb_addr[1] = fb_size;

    // Configure display
    PVR_DISPLAY_ADDR_1 = fb_addr[0];
    PVR_DISPLAY_ADDR_2 = fb_addr[1];
    PVR_DISPLAY_SIZE = ((height - 1) << 10) | ((width * bytes_per_pixel / 8) - 1);

    // Configure rendering
    uint32_t fb_cfg = (format << 2);
    PVR_FB_CFG_1 = fb_cfg;
    PVR_FB_CFG_2 = fb_cfg;
}

// Set background color
void pvr_set_bg_color(float r, float g, float b) {
    uint32_t ir = (uint32_t)(r * 255.0f);
    uint32_t ig = (uint32_t)(g * 255.0f);
    uint32_t ib = (uint32_t)(b * 255.0f);

    PVR_BORDER_COL = (ir << 16) | (ig << 8) | ib;
}

// Wait for PVR ready
void pvr_wait_ready(void) {
    // Wait for vertical blank
    while (!(PVR_RESET & 0x01));
    while (PVR_RESET & 0x01);
}

// Begin scene rendering
void pvr_scene_begin(void) {
    // Start new frame
    PVR_ISP_START = 0xFFFFFFFF;
}

// Finish scene rendering
void pvr_scene_finish(void) {
    // Signal end of scene
    PVR_TA_LIST_INIT = 0x80000000;
}

// Flip display buffers
void pvr_flip_buffers(void) {
    current_fb = 1 - current_fb;

    // Update display address
    if (current_fb == 0) {
        PVR_FB_RENDER_ADDR1 = fb_addr[1];
    } else {
        PVR_FB_RENDER_ADDR1 = fb_addr[0];
    }
}

// Begin rendering list
void pvr_list_begin(PVR_ListType list) {
    // Configure list type
    PVR_TA_ALLOC_CTRL = (list << 20);
}

// Finish rendering list
void pvr_list_finish(void) {
    // Submit list end marker
    volatile uint32_t* ta = (volatile uint32_t*)PVR_TA_INPUT;
    *ta = 0;
}

// Submit data to tile accelerator
void pvr_list_submit(void* data, uint32_t size) {
    volatile uint32_t* ta = (volatile uint32_t*)PVR_TA_INPUT;
    uint32_t* src = (uint32_t*)data;

    for (uint32_t i = 0; i < size / 4; i++) {
        *ta = *src++;
    }
}

// Draw a sprite (simple quad)
void pvr_draw_sprite(float x, float y, float w, float h, uint32_t color) {
    PVR_Vertex vertices[4];

    // Top-left
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 1.0f;
    vertices[0].argb = color;

    // Top-right
    vertices[1].x = x + w;
    vertices[1].y = y;
    vertices[1].z = 1.0f;
    vertices[1].argb = color;

    // Bottom-left
    vertices[2].x = x;
    vertices[2].y = y + h;
    vertices[2].z = 1.0f;
    vertices[2].argb = color;

    // Bottom-right
    vertices[3].x = x + w;
    vertices[3].y = y + h;
    vertices[3].z = 1.0f;
    vertices[3].argb = color;

    pvr_draw_quad(vertices);
}

// Draw quad
void pvr_draw_quad(PVR_Vertex* vertices) {
    // Submit vertices to TA
    pvr_list_submit(vertices, sizeof(PVR_Vertex) * 4);
}

// Draw triangle
void pvr_draw_triangle(PVR_Vertex* vertices) {
    pvr_list_submit(vertices, sizeof(PVR_Vertex) * 3);
}

// Allocate VRAM
void* pvr_mem_malloc(size_t size) {
    if (vram_ptr + size > 0x00800000) {
        return NULL; // Out of VRAM
    }

    void* ptr = (void*)(PVR_VRAM_BASE + vram_ptr);
    vram_ptr += (size + 31) & ~31; // 32-byte align

    return ptr;
}

// 64-byte aligned allocation
void* pvr_mem_alloc_64(size_t size) {
    vram_ptr = (vram_ptr + 63) & ~63;
    return pvr_mem_malloc(size);
}

// Free VRAM (simple allocator doesn't support individual frees)
void pvr_mem_free(void* ptr) {
    (void)ptr;
    // TODO: Implement proper memory management
}

// Get statistics
void pvr_get_stats(PVR_Stats* stats) {
    if (!stats) return;

    stats->vram_total = 0x00800000; // 8MB
    stats->vram_used = vram_ptr;
    stats->frame_count = 0; // TODO: Track frames
    stats->fps = 60.0f; // TODO: Calculate actual FPS
}

// Shutdown PVR
void pvr_shutdown(void) {
    // Software reset
    PVR_RESET = 0xFFFFFFFF;
}