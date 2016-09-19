#ifndef _TIMER_H_
#define _TIMER_H_

#include "i8254.h"
#include <minix/syslib.h>
#include <minix/drivers.h>

#define NOTIFICATION_ID 15
#define MOUSE_FPS 60

typedef struct{

	unsigned long counter;

	int IRQ_SET_TIMER;

}_timer;

_timer* new_timer();
void delete_timer(_timer* timer);

int timer_handler(_timer* timer);
void timer_reset_counter(_timer* timer);

int timer_subscribe_int();
int timer_unsubscribe_int();

int timer_set_square(unsigned long timer, unsigned long freq);

#endif /*_TIMER_H_*/
