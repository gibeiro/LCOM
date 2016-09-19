

#define MOUSE_IRQ 12
#define LAB3_NOTIFICATION_ID 16
#define ESC_BREAKCODE 0x81
#define DELAY_US 20000
#define DOUBLE_BYTE 0xE0

#define KBD_NOTIFICATION_ID 1
#define MS_NOTIFICATION_ID 2

// KEYBOARD COMMANDS
#define KBD_RESET 0xFF
#define KBD_SETTODEFAULT 0xF6
#define KBD_DISABLE 0xF5
#define KBD_CLEARBUFFER 0xF4
#define KBD_SETLEDS 0xED
#define MS_WRITE 0xD4

// MOUSE COMMANDS
#define MS_DEFAULT 0xF6
#define MS_DISABLE_STREAMMODE 0xF5
#define MS_STREAMMODE 0xF4
#define MS_STATUSREQUEST 0xE9


// KEYBOARD REGISTERS
#define STAT_REG    0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define IN_BUF 0x60

#define TO_ERR (1 << 6)
#define PAR_ERR (1 << 7)
#define OBF (1 << 0)
#define IBF (1 << 1)

////////////////////////
#define IS_SYNC (1 << 3)

// FIST BYTE INFO
#define LEFT_B BIT(0)
#define RIGHT_B BIT(1)
#define MIDDLE_B BIT(2)
#define X_SIGN	BIT(4)
#define Y_SIGN	BIT(5)
#define X_OVF BIT(6)
#define Y_OVF BIT (7)
// STATUS INFO
#define L_BUTTON BIT(2)
#define R_BUTTON BIT(0)
#define M_BUTTON BIT(1)
#define SCALE BIT(4)
#define REPORT BIT(5)
#define MODE BIT(6)
