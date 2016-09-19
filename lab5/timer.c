#include "timer.h"

int timer_subscribe_int(void ) {

	int hook_id_ini;
	hook_id_ini = timer_hook_id;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK)
		return -1;
	if (sys_irqenable(&timer_hook_id) != OK)
		return -1;

	return (1 << hook_id_ini);

}

int timer_unsubscribe_int() {

	if (sys_irqrmpolicy(&timer_hook_id) != OK)
		return 1;

	if(sys_irqdisable(&timer_hook_id) != OK)
		return 1;

	return 0;

}

int timer_set_square(unsigned long timer, unsigned long freq) {

	unsigned long fr;
	unsigned char command = TIMER_SEL0;
	unsigned char msb, lsb = 0x00;
	unsigned char p = TIMER_0;

	fr = TIMER_FREQ / freq;
	lsb = fr;
	msb = fr >> 8;

	if(timer ==0){
		command |= TIMER_SEL0;
		p = TIMER_0;}
	else if(timer == 1){
		command |= TIMER_SEL1;
		p = TIMER_1;}
	else if(timer == 2){
		command |= TIMER_SEL2;
		p = TIMER_2;}


	command |= TIMER_LSB_MSB;
	command |= TIMER_SQR_WAVE;
	command |= TIMER_BIN;

	if(sys_outb(TIMER_CTRL, command) != OK || sys_outb(p, lsb) != OK || sys_outb(p, msb) != OK)
		return 1;


	return 0;
}
