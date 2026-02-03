// ============================================================================
// include/saturn_cd.h - Sega Saturn CD-ROM Block Interface
// ============================================================================
#ifndef SATURN_CD_H
#define SATURN_CD_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// CD File Information
// ============================================================================

typedef struct {
    char name[32];          // File name
    uint32_t fad;          // Frame Address (starting sector)
    uint32_t size;         // File size in bytes
    uint8_t flags;         // File flags (directory, hidden, etc.)
    uint8_t unit;          // File unit number
    uint8_t gap;           // Gap size
    uint8_t file_number;   // File number
} CDFileInfo;

// File flags
#define CD_FILE_HIDDEN      0x01
#define CD_FILE_DIRECTORY   0x02
#define CD_FILE_ASSOCIATED  0x04
#define CD_FILE_RECORD      0x08
#define CD_FILE_PROTECTION  0x10
#define CD_FILE_MULTI_EXTENT 0x80

// ============================================================================
// CD Track Information
// ============================================================================

typedef struct {
    uint8_t track_number;
    uint8_t control;       // Control nibble (audio/data)
    uint8_t adr;          // ADR nibble
    uint32_t fad_start;   // Starting FAD
    uint32_t fad_end;     // Ending FAD (approximate)
    bool is_audio;        // True if audio track
    bool is_data;         // True if data track
} CDTrackInfo;

// Control bits
#define CD_CTRL_AUDIO       0x00  // Audio track (2 channels)
#define CD_CTRL_DATA        0x04  // Data track
#define CD_CTRL_COPY_PROHIBITED 0x00
#define CD_CTRL_COPY_PERMITTED  0x02
#define CD_CTRL_PREEMPHASIS_NONE 0x00
#define CD_CTRL_PREEMPHASIS_5015 0x01

// ============================================================================
// CD Initialization and Control
// ============================================================================

/**
 * Initialize the CD block
 * Returns: true on success, false on failure
 */
bool cd_init(void);

/**
 * Reset the CD block to initial state
 */
void cd_reset(void);

/**
 * Open the CD tray (eject disc)
 */
void cd_eject(void);

/**
 * Check if a disc is present
 * Returns: true if disc is present
 */
bool cd_is_disc_present(void);

/**
 * Check if CD is currently playing
 * Returns: true if playing
 */
bool cd_is_playing(void);

// ============================================================================
// Audio Playback Functions
// ============================================================================

/**
 * Play audio from start_fad to end_fad
 * @param start_fad Starting frame address
 * @param end_fad Ending frame address
 * Returns: true on success
 */
bool cd_play_audio(uint32_t start_fad, uint32_t end_fad);

/**
 * Play a specific audio track
 * @param track Track number (1-99)
 * Returns: true on success
 */
bool cd_play_track(uint8_t track);

/**
 * Pause audio playback
 */
void cd_pause(void);

/**
 * Resume audio playback
 */
void cd_resume(void);

/**
 * Stop audio playback
 */
void cd_stop(void);

/**
 * Set audio volume
 * @param volume Volume level (0-255)
 */
void cd_set_volume(uint8_t volume);

// ============================================================================
// Data Reading Functions
// ============================================================================

/**
 * Seek to a specific FAD (frame address)
 * @param fad Target frame address
 * Returns: true on success
 */
bool cd_seek(uint32_t fad);

/**
 * Read data sectors from CD
 * @param fad Starting frame address
 * @param buffer Destination buffer (must be at least 2048*num_sectors bytes)
 * @param num_sectors Number of sectors to read
 * Returns: true on success
 */
bool cd_read_sector(uint32_t fad, void *buffer, uint32_t num_sectors);

/**
 * Read multiple sectors asynchronously
 * @param fad Starting frame address
 * @param buffer Destination buffer
 * @param num_sectors Number of sectors
 * @param callback Function to call when complete
 * Returns: true if read started successfully
 */
bool cd_read_sector_async(uint32_t fad, void *buffer, uint32_t num_sectors,
                          void (*callback)(bool success));

// ============================================================================
// File System Functions
// ============================================================================

/**
 * Change to a directory
 * @param path Directory path (e.g., "/DATA")
 * Returns: true on success
 */
bool cd_change_directory(const char *path);

/**
 * Get information about a file
 * @param filename Name of the file
 * @param info Pointer to CDFileInfo structure to fill
 * Returns: true on success
 */
bool cd_get_file_info(const char *filename, CDFileInfo *info);

/**
 * Read an entire file into memory
 * @param filename Name of the file
 * @param buffer Destination buffer
 * @param size Pointer to receive file size (can be NULL)
 * Returns: true on success
 */
bool cd_read_file(const char *filename, void *buffer, uint32_t *size);

/**
 * List files in current directory
 * @param callback Function called for each file found
 * Returns: Number of files found
 */
int cd_list_files(void (*callback)(const CDFileInfo *info));

// ============================================================================
// TOC (Table of Contents) Functions
// ============================================================================

/**
 * Get the number of tracks on the disc
 * Returns: Number of tracks (0 if error)
 */
uint8_t cd_get_num_tracks(void);

/**
 * Get information about a specific track
 * @param track Track number (1-99)
 * @param info Pointer to CDTrackInfo structure to fill
 * Returns: true on success
 */
bool cd_get_track_info(uint8_t track, CDTrackInfo *info);

/**
 * Check if a track is audio
 * @param track Track number
 * Returns: true if audio track
 */
bool cd_is_audio_track(uint8_t track);

/**
 * Check if a track is data
 * @param track Track number
 * Returns: true if data track
 */
bool cd_is_data_track(uint8_t track);

// ============================================================================
// Position and Time Functions
// ============================================================================

/**
 * Get current playback position
 * Returns: Current FAD (frame address)
 */
uint32_t cd_get_current_fad(void);

/**
 * Get current playback time in MSF format
 * @param min Minutes (output)
 * @param sec Seconds (output)
 * @param frame Frames (output)
 */
void cd_get_current_msf(uint8_t *min, uint8_t *sec, uint8_t *frame);

/**
 * Convert MSF to FAD
 * @param min Minutes
 * @param sec Seconds
 * @param frame Frames
 * Returns: Frame Address
 */
uint32_t cd_msf_to_fad(uint8_t min, uint8_t sec, uint8_t frame);

/**
 * Convert FAD to MSF
 * @param fad Frame Address
 * @param min Minutes (output)
 * @param sec Seconds (output)
 * @param frame Frames (output)
 */
void cd_fad_to_msf(uint32_t fad, uint8_t *min, uint8_t *sec, uint8_t *frame);

// ============================================================================
// Disc Information
// ============================================================================

/**
 * Get disc information
 * @param title Buffer for disc title (can be NULL)
 * @param total_time Total disc time in seconds (can be NULL)
 */
void cd_get_disc_info(char *title, uint32_t *total_time);

/**
 * Get disc type
 * Returns: 0=no disc, 1=audio CD, 2=CD-ROM, 3=mixed mode
 */
uint8_t cd_get_disc_type(void);

// ============================================================================
// Advanced Functions
// ============================================================================

/**
 * Set CD sector size mode
 * @param mode 0=2048 bytes (data only), 1=2352 bytes (raw)
 */
void cd_set_sector_size(uint8_t mode);

/**
 * Get CD status register value
 * Returns: Status register value
 */
uint16_t cd_get_status(void);

/**
 * Wait for CD operation to complete
 * @param timeout Timeout in milliseconds (0 = no timeout)
 * Returns: true if completed, false if timeout
 */
bool cd_wait_ready(uint32_t timeout);

/**
 * Abort current CD operation
 */
void cd_abort(void);

// ============================================================================
// Subcode Functions (for CD+G, CD+MIDI, etc.)
// ============================================================================

typedef struct {
    uint8_t control_adr;
    uint8_t track_number;
    uint8_t index;
    uint8_t min;
    uint8_t sec;
    uint8_t frame;
    uint8_t abs_min;
    uint8_t abs_sec;
    uint8_t abs_frame;
} CDSubcodeQ;

/**
 * Get current subcode Q data
 * @param subcode Pointer to CDSubcodeQ structure to fill
 * Returns: true on success
 */
bool cd_get_subcode_q(CDSubcodeQ *subcode);

// ============================================================================
// Error Codes
// ============================================================================

typedef enum {
    CD_ERR_OK = 0,
    CD_ERR_NO_DISC,
    CD_ERR_NOT_READY,
    CD_ERR_INVALID_PARAM,
    CD_ERR_READ_ERROR,
    CD_ERR_SEEK_ERROR,
    CD_ERR_TIMEOUT,
    CD_ERR_FILE_NOT_FOUND,
    CD_ERR_NO_INIT
} CDError;

/**
 * Get last error code
 * Returns: Error code
 */
CDError cd_get_last_error(void);

/**
 * Get error message string
 * @param error Error code
 * Returns: Error message
 */
const char* cd_get_error_string(CDError error);

// ============================================================================
// DMA Transfer Functions (for faster data transfer)
// ============================================================================

/**
 * Read sectors using DMA
 * @param fad Starting frame address
 * @param buffer Destination buffer (must be in appropriate memory region)
 * @param num_sectors Number of sectors
 * Returns: true on success
 */
bool cd_read_sector_dma(uint32_t fad, void *buffer, uint32_t num_sectors);

// ============================================================================
// CD-ROM XA (eXtended Architecture) Functions
// ============================================================================

/**
 * Enable CD-ROM XA mode
 */
void cd_enable_xa(void);

/**
 * Disable CD-ROM XA mode
 */
void cd_disable_xa(void);

/**
 * Play XA audio track
 * @param track Track number
 * Returns: true on success
 */
bool cd_play_xa_audio(uint8_t track);

// ============================================================================
// Utility Macros
// ============================================================================

// Convert between sectors and bytes
#define CD_SECTOR_SIZE      2048
#define CD_RAW_SECTOR_SIZE  2352
#define CD_BYTES_TO_SECTORS(bytes) (((bytes) + CD_SECTOR_SIZE - 1) / CD_SECTOR_SIZE)
#define CD_SECTORS_TO_BYTES(sectors) ((sectors) * CD_SECTOR_SIZE)

// FAD constants
#define CD_FAD_START        150     // First data sector FAD
#define CD_FAD_PREGAP       150     // Standard pregap size

// Timing constants
#define CD_FRAMES_PER_SEC   75      // CD frames per second
#define CD_SECTORS_PER_SEC  75      // Sectors per second

#endif // SATURN_CD_H