#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"



int hook_id, int_counter = 0;

//added functions

//default functions
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

//LAB2
int timer_subscribe_int(void) {
	hook_id = TIMER_NOTIFICATION_ID;

	int i;

	i = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
	if (i != OK) {
		printf("sys_irqsetpolicy() failed to run \n");
		return -1;
	}

	i = sys_irqenable(&hook_id);
	if (i != OK) {
		printf("sys_irqenable() failed to run \n");
		return -2;
	}

	return (1 << TIMER_NOTIFICATION_ID);
}

//LAB2
int timer_unsubscribe_int() {

	int i;
	i = sys_irqdisable(&hook_id);
	if (i != OK) {
		printf("sys_irqdisable() !! failed to run \n");
		return 2;
	}

	i = sys_irqrmpolicy(&hook_id);
	if (i != OK) {
		printf("sys_irqrmpolicy() failed to run \n");
		return 1;
	}

	return 0;
}

//LAB2
void timer_int_handler() {
	int_counter++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {

	unsigned long st_t;

	int i;
	i = sys_outb(TIMER_CTRL,
	TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_);
	if (i != 0) {
		printf("sys_outb() failed to run \n");
		return i;
	}

	switch (timer) {
	case 0:
		i = sys_inb(TIMER_0, &st_t);
		if (i != 0) {
			printf("sys_inb() failed to run \n");
			return i;
		}
		break;
	case 1:
		i = sys_inb(TIMER_1, &st_t);
		if (i != 0) {
			printf("sys_inb() failed to run \n");
			return i;
		}
		break;
	case 2:
		i = sys_inb(TIMER_2, &st_t);
		if (i != 0) {
			printf("sys_inb() failed to run \n");
			return i;
		}
		break;
	default:
		return 1;
	}
	*st = st_t;

	return 0;
}

int timer_display_conf(unsigned char conf) {

	printf("Counter no.: ");
	if (BIT(7) ^ conf && BIT(6) ^ conf)
		printf("0 \n");
	else if (BIT(7) ^ conf && BIT(6) & conf)
		printf("1 \n");
	else if (BIT(7) & conf && BIT(6) ^ conf)
		printf("2 \n");

	printf("Type of Access: ");
	if (BIT(5) ^ conf && BIT(4) & conf)
		printf("LSB \n");
	else if (BIT(5) & conf && BIT(4) ^ conf)
		printf("MSB \n");
	else if (BIT(5) & conf && BIT(4) & conf)
		printf("LSB followed by MSB \n");

	printf("Operating mode: ");
	if (BIT(3) ^ conf && BIT(2) ^ conf && BIT(1) ^ conf)
		printf("0 \n");
	else if (BIT(3) ^ conf && BIT(2) ^ conf && BIT(1) & conf)
		printf("1 \n");
	else if (BIT(2) & conf && BIT(1) ^ conf)
		printf("2 \n");
	else if (BIT(2) & conf && BIT(1) & conf)
		printf("3 \n");
	else if (BIT(3) & conf && BIT(2) ^ conf && BIT(1) ^ conf)
		printf("4 \n");
	else if (BIT(3) & conf && BIT(2) ^ conf && BIT(1) & conf)
		printf("5 \n");

	printf("Counter mode: ");
	if (BIT(0) & conf)
		printf("BCD \n");
	else
		printf("Binary \n");

	return 0;
}

int timer_test_square(unsigned long freq) {

	unsigned long timer = 0;
	int i;
	i = timer_set_square(timer, freq);
	if (i != 0) {
		printf("timer_set_square() failed to run. \n");
		return i;
	}

	return 0;
}

//LAB2
int timer_test_int(unsigned long time) {

	int i, r, ipc_status, irq_set = (1 << TIMER_NOTIFICATION_ID);
	message msg;

	FILE * msg_ = fopen("\\usr\\log\\messages","w");

	i = timer_subscribe_int();
	if (i != irq_set) {
		printf("timer_subscribe_int() failed to run \n");
		return i;
	}

	while (time != 0) { /* You may want to use a different condition */
		timer_int_handler();
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					/* process it */
					if(int_counter%TIMER0_FREQ == 0)
					{
						//printf("One second has passed. Your life runs shorter. \n");
						//fprintf(msg_,"One second has passed. Your life runs shorter. \n");
						time--;
					}
			}
			break;
		default:
			break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	i = timer_unsubscribe_int();
	if (i != 0) {
		printf("timer_unsubscribe_int() failed to run");
		return i;
	}

	//printf("\n");
	//printf("A few seconds have passed. Your life ran shorter today. \n");

	return 0;
}

int timer_test_config(unsigned long timer) {

	unsigned char st;
	int i;
	i = timer_get_conf(timer, &st);
	if (i != 0) {
		printf("timer_get_conf() failed to run.");
		return i;
	}
	i = timer_display_conf(st);
	if (i != 0) {
		printf("timer_display_conf() failed to run.");
		return i;
	}

	return 0;
}
