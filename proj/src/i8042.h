#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


//KBC COMMANDS
#define STAT_REG 			0x64
#define IN_BUF 				0x64
#define KBC_CMD_REG			0x64
#define OUT_BUF 			0x60
#define DELAY_US 			20000
#define BREAK_CODE 			0x81
#define MAKE_CODE 			0x01
#define BIT(n) 				(0x01<<(n))
#define OBF 				BIT(0)
#define IBF 				BIT(1)
#define PAR_ERR 			BIT(7)
#define TO_ERR 				BIT(5)
#define LEDS_CHANGE 		0xED
#define KBCTOMOUSE 			0xD4
#define ACK 				0xFA

//PS/2 Mouse COMMAND SET (from The PS/2 Mouse Interface)
#define MOUSE_IRQ 			12
#define RESET_CMD 			0xFF
#define RESEND_CMD 			0xFE
#define SET_DEF 			0xF6
#define DIS_DATA_REP		0xF5
#define EN_DATA_REP			0xF4
#define SET_SAMPLE_RATE		0xF3
#define DEV_ID 				0xF2
#define REMOTE_MODE 		0xF0
#define SET_WRAP_MODE 		0xEE
#define RESET_WRAP_MODE 	0xEC
#define READ_DATA 			0xEB
#define SET_STREAM_MODE 	0xEA
#define STATUS_REQ 			0xE9


//MOVEMENT DATA PACKETS  (from The PS/2 Mouse Interface)

//The standard PS/2 mouse sends movement/button information to
//the host using the following 3-byte packet:
//1ST BYTE
#define LEFT_BTN 			BIT(0)
#define RIGHT_BTN 			BIT(1)
#define MIDDLE_BTN 			BIT(2)
#define XSIGN				BIT(4)
#define YSIGN				BIT(5)
#define X_OF 				BIT(6)
#define Y_OF 				BIT (7)

//MOUSE CONFIGS
#define LEFT_CONF 			BIT(2)
#define RIGHT_CONF 			BIT(1)
#define MID_CONF 			BIT(0)
#define SCALING 			BIT(4)
#define ENABLE 				BIT(5)
#define MODE 				BIT(6)

//X MOVEMENT IS DESTINED TO THE 2ND BYTE
//Y MOVEMENT IS DESTINED TO THE 3RD BYTE

#endif // _LCOM_I8042_H
