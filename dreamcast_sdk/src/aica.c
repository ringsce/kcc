// dreamcast_sdk/src/aica.c
// AICA Sound Processor Implementation

#include "dreamcast_aica.h"
#include <string.h>

// AICA channel states
static AICA_Channel aica_channels[AICA_MAX_CHANNELS];

// Initialize AICA
void aica_init(void) {
    // Reset AICA
    AICA_MASTER_CTRL = 0x00000000;

    // Clear all channels
    memset(aica_channels, 0, sizeof(aica_channels));

    for (int i = 0; i < AICA_MAX_CHANNELS; i++) {
        aica_channel_stop(i);
    }

    // Set master volume
    AICA_MASTER_VOL = 0x0F;

    // Enable AICA
    AICA_MASTER_CTRL = 0x00000001;
}

// Shutdown AICA
void aica_shutdown(void) {
    // Stop all channels
    for (int i = 0; i < AICA_MAX_CHANNELS; i++) {
        aica_channel_stop(i);
    }

    // Disable AICA
    AICA_MASTER_CTRL = 0x00000000;
}

// Play sound on channel
void aica_channel_play(int channel, AICA_SoundData* sound) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS || !sound) {
        return;
    }

    volatile AICA_ChannelReg* reg = &AICA_CHANNEL(channel);

    // Stop channel first
    reg->play_ctrl = 0x8000; // KEY_OFF

    // Set sample address
    reg->sample_addr = (uint32_t)sound->data & 0x00FFFFFF;

    // Set sample format
    uint32_t fmt = 0;
    switch (sound->format) {
        case AICA_FMT_16BIT:
            fmt = 0x00;
            break;
        case AICA_FMT_8BIT:
            fmt = 0x01;
            break;
        case AICA_FMT_ADPCM:
            fmt = 0x02;
            break;
    }

    reg->sample_fmt = fmt;

    // Set loop points
    reg->loop_start = sound->loop_start;
    reg->loop_end = sound->loop_end;

    // Set frequency (pitch)
    // AICA frequency = (sample_rate * 1024) / 44100
    uint32_t pitch = (sound->sample_rate << 10) / 44100;
    reg->pitch = pitch & 0xFFFF;

    // Set volume
    reg->volume = sound->volume & 0xFF;

    // Set pan (0=left, 0x0F=center, 0x1F=right)
    reg->pan = sound->pan & 0x1F;

    // Start playback
    uint32_t play_ctrl = 0x4000; // KEY_ON
    if (sound->loop) {
        play_ctrl |= 0x0200; // LOOP
    }
    reg->play_ctrl = play_ctrl;

    // Update channel state
    aica_channels[channel].active = 1;
    aica_channels[channel].sample_rate = sound->sample_rate;
    aica_channels[channel].volume = sound->volume;
}

// Stop channel
void aica_channel_stop(int channel) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS) {
        return;
    }

    volatile AICA_ChannelReg* reg = &AICA_CHANNEL(channel);
    reg->play_ctrl = 0x8000; // KEY_OFF

    aica_channels[channel].active = 0;
}

// Set channel volume
void aica_channel_set_volume(int channel, uint8_t volume) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS) {
        return;
    }

    volatile AICA_ChannelReg* reg = &AICA_CHANNEL(channel);
    reg->volume = volume & 0xFF;

    aica_channels[channel].volume = volume;
}

// Set channel pan
void aica_channel_set_pan(int channel, uint8_t pan) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS) {
        return;
    }

    volatile AICA_ChannelReg* reg = &AICA_CHANNEL(channel);
    reg->pan = pan & 0x1F;
}

// Set channel pitch
void aica_channel_set_pitch(int channel, uint16_t pitch) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS) {
        return;
    }

    volatile AICA_ChannelReg* reg = &AICA_CHANNEL(channel);
    reg->pitch = pitch & 0xFFFF;
}

// Check if channel is playing
int aica_channel_is_playing(int channel) {
    if (channel < 0 || channel >= AICA_MAX_CHANNELS) {
        return 0;
    }

    return aica_channels[channel].active;
}

// Set master volume
void aica_set_master_volume(uint8_t volume) {
    AICA_MASTER_VOL = volume & 0x0F;
}

// Upload sound data to AICA RAM
void aica_upload_sound(void* dest, void* src, uint32_t size) {
    // AICA RAM is at 0x00800000
    uint32_t aica_addr = (uint32_t)dest & 0x007FFFFF;
    volatile uint8_t* aica_ram = (volatile uint8_t*)(AICA_RAM_BASE + aica_addr);
    uint8_t* source = (uint8_t*)src;

    for (uint32_t i = 0; i < size; i++) {
        aica_ram[i] = source[i];
    }
}

// Find free channel
int aica_find_free_channel(void) {
    for (int i = 0; i < AICA_MAX_CHANNELS; i++) {
        if (!aica_channels[i].active) {
            return i;
        }
    }
    return -1; // No free channels
}