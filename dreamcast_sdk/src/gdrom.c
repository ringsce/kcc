// dreamcast_sdk/src/gdrom.c
// GD-ROM Drive Implementation

#include "dreamcast_gdrom.h"
#include <string.h>

// Drive status
static GDROM_Status drive_status = GDROM_STATUS_IDLE;
static int disc_inserted = 0;

// Initialize GD-ROM
void gdrom_init(void) {
    // Reset GD-ROM subsystem
    GDROM_RESET_ENABLE = 0x1F;
    for (volatile int i = 0; i < 10000; i++);
    GDROM_RESET_ENABLE = 0x00;

    // Check for disc
    gdrom_check_disc();

    drive_status = GDROM_STATUS_IDLE;
}

// Check if disc is inserted
int gdrom_check_disc(void) {
    uint32_t status = GDROM_STATUS_REG;
    disc_inserted = (status & 0x01) ? 1 : 0;
    return disc_inserted;
}

// Get drive status
GDROM_Status gdrom_get_status(void) {
    return drive_status;
}

// Execute GD-ROM command
static int gdrom_exec_command(uint32_t cmd, void* params, uint32_t param_size) {
    // Wait for drive ready
    int timeout = 100000;
    while ((GDROM_STATUS_REG & 0x08) && timeout--) {
        for (volatile int i = 0; i < 100; i++);
    }

    if (timeout <= 0) {
        return -1; // Timeout
    }

    // Send command
    GDROM_COMMAND = cmd;

    // Send parameters if any
    if (params && param_size > 0) {
        uint8_t* p = (uint8_t*)params;
        for (uint32_t i = 0; i < param_size; i++) {
            GDROM_DATA = p[i];
        }
    }

    // Wait for completion
    timeout = 100000;
    while ((GDROM_STATUS_REG & 0x08) && timeout--) {
        for (volatile int i = 0; i < 100; i++);
    }

    return (timeout > 0) ? 0 : -1;
}

// Read sectors from GD-ROM
int gdrom_read_sectors(uint32_t sector, uint32_t count, void* buffer) {
    if (!disc_inserted || !buffer) {
        return -1;
    }

    drive_status = GDROM_STATUS_READING;

    // Prepare read command
    struct {
        uint32_t sector;
        uint32_t count;
    } params;

    params.sector = sector;
    params.count = count;

    // Execute read command (0x30 = READ CD)
    if (gdrom_exec_command(0x30, &params, sizeof(params)) != 0) {
        drive_status = GDROM_STATUS_ERROR;
        return -1;
    }

    // Transfer data
    uint32_t* dest = (uint32_t*)buffer;
    uint32_t total_bytes = count * GDROM_SECTOR_SIZE;

    for (uint32_t i = 0; i < total_bytes / 4; i++) {
        dest[i] = GDROM_DATA;
    }

    drive_status = GDROM_STATUS_IDLE;
    return 0;
}

// Seek to sector
int gdrom_seek(uint32_t sector) {
    if (!disc_inserted) {
        return -1;
    }

    drive_status = GDROM_STATUS_SEEKING;

    struct {
        uint32_t sector;
    } params;

    params.sector = sector;

    // Execute seek command (0x21 = SEEK)
    int result = gdrom_exec_command(0x21, &params, sizeof(params));

    drive_status = (result == 0) ? GDROM_STATUS_IDLE : GDROM_STATUS_ERROR;
    return result;
}

// Get disc information
int gdrom_get_disc_info(GDROM_DiscInfo* info) {
    if (!disc_inserted || !info) {
        return -1;
    }

    memset(info, 0, sizeof(GDROM_DiscInfo));

    // Execute TOC command (0x14 = READ TOC)
    uint8_t toc_data[1024];
    if (gdrom_exec_command(0x14, NULL, 0) != 0) {
        return -1;
    }

    // Read TOC data
    for (int i = 0; i < 1024; i++) {
        toc_data[i] = GDROM_DATA & 0xFF;
    }

    // Parse TOC (simplified)
    info->total_sectors = ((uint32_t)toc_data[0] << 16) |
                         ((uint32_t)toc_data[1] << 8) |
                         toc_data[2];
    info->session_count = toc_data[3];

    return 0;
}

// Get session information
int gdrom_get_session_info(int session, GDROM_SessionInfo* info) {
    if (!disc_inserted || !info || session < 0) {
        return -1;
    }

    // TODO: Implement session info parsing
    memset(info, 0, sizeof(GDROM_SessionInfo));

    return 0;
}

// Stop drive
void gdrom_stop(void) {
    // Execute stop command (0xE0 = STOP)
    gdrom_exec_command(0xE0, NULL, 0);
    drive_status = GDROM_STATUS_IDLE;
}

// Spin up drive
int gdrom_spin_up(void) {
    // Execute spin up command (0x70 = SPIN UP)
    int result = gdrom_exec_command(0x70, NULL, 0);
    if (result == 0) {
        drive_status = GDROM_STATUS_IDLE;
    }
    return result;
}

// Get drive firmware version
uint32_t gdrom_get_version(void) {
    return GDROM_VERSION;
}

// Shutdown GD-ROM
void gdrom_shutdown(void) {
    gdrom_stop();
    drive_status = GDROM_STATUS_IDLE;
}