// include/dreamcast.h
// Sega Dreamcast Hardware Definitions

#ifndef DREAMCAST_H
#define DREAMCAST_H

#include <stdint.h>
#include <stddef.h>

// Dreamcast Memory Map
#define DC_RAM_BASE         0x8C000000  // 16MB main RAM
#define DC_VRAM_BASE        0xA5000000  // 8MB video RAM
#define DC_AICA_RAM_BASE    0x00800000  // 2MB AICA sound RAM
#define DC_FLASH_BASE       0x00200000  // 256KB flash ROM

// Hardware initialization
void dreamcast_init(void);
void dreamcast_video_init(void);
void dreamcast_audio_init(void);
void dreamcast_controller_init(void);
void dreamcast_shutdown(void);

// Memory utilities
void* dreamcast_memcpy(void* dest, const void* src, size_t n);
void* dreamcast_memset(void* s, int c, size_t n);

// I/O functions
void dreamcast_print(const char* str);

// Synchronization
void dreamcast_vsync(void);

#endif // DREAMCAST_H