#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "i8254.h"
#include "i8042.h"

#define KBD_IRQ 1
#define UP_ARROW 0x48
#define DOWN_ARROW 0x50
#define LEFT_ARROW 0x4B
#define RIGHT_ARROW 0x4D
#define ESC 0x01
#define ERROR 0x00
#define SPECIAL_KEY_PREFIX 0xE0

typedef struct{

	int IRQ_SET_KEYBOARD;

	unsigned char scancode;

}_keyboard;

_keyboard* new_keyboard();

void delete_keyboard(_keyboard* keyboard);
void kbd_handler(_keyboard* keyboard);

int kbd_subscribe_int();
int kbd_unsubscribe_int();

unsigned char kbd_read();

#endif /*_KEYBOARD_H_*/
