// include/dreamcast_aica.h
// AICA Sound Processor (Yamaha AICA - ARM7 based)

#ifndef DREAMCAST_AICA_H
#define DREAMCAST_AICA_H

#include <stdint.h>

// AICA Memory Map
#define AICA_BASE           0x00800000  // AICA control registers
#define AICA_RAM_BASE       0x00800000  // 2MB sound RAM

// AICA Control Registers
#define AICA_MASTER_CTRL    (*(volatile uint32_t*)(AICA_BASE + 0x2800))
#define AICA_MASTER_VOL     (*(volatile uint32_t*)(AICA_BASE + 0x2808))
#define AICA_CHANNEL_INFO   (*(volatile uint32_t*)(AICA_BASE + 0x280C))

// Channel registers (64 channels)
#define AICA_MAX_CHANNELS   64
#define AICA_CHANNEL_BASE   0x00800000

// Channel register structure
typedef struct {
    volatile uint32_t play_ctrl;    // Playback control
    volatile uint32_t sample_addr;  // Sample start address
    volatile uint32_t loop_start;   // Loop start position
    volatile uint32_t loop_end;     // Loop end position
    volatile uint32_t volume;       // Channel volume
    volatile uint32_t pan;          // Pan position
    volatile uint32_t pitch;        // Pitch/frequency
    volatile uint32_t sample_fmt;   // Sample format
} AICA_ChannelReg;

#define AICA_CHANNEL(n) (*(volatile AICA_ChannelReg*)(AICA_CHANNEL_BASE + 0x80 * (n)))

// Sound formats
typedef enum {
    AICA_FMT_16BIT = 0,     // 16-bit PCM
    AICA_FMT_8BIT = 1,      // 8-bit PCM
    AICA_FMT_ADPCM = 2,     // 4-bit ADPCM
} AICA_SampleFormat;

// Sound data structure
typedef struct {
    void* data;             // Pointer to sample data
    uint32_t size;          // Size in bytes
    uint32_t sample_rate;   // Sample rate (Hz)
    AICA_SampleFormat format;
    uint32_t loop_start;    // Loop start position
    uint32_t loop_end;      // Loop end position
    uint8_t volume;         // Volume (0-255)
    uint8_t pan;            // Pan (0=left, 15=center, 31=right)
    int loop;               // 1=loop, 0=one-shot
} AICA_SoundData;

// Channel state
typedef struct {
    int active;
    uint32_t sample_rate;
    uint8_t volume;
} AICA_Channel;

// Initialization
void aica_init(void);
void aica_shutdown(void);

// Channel control
void aica_channel_play(int channel, AICA_SoundData* sound);
void aica_channel_stop(int channel);
void aica_channel_set_volume(int channel, uint8_t volume);
void aica_channel_set_pan(int channel, uint8_t pan);
void aica_channel_set_pitch(int channel, uint16_t pitch);
int aica_channel_is_playing(int channel);
int aica_find_free_channel(void);

// Master control
void aica_set_master_volume(uint8_t volume);

// Memory management
void aica_upload_sound(void* dest, void* src, uint32_t size);

#endif // DREAMCAST_AICA_H