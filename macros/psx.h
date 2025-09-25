/*
 * psx.h - A detailed header file for the Sony PlayStation 1 (PSX) hardware.
 *
 * This file contains constants and structures based on the known specifications
 * of the PlayStation's main components, reflecting the information from the
 * LSI Logic CW33300-based R3051 CPU and its integrated coprocessors.
 */

// =============================================================================
//      1. Header Guard
// =============================================================================
#ifndef PSX_H
#define PSX_H

// =============================================================================
//      2. Main CPU (LSI CoreWare CW33300 / MIPS R3051)
// =============================================================================
#define PSX_CPU_MODEL "LSI CoreWare CW33300 (MIPS R3051 compatible)"
#define PSX_CPU_CLOCK_HZ 33868800UL // 33.8688 MHz
#define PSX_CPU_MIPS 30             // 30 Million Instructions Per Second
#define PSX_BUS_BANDWIDTH_Bps 132000000UL // 132 MB/s

// --- CPU Cache ---
#define PSX_CPU_I_CACHE_SIZE_BYTES 4096 // 4 KB Instruction Cache
#define PSX_CPU_D_CACHE_SIZE_BYTES 1024 // 1 KB Data Cache (Scratchpad RAM)

// --- Manufacturing Details ---
#define PSX_CPU_FAB_PROCESS_NM 500  // 500 nm (0.5 micron) process node
#define PSX_CPU_MANUFACTURER "LSI Logic Corp. (SGI License)"

// Structure to hold all CPU-related specifications
typedef struct {
    const char* model;
    unsigned long clock_speed_hz;
    unsigned int mips;
    unsigned long bus_bandwidth_bytes_per_sec;
    unsigned int instruction_cache_bytes;
    unsigned int data_cache_bytes;
} PSX_CPU_Specs;


// =============================================================================
//      3. Geometry Transformation Engine (GTE) - Coprocessor 1
// =============================================================================
// The GTE is a coprocessor residing within the main CPU die.
#define PSX_GTE_MIPS 66 // 66 Million Instructions Per Second

// --- Polygon Rendering Performance (Polygons per second) ---
#define PSX_GTE_POLYS_PER_SEC_FLAT           360000
#define PSX_GTE_POLYS_PER_SEC_TEXTURED       180000
#define PSX_GTE_POLYS_PER_SEC_TEX_LIT_GOURAUD 90000

// Structure for GTE specifications
typedef struct {
    unsigned int mips;
    unsigned int pps_flat_shaded;
    unsigned int pps_textured;
    unsigned int pps_full_effect;
} PSX_GTE_Specs;


// =============================================================================
//      4. Motion Decoder (MDEC) - Coprocessor
// =============================================================================
// The MDEC also resides within the main CPU and handles video decompression.
#define PSX_MDEC_MIPS 80 // 80 Million Instructions Per Second

// Structure for MDEC specifications
typedef struct {
    unsigned int mips;
    const char* function;
} PSX_MDEC_Specs;


// =============================================================================
//      5. System Memory Map
// =============================================================================
#define PSX_RAM_MAIN_SIZE_BYTES  (2 * 1024 * 1024) // 2 MiB EDO DRAM
#define PSX_VRAM_SIZE_BYTES      (1 * 1024 * 1024) // 1 MiB Framebuffer (GPU RAM)
#define PSX_SPU_RAM_SIZE_BYTES   (512 * 1024)      // 512 KiB Sound RAM
#define PSX_BIOS_ROM_SIZE_BYTES  (512 * 1024)      // 512 KiB BIOS ROM
#define PSX_CDROM_BUFFER_BYTES   (32 * 1024)       // 32 KiB CD-ROM Buffer

// Structure for the memory layout
typedef struct {
    unsigned int main_ram_bytes;
    unsigned int vram_bytes;
    unsigned int spu_ram_bytes;
    unsigned int bios_rom_bytes;
    unsigned int cdrom_buffer_bytes;
} PSX_Memory_Map;


#endif // End of header guard PSX_H

