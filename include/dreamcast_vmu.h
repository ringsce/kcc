// include/dreamcast_vmu.h
// Visual Memory Unit (VMU) Interface
// 128KB storage, LCD display, standalone gaming

#ifndef DREAMCAST_VMU_H
#define DREAMCAST_VMU_H

#include <stdint.h>

// VMU specifications
#define VMU_BLOCK_SIZE      512     // Bytes per block
#define VMU_TOTAL_BLOCKS    256     // Total blocks (128KB)
#define VMU_MAX_FILES       200     // Maximum directory entries

// File types
#define VMU_FILE_TYPE_DATA  0x33    // Data file
#define VMU_FILE_TYPE_GAME  0xCC    // VMU game

// Directory entry structure
typedef struct {
    uint8_t file_type;              // File type (data/game)
    uint8_t copy_protected;         // Copy protection flag
    uint16_t start_block;           // Starting block number
    char filename[13];              // Filename (12 chars + null)
    uint32_t timestamp;             // Creation timestamp
    uint16_t file_size;             // File size in bytes
    uint16_t header_offset;         // Header data offset
} VMU_DirEntry;

// VMU file header
typedef struct {
    char description[16];           // Description (Japanese)
    char description_en[32];        // Description (English)
    char creator[16];               // Creator name
    uint16_t icon_count;            // Number of icon frames
    uint16_t icon_speed;            // Animation speed
    uint16_t eyecatch_type;         // Eyecatch graphics type
    uint16_t crc;                   // CRC checksum
    uint32_t file_size;             // Total file size
    uint8_t reserved[20];           // Reserved
    uint16_t icon_palette[16];      // Icon palette (4-bit)
    uint8_t icon_data[512];         // Icon bitmap data
} VMU_FileHeader;

// VMU info
typedef struct {
    uint32_t total_space;           // Total space in bytes
    uint32_t free_space;            // Free space in bytes
    uint16_t total_blocks;          // Total blocks
    uint16_t free_blocks;           // Free blocks
    uint16_t file_count;            // Number of files
} VMU_Info;

// Initialization
void vmu_init(void);
void vmu_shutdown(void);

// Device management
int vmu_is_present(int port);
int vmu_format(int port);

// Directory operations
int vmu_read_directory(int port);
int vmu_get_directory_entry(int index, VMU_DirEntry* entry);

// Block-level I/O
int vmu_read_block(int port, uint16_t block, void* buffer, uint16_t count);
int vmu_write_block(int port, uint16_t block, void* buffer, uint16_t count);

// File operations
int vmu_read_file(int port, const char* filename, void* buffer, uint32_t* size);
int vmu_write_file(int port, const char* filename, void* buffer, uint32_t size);
int vmu_delete_file(int port, const char* filename);

// Information
uint32_t vmu_get_free_space(int port);

#endif // DREAMCAST_VMU_H