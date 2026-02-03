// include/dreamcast_maple.h
// Maple Bus Interface (Controllers, VMU, Keyboard, Mouse)

#ifndef DREAMCAST_MAPLE_H
#define DREAMCAST_MAPLE_H

#include <stdint.h>

// Maple Bus Memory Map
#define MAPLE_BASE          0xA05F6C00

// Maple Registers
#define MAPLE_DMA_ADDR      (*(volatile uint32_t*)(MAPLE_BASE + 0x04))
#define MAPLE_RESET         (*(volatile uint32_t*)(MAPLE_BASE + 0x0C))
#define MAPLE_ENABLE        (*(volatile uint32_t*)(MAPLE_BASE + 0x14))
#define MAPLE_DMA_ENABLE    (*(volatile uint32_t*)(MAPLE_BASE + 0x18))
#define MAPLE_SPEED         (*(volatile uint32_t*)(MAPLE_BASE + 0x80))

// Maple commands
#define MAPLE_CMD_DEVICE_INFO       0x01
#define MAPLE_CMD_GET_CONDITION     0x09
#define MAPLE_CMD_BLOCK_READ        0x04
#define MAPLE_CMD_BLOCK_WRITE       0x05

// Maximum devices
#define MAPLE_MAX_PORTS     4
#define MAPLE_MAX_UNITS     6  // Main unit + 5 sub-units per port

// Device types
typedef enum {
    MAPLE_DEVICE_UNKNOWN = 0,
    MAPLE_DEVICE_CONTROLLER = 1,
    MAPLE_DEVICE_VMU = 2,
    MAPLE_DEVICE_KEYBOARD = 4,
    MAPLE_DEVICE_MOUSE = 8,
} MAPLE_DeviceType;

// Controller buttons
typedef enum {
    MAPLE_BTN_A         = 0x0001,
    MAPLE_BTN_B         = 0x0002,
    MAPLE_BTN_X         = 0x0004,
    MAPLE_BTN_Y         = 0x0008,
    MAPLE_BTN_START     = 0x0010,
    MAPLE_BTN_DPAD_UP   = 0x0020,
    MAPLE_BTN_DPAD_DOWN = 0x0040,
    MAPLE_BTN_DPAD_LEFT = 0x0080,
    MAPLE_BTN_DPAD_RIGHT= 0x0100,
    MAPLE_BTN_L_TRIGGER = 0x0200,
    MAPLE_BTN_R_TRIGGER = 0x0400,
} MAPLE_Buttons;

// Controller state
typedef struct {
    uint16_t buttons;       // Button bitfield
    uint8_t trigger_left;   // Left trigger (0-255)
    uint8_t trigger_right;  // Right trigger (0-255)
    int8_t joystick_x;      // Analog X (-128 to 127)
    int8_t joystick_y;      // Analog Y (-128 to 127)
} MAPLE_ControllerState;

// Device information
typedef struct {
    uint32_t device_id;
    MAPLE_DeviceType device_type;
    uint32_t functions;
    char product_name[32];
    char license[64];
} MAPLE_DeviceInfo;

// Internal device structure
typedef struct {
    int connected;
    int port;
    uint32_t device_id;
    MAPLE_DeviceType device_type;
} MAPLE_Device;

// Initialization
void maple_init(void);
void maple_shutdown(void);

// Device scanning
void maple_scan_devices(void);
int maple_is_connected(int port);
MAPLE_DeviceType maple_get_device_type(int port);
int maple_get_device_info(int port, MAPLE_DeviceInfo* info);

// Controller functions
int maple_read_controller(int port, MAPLE_ControllerState* state);

#endif // DREAMCAST_MAPLE_H