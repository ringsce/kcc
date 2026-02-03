// dreamcast_sdk/src/maple.c
// Maple Bus Implementation (Controller Interface)

#include "dreamcast_maple.h"
#include <string.h>

// Device state for each port
static MAPLE_Device maple_devices[MAPLE_MAX_PORTS];

// Initialize Maple Bus
void maple_init(void) {
    // Reset Maple Bus
    MAPLE_DMA_ENABLE = 0;
    MAPLE_SPEED = 0;

    // Clear device states
    memset(maple_devices, 0, sizeof(maple_devices));

    // Enable Maple Bus
    MAPLE_ENABLE = 1;
    MAPLE_SPEED = 2;

    // Scan for devices
    maple_scan_devices();
}

// Scan for connected devices
void maple_scan_devices(void) {
    for (int port = 0; port < MAPLE_MAX_PORTS; port++) {
        MAPLE_Device* dev = &maple_devices[port];

        // Send device info request
        uint32_t cmd = MAPLE_CMD_DEVICE_INFO;
        MAPLE_DMA_ADDR = (uint32_t)&cmd;
        MAPLE_DMA_ENABLE = 1;

        // Wait for response
        int timeout = 10000;
        while ((MAPLE_DMA_ENABLE & 1) && timeout--) {
            for (volatile int i = 0; i < 10; i++);
        }

        if (timeout > 0) {
            // Device found
            dev->connected = 1;
            dev->port = port;
            dev->device_id = MAPLE_DMA_ADDR & 0xFF;

            // Determine device type
            switch (dev->device_id) {
                case 0x01:
                    dev->device_type = MAPLE_DEVICE_CONTROLLER;
                    break;
                case 0x02:
                    dev->device_type = MAPLE_DEVICE_VMU;
                    break;
                case 0x04:
                    dev->device_type = MAPLE_DEVICE_KEYBOARD;
                    break;
                case 0x08:
                    dev->device_type = MAPLE_DEVICE_MOUSE;
                    break;
                default:
                    dev->device_type = MAPLE_DEVICE_UNKNOWN;
            }
        } else {
            dev->connected = 0;
        }
    }
}

// Read controller state
int maple_read_controller(int port, MAPLE_ControllerState* state) {
    if (port < 0 || port >= MAPLE_MAX_PORTS || !state) {
        return -1;
    }

    MAPLE_Device* dev = &maple_devices[port];
    if (!dev->connected || dev->device_type != MAPLE_DEVICE_CONTROLLER) {
        return -1;
    }

    // Send get condition command
    uint32_t cmd[2];
    cmd[0] = MAPLE_CMD_GET_CONDITION;
    cmd[1] = 0x01000000; // Function code for controller

    MAPLE_DMA_ADDR = (uint32_t)cmd;
    MAPLE_DMA_ENABLE = 1;

    // Wait for response
    int timeout = 10000;
    while ((MAPLE_DMA_ENABLE & 1) && timeout--) {
        for (volatile int i = 0; i < 10; i++);
    }

    if (timeout <= 0) {
        return -1;
    }

    // Parse response
    uint32_t* response = (uint32_t*)MAPLE_DMA_ADDR;

    // Buttons (inverted - 0 = pressed)
    uint16_t buttons = ~(response[2] & 0xFFFF);

    state->buttons = 0;
    if (buttons & 0x0004) state->buttons |= MAPLE_BTN_A;
    if (buttons & 0x0002) state->buttons |= MAPLE_BTN_B;
    if (buttons & 0x0008) state->buttons |= MAPLE_BTN_X;
    if (buttons & 0x0001) state->buttons |= MAPLE_BTN_Y;
    if (buttons & 0x0010) state->buttons |= MAPLE_BTN_START;
    if (buttons & 0x0100) state->buttons |= MAPLE_BTN_DPAD_UP;
    if (buttons & 0x0200) state->buttons |= MAPLE_BTN_DPAD_DOWN;
    if (buttons & 0x0400) state->buttons |= MAPLE_BTN_DPAD_LEFT;
    if (buttons & 0x0800) state->buttons |= MAPLE_BTN_DPAD_RIGHT;

    // Triggers (0-255)
    state->trigger_left = (response[3] >> 8) & 0xFF;
    state->trigger_right = response[3] & 0xFF;

    // Analog stick (-128 to 127)
    state->joystick_x = ((int8_t*)response)[16];
    state->joystick_y = ((int8_t*)response)[17];

    return 0;
}

// Check if device is connected
int maple_is_connected(int port) {
    if (port < 0 || port >= MAPLE_MAX_PORTS) {
        return 0;
    }
    return maple_devices[port].connected;
}

// Get device type
MAPLE_DeviceType maple_get_device_type(int port) {
    if (port < 0 || port >= MAPLE_MAX_PORTS) {
        return MAPLE_DEVICE_UNKNOWN;
    }
    return maple_devices[port].device_type;
}

// Get device info
int maple_get_device_info(int port, MAPLE_DeviceInfo* info) {
    if (port < 0 || port >= MAPLE_MAX_PORTS || !info) {
        return -1;
    }

    MAPLE_Device* dev = &maple_devices[port];
    if (!dev->connected) {
        return -1;
    }

    info->device_id = dev->device_id;
    info->device_type = dev->device_type;
    info->functions = 0x01000000; // Basic function
    strncpy(info->product_name, "Device", sizeof(info->product_name) - 1);
    strncpy(info->license, "Sega", sizeof(info->license) - 1);

    return 0;
}

// Shutdown Maple Bus
void maple_shutdown(void) {
    MAPLE_DMA_ENABLE = 0;
    MAPLE_ENABLE = 0;
}