#include "keyboard.h"

static int kbd_hook_id;

int kbd_subscribe_int() {

	kbd_hook_id = KBD_IRQ;

	int tmp = BIT(kbd_hook_id);

	if (
			sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK ||
			sys_irqenable(&kbd_hook_id) != OK
	)
		return -1;

	else
		return tmp;

}

int kbd_unsubscribe_int() {

	if(
			sys_irqdisable(&kbd_hook_id) != OK ||
			sys_irqrmpolicy(&kbd_hook_id) != OK
	)
		return -1;

	else
		return 0;

}

unsigned char kbd_read() {

	unsigned long stat, data ;

	while (1) {
		sys_inb(IN_BUF, &stat);
		if (stat & OBF) {
			sys_inb(OUT_BUF, &data);
			if ( ( stat & (PAR_ERR  | TO_ERR) ) == 0)
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

_keyboard* new_keyboard(){

	_keyboard* keyboard = (_keyboard*) malloc(sizeof(_keyboard));

	keyboard->scancode = 0;
	keyboard->IRQ_SET_KEYBOARD = kbd_subscribe_int();

	return keyboard;
}

void delete_keyboard(_keyboard* keyboard){

	kbd_unsubscribe_int();
	free(keyboard);

}

void kbd_handler(_keyboard* keyboard){

	while(1){

		unsigned char tmp = kbd_read();

		if(
				tmp == ACK ||
				tmp == SPECIAL_KEY_PREFIX
		)
			continue;

		keyboard->scancode = tmp;

		break;

	}

}
