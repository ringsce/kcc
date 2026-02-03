// include/dreamcast_gdrom.h
// GD-ROM Drive Interface (1GB Gigabyte Disc)

#ifndef DREAMCAST_GDROM_H
#define DREAMCAST_GDROM_H

#include <stdint.h>

// GD-ROM Memory Map
#define GDROM_BASE          0xA05F7000

// GD-ROM Registers
#define GDROM_COMMAND       (*(volatile uint32_t*)(GDROM_BASE + 0x018))
#define GDROM_STATUS_REG    (*(volatile uint32_t*)(GDROM_BASE + 0x01C))
#define GDROM_DATA          (*(volatile uint32_t*)(GDROM_BASE + 0x084))
#define GDROM_RESET_ENABLE  (*(volatile uint32_t*)(GDROM_BASE + 0x090))
#define GDROM_VERSION       (*(volatile uint32_t*)(GDROM_BASE + 0x0F0))

// Sector size
#define GDROM_SECTOR_SIZE   2048

// Drive status
typedef enum {
    GDROM_STATUS_IDLE = 0,
    GDROM_STATUS_READING,
    GDROM_STATUS_SEEKING,
    GDROM_STATUS_ERROR,
    GDROM_STATUS_NO_DISC,
} GDROM_Status;

// Disc information
typedef struct {
    uint32_t total_sectors;
    uint8_t session_count;
    uint8_t disc_type;      // 0=CD-ROM, 0x80=GD-ROM
} GDROM_DiscInfo;

// Session information
typedef struct {
    uint32_t start_sector;
    uint32_t end_sector;
    uint8_t track_count;
} GDROM_SessionInfo;

// Track information
typedef struct {
    uint32_t start_sector;
    uint32_t end_sector;
    uint8_t track_number;
    uint8_t track_type;     // 0=data, 1=audio
} GDROM_TrackInfo;

// Initialization
void gdrom_init(void);
void gdrom_shutdown(void);

// Drive control
int gdrom_check_disc(void);
GDROM_Status gdrom_get_status(void);
void gdrom_stop(void);
int gdrom_spin_up(void);

// Reading
int gdrom_read_sectors(uint32_t sector, uint32_t count, void* buffer);
int gdrom_seek(uint32_t sector);

// Disc information
int gdrom_get_disc_info(GDROM_DiscInfo* info);
int gdrom_get_session_info(int session, GDROM_SessionInfo* info);
uint32_t gdrom_get_version(void);

#endif // DREAMCAST_GDROM_H