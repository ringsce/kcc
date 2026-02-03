// dreamcast_sdk/src/vmu.c
// Visual Memory Unit (VMU) Implementation

#include "dreamcast_vmu.h"
#include "dreamcast_maple.h"
#include <string.h>

// VMU directory cache
static VMU_DirEntry vmu_directory[VMU_MAX_FILES];

// Initialize VMU subsystem
void vmu_init(void) {
    memset(vmu_directory, 0, sizeof(vmu_directory));
}

// Check if VMU is present
int vmu_is_present(int port) {
    if (port < 0 || port >= MAPLE_MAX_PORTS) {
        return 0;
    }

    // Check if a VMU is connected to this port
    MAPLE_DeviceType type = maple_get_device_type(port);
    return (type == MAPLE_DEVICE_VMU) ? 1 : 0;
}

// Format VMU
int vmu_format(int port) {
    if (!vmu_is_present(port)) {
        return -1;
    }

    // Send format command via Maple
    uint32_t cmd[4];
    cmd[0] = 0x05; // BLOCK_WRITE command
    cmd[1] = 0x00020000; // Memory card function
    cmd[2] = 0; // Start block
    cmd[3] = VMU_TOTAL_BLOCKS; // Block count

    // TODO: Implement actual formatting via Maple DMA

    return 0;
}

// Read directory
int vmu_read_directory(int port) {
    if (!vmu_is_present(port)) {
        return -1;
    }

    // Read directory blocks (blocks 0-1)
    uint8_t dir_data[VMU_BLOCK_SIZE * 2];

    if (vmu_read_block(port, 0, dir_data, 2) != 0) {
        return -1;
    }

    // Parse directory entries
    memset(vmu_directory, 0, sizeof(vmu_directory));

    for (int i = 0; i < VMU_MAX_FILES; i++) {
        uint8_t* entry = &dir_data[i * 32];

        // Check if entry is valid (first byte != 0x00 or 0xFF)
        if (entry[0] != 0x00 && entry[0] != 0xFF) {
            VMU_DirEntry* dir = &vmu_directory[i];

            dir->file_type = entry[0];
            dir->copy_protected = entry[1] & 0x01;
            dir->start_block = (entry[2] << 8) | entry[3];
            memcpy(dir->filename, &entry[4], 12);
            dir->filename[12] = '\0';
            dir->timestamp = (entry[16] << 24) | (entry[17] << 16) |
                            (entry[18] << 8) | entry[19];
            dir->file_size = (entry[20] << 8) | entry[21];
            dir->header_offset = (entry[22] << 8) | entry[23];
        }
    }

    return 0;
}

// Get directory entry
int vmu_get_directory_entry(int index, VMU_DirEntry* entry) {
    if (index < 0 || index >= VMU_MAX_FILES || !entry) {
        return -1;
    }

    if (vmu_directory[index].filename[0] == '\0') {
        return -1; // Entry is empty
    }

    memcpy(entry, &vmu_directory[index], sizeof(VMU_DirEntry));
    return 0;
}

// Read block from VMU
int vmu_read_block(int port, uint16_t block, void* buffer, uint16_t count) {
    if (!vmu_is_present(port) || !buffer) {
        return -1;
    }

    if (block + count > VMU_TOTAL_BLOCKS) {
        return -1; // Out of range
    }

    // Send block read command via Maple
    uint32_t cmd[4];
    cmd[0] = 0x04; // BLOCK_READ command
    cmd[1] = 0x00020000; // Memory card function
    cmd[2] = block; // Start block
    cmd[3] = count; // Block count

    // TODO: Implement actual read via Maple DMA
    // For now, just clear the buffer
    memset(buffer, 0, count * VMU_BLOCK_SIZE);

    return 0;
}

// Write block to VMU
int vmu_write_block(int port, uint16_t block, void* buffer, uint16_t count) {
    if (!vmu_is_present(port) || !buffer) {
        return -1;
    }

    if (block + count > VMU_TOTAL_BLOCKS) {
        return -1; // Out of range
    }

    // Send block write command via Maple
    uint32_t cmd[4];
    cmd[0] = 0x05; // BLOCK_WRITE command
    cmd[1] = 0x00020000; // Memory card function
    cmd[2] = block; // Start block
    cmd[3] = count; // Block count

    // TODO: Implement actual write via Maple DMA

    return 0;
}

// Read file from VMU
int vmu_read_file(int port, const char* filename, void* buffer, uint32_t* size) {
    if (!vmu_is_present(port) || !filename || !buffer || !size) {
        return -1;
    }

    // Read directory first
    if (vmu_read_directory(port) != 0) {
        return -1;
    }

    // Find file in directory
    VMU_DirEntry* entry = NULL;
    for (int i = 0; i < VMU_MAX_FILES; i++) {
        if (strcmp(vmu_directory[i].filename, filename) == 0) {
            entry = &vmu_directory[i];
            break;
        }
    }

    if (!entry) {
        return -1; // File not found
    }

    // Calculate blocks needed
    uint16_t blocks = (entry->file_size + VMU_BLOCK_SIZE - 1) / VMU_BLOCK_SIZE;

    // Read file data
    if (vmu_read_block(port, entry->start_block, buffer, blocks) != 0) {
        return -1;
    }

    *size = entry->file_size;
    return 0;
}

// Write file to VMU
int vmu_write_file(int port, const char* filename, void* buffer, uint32_t size) {
    if (!vmu_is_present(port) || !filename || !buffer || size == 0) {
        return -1;
    }

    // TODO: Implement file writing
    // - Find free blocks
    // - Create directory entry
    // - Write data
    // - Update directory

    return -1; // Not implemented yet
}

// Delete file from VMU
int vmu_delete_file(int port, const char* filename) {
    if (!vmu_is_present(port) || !filename) {
        return -1;
    }

    // TODO: Implement file deletion
    // - Find file in directory
    // - Mark blocks as free
    // - Clear directory entry

    return -1; // Not implemented yet
}

// Get free space
uint32_t vmu_get_free_space(int port) {
    if (!vmu_is_present(port)) {
        return 0;
    }

    // Read directory and count free blocks
    if (vmu_read_directory(port) != 0) {
        return 0;
    }

    // TODO: Calculate actual free space
    // For now, return theoretical maximum
    return VMU_TOTAL_BLOCKS * VMU_BLOCK_SIZE;
}

// Shutdown VMU subsystem
void vmu_shutdown(void) {
    memset(vmu_directory, 0, sizeof(vmu_directory));
}