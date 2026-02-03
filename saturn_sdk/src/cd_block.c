// ============================================================================
// saturn_sdk/src/cd_block.c - CD-ROM Block Interface
// ============================================================================
#include "saturn.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ============================================================================
// CD Block Hardware Registers
// ============================================================================

#define CD_HIRQ         (*(volatile uint16_t*)0x25890008)  // Host IRQ
#define CD_HIRQ_MASK    (*(volatile uint16_t*)0x2589000C)  // Host IRQ Mask
#define CD_CR1          (*(volatile uint16_t*)0x25890018)  // Command Register 1
#define CD_CR2          (*(volatile uint16_t*)0x2589001C)  // Command Register 2
#define CD_CR3          (*(volatile uint16_t*)0x25890020)  // Command Register 3
#define CD_CR4          (*(volatile uint16_t*)0x25890024)  // Command Register 4
#define CD_DATATRNS     (*(volatile uint32_t*)0x25818000)  // Data Transfer

// HIRQ Flags
#define HIRQ_CMOK       0x0001  // Command dispatch possible
#define HIRQ_DRDY       0x0002  // Data ready
#define HIRQ_CSCT       0x0004  // 1 sector stored
#define HIRQ_BFUL       0x0008  // Buffer full
#define HIRQ_PEND       0x0010  // CD play end
#define HIRQ_DCHG       0x0020  // Disc change
#define HIRQ_ESEL       0x0040  // Selector setting finished
#define HIRQ_EHST       0x0080  // Host I/O finished
#define HIRQ_ECPY       0x0100  // Copy/Move finished
#define HIRQ_EFLS       0x0200  // File system processing finished
#define HIRQ_SCDQ       0x0400  // Subcode Q update
#define HIRQ_MPED       0x0800  // MPEG operation finished
#define HIRQ_MPCM       0x1000  // MPEG interrupt
#define HIRQ_MPST       0x2000  // MPEG status update

// CD Commands
#define CD_CMD_GET_STATUS       0x0000
#define CD_CMD_GET_HWINFO       0x0100
#define CD_CMD_GET_TOC          0x0200
#define CD_CMD_GET_SESSION      0x0300
#define CD_CMD_INIT             0x0400
#define CD_CMD_OPEN             0x0500
#define CD_CMD_END_TRANS        0x0600
#define CD_CMD_PLAY             0x1000
#define CD_CMD_SEEK             0x1100
#define CD_CMD_SCAN             0x1200
#define CD_CMD_GET_SUBCODE      0x2000
#define CD_CMD_SET_CDDEV        0x3000
#define CD_CMD_CHG_DIR          0x7000
#define CD_CMD_READ             0x7100
#define CD_CMD_GET_FILE_INFO    0x7300

// ============================================================================
// CD Block State
// ============================================================================

typedef struct {
    bool initialized;
    bool disc_present;
    uint16_t status;
    uint8_t toc[408];  // Table of Contents
    uint32_t current_fad;  // Current Frame Address
    uint32_t play_fad_start;
    uint32_t play_fad_end;
} CDBlockState;

static CDBlockState cd_state = {0};

// Sector buffer (2048 bytes + header)
static uint8_t sector_buffer[2352];

// ============================================================================
// Low-Level CD Block Functions
// ============================================================================

static void cd_wait_hirq(uint16_t flag) {
    while ((CD_HIRQ & flag) == 0) {
        // Wait for flag
    }
}

static void cd_clear_hirq(uint16_t flags) {
    CD_HIRQ = ~flags;
}

static uint16_t cd_send_command(uint16_t cmd, uint16_t arg1,
                                uint16_t arg2, uint16_t arg3) {
    // Wait for previous command to complete
    cd_wait_hirq(HIRQ_CMOK);

    // Clear CMOK flag
    cd_clear_hirq(HIRQ_CMOK);

    // Send command and arguments
    CD_CR1 = cmd;
    CD_CR2 = arg1;
    CD_CR3 = arg2;
    CD_CR4 = arg3;

    // Wait for command to be accepted
    cd_wait_hirq(HIRQ_CMOK | HIRQ_EFLS);

    // Get status
    uint16_t status = CD_CR1;

    return status;
}

static void cd_get_status(void) {
    cd_state.status = cd_send_command(CD_CMD_GET_STATUS, 0, 0, 0);
}

// ============================================================================
// FAD (Frame Address) Conversion
// ============================================================================

// Convert MSF (Minutes:Seconds:Frames) to FAD
static uint32_t msf_to_fad(uint8_t min, uint8_t sec, uint8_t frame) {
    return (min * 60 * 75) + (sec * 75) + frame + 150;
}

// Convert FAD to MSF
static void fad_to_msf(uint32_t fad, uint8_t *min, uint8_t *sec, uint8_t *frame) {
    fad -= 150;
    *min = fad / (60 * 75);
    fad %= (60 * 75);
    *sec = fad / 75;
    *frame = fad % 75;
}

// ============================================================================
// CD Block Initialization
// ============================================================================

bool cd_init(void) {
    // Send initialization command
    uint16_t status = cd_send_command(CD_CMD_INIT, 0, 0, 0);

    if ((status & 0xFF) != 0) {
        return false;  // Initialization failed
    }

    // Wait for disc change interrupt
    cd_wait_hirq(HIRQ_DCHG);
    cd_clear_hirq(HIRQ_DCHG);

    // Get TOC (Table of Contents)
    cd_send_command(CD_CMD_GET_TOC, 0, 0, 0);
    cd_wait_hirq(HIRQ_DRDY);

    // Read TOC data
    uint16_t *toc_ptr = (uint16_t*)cd_state.toc;
    for (int i = 0; i < 204; i++) {
        toc_ptr[i] = CD_DATATRNS >> 16;
    }

    cd_clear_hirq(HIRQ_DRDY);

    cd_state.initialized = true;
    cd_state.disc_present = true;

    return true;
}

// ============================================================================
// CD Playback Control
// ============================================================================

bool cd_play_audio(uint32_t start_fad, uint32_t end_fad) {
    if (!cd_state.initialized) {
        return false;
    }

    cd_state.play_fad_start = start_fad;
    cd_state.play_fad_end = end_fad;

    // Send play command
    uint16_t status = cd_send_command(CD_CMD_PLAY,
                                      start_fad >> 16,
                                      start_fad & 0xFFFF,
                                      (end_fad >> 16) | 0x8000);

    return (status & 0xFF) == 0;
}

bool cd_play_track(uint8_t track) {
    if (!cd_state.initialized || track == 0 || track > 99) {
        return false;
    }

    // Get track info from TOC
    uint8_t *toc = cd_state.toc;
    uint32_t start_fad = (toc[track * 4] << 16) |
                         (toc[track * 4 + 1] << 8) |
                         toc[track * 4 + 2];
    uint32_t end_fad = (toc[(track + 1) * 4] << 16) |
                       (toc[(track + 1) * 4 + 1] << 8) |
                       toc[(track + 1) * 4 + 2];

    return cd_play_audio(start_fad, end_fad);
}

void cd_pause(void) {
    if (!cd_state.initialized) {
        return;
    }

    cd_send_command(CD_CMD_PLAY, 0, 0, 0x8000);
}

void cd_resume(void) {
    if (!cd_state.initialized) {
        return;
    }

    cd_play_audio(cd_state.play_fad_start, cd_state.play_fad_end);
}

void cd_stop(void) {
    if (!cd_state.initialized) {
        return;
    }

    cd_send_command(CD_CMD_END_TRANS, 0, 0, 0);
}

// ============================================================================
// CD Data Reading
// ============================================================================

bool cd_seek(uint32_t fad) {
    if (!cd_state.initialized) {
        return false;
    }

    uint16_t status = cd_send_command(CD_CMD_SEEK,
                                      fad >> 16,
                                      fad & 0xFFFF, 0);

    cd_state.current_fad = fad;

    return (status & 0xFF) == 0;
}

bool cd_read_sector(uint32_t fad, void *buffer, uint32_t num_sectors) {
    if (!cd_state.initialized || buffer == NULL || num_sectors == 0) {
        return false;
    }

    // Seek to position
    if (!cd_seek(fad)) {
        return false;
    }

    // Set CD device connection
    cd_send_command(CD_CMD_SET_CDDEV, 0, 0, 0);

    // Read sectors
    uint16_t status = cd_send_command(CD_CMD_READ,
                                      fad >> 16,
                                      fad & 0xFFFF,
                                      (num_sectors << 8) | 0x80);

    if ((status & 0xFF) != 0) {
        return false;
    }

    // Wait for data ready
    cd_wait_hirq(HIRQ_DRDY);

    // Transfer data
    uint32_t *dest = (uint32_t*)buffer;
    for (uint32_t sector = 0; sector < num_sectors; sector++) {
        // Wait for sector
        cd_wait_hirq(HIRQ_CSCT);
        cd_clear_hirq(HIRQ_CSCT);

        // Read 2048 bytes (512 longs)
        for (int i = 0; i < 512; i++) {
            dest[i] = CD_DATATRNS;
        }
        dest += 512;
    }

    cd_clear_hirq(HIRQ_DRDY);
    cd_send_command(CD_CMD_END_TRANS, 0, 0, 0);

    return true;
}

// ============================================================================
// File System Functions
// ============================================================================

typedef struct {
    char name[32];
    uint32_t fad;
    uint32_t size;
    uint8_t flags;
} CDFileInfo;

bool cd_change_directory(const char *path) {
    if (!cd_state.initialized || path == NULL) {
        return false;
    }

    // Convert path to filter number (simplified)
    uint16_t filter = 0x0024;  // Root directory filter

    uint16_t status = cd_send_command(CD_CMD_CHG_DIR, filter, 0, 0);
    cd_wait_hirq(HIRQ_EFLS);
    cd_clear_hirq(HIRQ_EFLS);

    return (status & 0xFF) == 0;
}

bool cd_get_file_info(const char *filename, CDFileInfo *info) {
    if (!cd_state.initialized || filename == NULL || info == NULL) {
        return false;
    }

    // This is a simplified implementation
    // Real implementation would search directory entries

    memset(info, 0, sizeof(CDFileInfo));
    strncpy(info->name, filename, 31);

    // Send get file info command
    uint16_t status = cd_send_command(CD_CMD_GET_FILE_INFO, 0, 0, 0);

    if ((status & 0xFF) != 0) {
        return false;
    }

    cd_wait_hirq(HIRQ_DRDY);

    // Read file info (simplified)
    uint16_t data[4];
    data[0] = CD_DATATRNS >> 16;
    data[1] = CD_DATATRNS >> 16;
    data[2] = CD_DATATRNS >> 16;
    data[3] = CD_DATATRNS >> 16;

    info->fad = ((uint32_t)data[0] << 16) | data[1];
    info->size = ((uint32_t)data[2] << 16) | data[3];

    cd_clear_hirq(HIRQ_DRDY);

    return true;
}

bool cd_read_file(const char *filename, void *buffer, uint32_t *size) {
    if (!cd_state.initialized || filename == NULL || buffer == NULL) {
        return false;
    }

    // Get file information
    CDFileInfo info;
    if (!cd_get_file_info(filename, &info)) {
        return false;
    }

    // Calculate number of sectors
    uint32_t num_sectors = (info.size + 2047) / 2048;

    // Read the file
    bool success = cd_read_sector(info.fad, buffer, num_sectors);

    if (success && size != NULL) {
        *size = info.size;
    }

    return success;
}

// ============================================================================
// CD Status and Information
// ============================================================================

bool cd_is_disc_present(void) {
    cd_get_status();
    return (cd_state.status & 0x0F) != 0x00;
}

uint8_t cd_get_num_tracks(void) {
    if (!cd_state.initialized) {
        return 0;
    }

    // First track is at offset 0, last track at offset 1
    return cd_state.toc[1] - cd_state.toc[0] + 1;
}

bool cd_is_audio_track(uint8_t track) {
    if (!cd_state.initialized || track == 0 || track > 99) {
        return false;
    }

    // Check control byte in TOC
    uint8_t control = cd_state.toc[track * 4 + 3] >> 4;
    return (control & 0x04) == 0;  // Audio if bit 2 is 0
}

bool cd_is_playing(void) {
    cd_get_status();
    return (cd_state.status & 0x0100) != 0;  // Playing status
}

uint32_t cd_get_current_fad(void) {
    // Get subcode Q to find current position
    cd_send_command(CD_CMD_GET_SUBCODE, 0, 0, 0);
    cd_wait_hirq(HIRQ_DRDY);

    uint16_t data[5];
    for (int i = 0; i < 5; i++) {
        data[i] = CD_DATATRNS >> 16;
    }

    cd_clear_hirq(HIRQ_DRDY);

    // Extract FAD from subcode Q
    uint8_t min = data[3] >> 8;
    uint8_t sec = data[3] & 0xFF;
    uint8_t frame = data[4] >> 8;

    return msf_to_fad(min, sec, frame);
}

// ============================================================================
// CD Reset and Cleanup
// ============================================================================

void cd_reset(void) {
    cd_send_command(CD_CMD_INIT, 0, 1, 0);  // Reset CD block
    cd_state.initialized = false;
    cd_state.disc_present = false;
}

void cd_eject(void) {
    cd_send_command(CD_CMD_OPEN, 0, 0, 0);  // Open tray
}

// ============================================================================
// Utility Functions
// ============================================================================

void cd_get_disc_info(char *title, uint32_t *total_time) {
    if (!cd_state.initialized) {
        return;
    }

    // Read disc title from TOC (if available)
    if (title != NULL) {
        strncpy(title, "UNTITLED", 32);
    }

    // Calculate total disc time
    if (total_time != NULL) {
        uint8_t num_tracks = cd_get_num_tracks();
        if (num_tracks > 0) {
            uint32_t last_fad = (cd_state.toc[num_tracks * 4] << 16) |
                               (cd_state.toc[num_tracks * 4 + 1] << 8) |
                               cd_state.toc[num_tracks * 4 + 2];
            *total_time = last_fad / 75;  // Convert to seconds
        } else {
            *total_time = 0;
        }
    }
}