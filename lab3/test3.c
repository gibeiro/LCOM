#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "timer.h"
#include "i8254.h"
#include "test3.h"
#include "i8042.h"

extern int asm_code;

int hook_id, global_counter = 0, double_byte = 0;
unsigned long code = 0;

int kbd_subscribe_int() {
	hook_id = KBD_NOTIFICATION_ID;

	int i;

	i = sys_irqsetpolicy(TIMER1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
	if (i != OK) {
		printf("sys_irqsetpolicy() failed to run. \n");
		return -1;
	}

	i = sys_irqenable(&hook_id);
	if (i != OK) {
		printf("sys_irqenable() failed to run. \n");
		return -2;
	}

	return (1 << KBD_NOTIFICATION_ID);
}

int kbd_unsubscribe_int() {

	int i;

	i = sys_irqdisable(&hook_id);
	if (i != OK) {
		printf("sys_irqdisable() failed to run \n");
		return 2;
	}

	i = sys_irqrmpolicy(&hook_id);
	if (i != OK) {
		printf("sys_irqrmpolicy() failed to run \n");
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

void kbd_write(unsigned long cmd) {

	unsigned long stat;

	while (1) {
		sys_inb(STAT_REG, &stat);
		if ((stat & IBF) == 0) {
			sys_outb(KBC_CMD_REG, cmd);
			return;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

void kbd_int_handler() {

	code = kbd_read();
}

void kbd_print_code() {

	if (code == DOUBLE_BYTE) {
		double_byte = 1;
		return;
	}
	if ((code & BIT(7))) {
		printf("Breakcode: 0x");
		if (double_byte)
			printf("E0");
		printf("%02X \n", code);
	} else {
		printf("Makecode: 0x");
		if (double_byte)
			printf("E0");
		printf("%02X \n", code);
	}

	double_byte = 0;
}

int kbd_test_scan_ASM()
{
	int r, ipc_status, irq_set = (1 << KBD_NOTIFICATION_ID);
	message msg;
	while (code != ESC_BREAKCODE) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					printf("OK! \n");
					kbd_int_handler_ASM();

					code = asm_code;
					kbd_print_code();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}
	printf("OK! \n");

	return 0;
}

int kbd_test_scan_C() {
	int r, ipc_status, irq_set = (1 << KBD_NOTIFICATION_ID);
	message msg;

	while (code != ESC_BREAKCODE) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					kbd_int_handler();
					kbd_print_code();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	return 0;
}

int kbd_test_scan(unsigned short ass) {

	int i;

	i = kbd_subscribe_int();
	if(i != (1 << KBD_NOTIFICATION_ID))
	{
		printf("kbd_subscribe() failed to run. \n");
		return i;
	}

	if (ass == 0)
		kbd_test_scan_C();
	 else kbd_test_scan_ASM();

	i = kbd_unsubscribe_int();
	if(i != 0)
	{
		printf("kbd_unsubscribe() failed to run. \n");
		return i;
	}

}

int kbd_test_leds(unsigned short n, unsigned short *leds) {

	unsigned int i;
	unsigned long int data;
	unsigned long cmd = 0;
	int led_stat[3] = { 0, 0, 0 };

	sys_outb(OUT_BUF, cmd);

	for (i = 0; i < n; i++) {
		sys_outb(OUT_BUF, KBD_SETLEDS);
		sys_inb(OUT_BUF, &data);

		timer_test_int(1);

		if (data == 0xFE) { //RESEND
			i--;
			printf("\n ERRO: Reenviar informação!");
			continue;
		} else if (data == 0xFC) { //ERROR AND RESET
			printf("\n ERRO: Recomeçar!");
			i = 0;
			continue;
		} else {

			switch (leds[i]) {
			case 0: //NUM LOCK
				if (led_stat[0] == 0) {
					printf("\nLED_0 ON");
					led_stat[0] = BIT(0);
				} else {
					led_stat[0] = 0;
					printf("\nLED_0 OFF");
				}
				break;
			case 1: //CAPS LOCK
				if (led_stat[1] == 0) {
					led_stat[1] = BIT(1);
					printf("\nLED_1 ON");
				} else {
					led_stat[1] = 0;
					printf("\nLED_1 OFF");
				}
				break;
			case 2: //SCROLL LOCK
				if (led_stat[2] == 0) {
					led_stat[2] = BIT(2);
					printf("\nLED_2 ON");
				} else {
					led_stat[2] = 0;
					printf("\nLED_2 OFF");
				}
				break;
			default:
				printf(
						"\nERRO: Argumento inválido! %d",
						i);
				break;
			}


		}

	}

	cmd = led_stat[0] | led_stat[1] | led_stat[2];
	sys_outb(OUT_BUF, cmd);
	sys_inb(OUT_BUF, &data);
}

int kbd_test_timed_scan(unsigned short n) {

	unsigned short time = n;
	int i, r, ipc_status, irq1 = (1 << TIMER_NOTIFICATION_ID), irq2 = (1
			<< KBD_NOTIFICATION_ID);
	message msg;

	i = kbd_subscribe_int();
	i = timer_subscribe_int();

	while (code != ESC_BREAKCODE && time != 0) {
		global_counter++;
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq1) {

					if (global_counter % TIMER0_FREQ == 0)
						time--;
				}
				if (msg.NOTIFY_ARG & irq2) {
					time = n;
					kbd_int_handler();
					kbd_print_code();
				}
				break;
			default:
				break;
			}
		}
	}


	i = timer_unsubscribe_int();
	{
		printf("timer_unsubscribe_int() failed to run. \n");
		if (i != 0)
			return i;
		else
			return 0;
	}

	i = kbd_unsubscribe_int();
	{
		printf("kbd_unsubscribe_int() failed to run. \n");
		if (i != 0)
			return i;
		else
			return 0;
	}
}
