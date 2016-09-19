#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "i8042.h"
#include "bmp.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

typedef struct{

	int x, y;
	unsigned  char dx, dy;
	_Bool x_signal, y_signal;
	_Bool mouse1_down;

	int packet_index;
	unsigned char packet[3];

	int IRQ_SET_MOUSE;

	Bitmap* mouse_pointer;

}_mouse;

_mouse* new_mouse();
void delete_mouse(_mouse* mouse);

int mouse_subscribe_int();
int mouse_unsubscribe_int();

unsigned long mouse_read();
int mouse_write(unsigned char cmd);

void mouse_handler(_mouse* mouse);
void mouse_reset(_mouse* mouse);
void draw_mouse(_mouse* mouse);

#endif /*_MOUSE_H_*/
