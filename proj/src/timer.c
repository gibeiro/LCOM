#include "timer.h"

static int timer_hook_id;

_timer* new_timer(){

	_timer* timer = (_timer*) malloc(sizeof(_timer));

	timer->counter = 0;

	timer->IRQ_SET_TIMER = timer_subscribe_int();

	timer_set_square(0,MOUSE_FPS);

	return timer;

}

void delete_timer(_timer* timer){

	timer_unsubscribe_int();

	free(timer);

}

int timer_subscribe_int(){

	timer_hook_id = TIMER0_IRQ;

	int tmp = BIT(timer_hook_id);

	if(
			sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK ||
			sys_irqenable(&timer_hook_id) != OK
	)
		return -1;

	else
		return tmp;

}

int timer_unsubscribe_int(){

	if(
			sys_irqdisable(&timer_hook_id) != OK ||
			sys_irqrmpolicy(&timer_hook_id) != OK
	)
		return -1;

	else
		return 0;

}

int timer_handler(_timer* timer){

	timer->counter++;

	return timer->counter;

}

void timer_reset_counter(_timer* timer){

	timer->counter = 0;

}

int timer_set_square(unsigned long timer, unsigned long freq) {

	char lsb = (char) freq;
	char msb = (char) (freq >> 8);

	switch (timer) {
	case 0:
		sys_outb(TIMER_0,
				TIMER_CTRL | TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN);
		sys_outb(TIMER_0, lsb);
		sys_outb(TIMER_0, msb);
		break;
	case 1:
		sys_outb(TIMER_1,
				TIMER_CTRL | TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN);
		sys_outb(TIMER_1, lsb);
		sys_outb(TIMER_1, msb);
		break;
	case 2:
		sys_outb(TIMER_2,
				TIMER_CTRL | TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN);
		sys_outb(TIMER_2, lsb);
		sys_outb(TIMER_2, msb);
		break;
	default:
		return 1;
	}

	return 0;
}
