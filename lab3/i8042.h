

#define TIMER1_IRQ 1
#define LAB3_NOTIFICATION_ID 16
#define ESC_BREAKCODE 0x81
#define DELAY_US 20000
#define DOUBLE_BYTE 0xE0

#define KBD_NOTIFICATION_ID 1

// KEYBOARD COMMANDS
#define KBD_RESET 0xFF
#define KBD_SETTODEFAULT 0xF6
#define KBD_DISABLE 0xF5
#define KBD_CLEARBUFFER 0xF4
#define KBD_SETLEDS 0xED


// KEYBOARD REGISTERS
#define STAT_REG    0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60

#define TO_ERR (1 << 6)
#define PAR_ERR (1 << 7)
#define OBF (1 << 0)
#define IBF (1 << 1)
