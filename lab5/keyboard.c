#include "keyboard.h"

static int kbd_hook_id = KBD_DEFAULT_HOOK_ID;

int kbd_subscribe_int() {

	if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) {
		printf("\tkbd_subscribe_int(): sys_irqsetpolicy() failed to run.\n");
		return 1;
	}

	if (sys_irqenable(&kbd_hook_id) != OK) {
		printf("\tkbd_subscribe_int(): sys_irqenable() failed to run.\n");
		return 1;
	}

	return BIT(kbd_hook_id);
}

int kbd_unsubscribe_int() {

	int i;

	i = sys_irqdisable(&kbd_hook_id);
	if (i != OK) {
		printf("\tkbd_unsubscribe_int(): sys_irqdisable() failed to run.\n");
		return 2;
	}

	i = sys_irqrmpolicy(&kbd_hook_id);
	if (i != OK) {
		printf("\tkbd_unsubscribe_int(): sys_irqrmpolicy() failed to run.\n");
		return 1;
	}

	return 0;
}

unsigned long kbd_read() {

	unsigned long data, stat;

	while (1) {
		sys_inb(STAT_REG, &stat);

		if (stat & OBF) {
			sys_inb(OUT_BUF, &data);
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}
