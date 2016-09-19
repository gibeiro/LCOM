#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "test4.h"
#include "timer.h"
#include "i8042.h"
#include "i8254.h"

int hook_id;
static int ms_counter = 0;
static int packet_counter = 0;
long int packet[3];
long unsigned int status;

int ms_subscribe_int() {
	hook_id = MS_NOTIFICATION_ID;

	int i;

	i = sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
	if (i != OK) {
		printf("sys_irqsetpolicy() failed to run. \n");
		return -1;
	}

	i = sys_irqenable(&hook_id);
	if (i != OK) {
		printf("sys_irqenable() failed to run. \n");
		return -2;
	}

	return (1 << MS_NOTIFICATION_ID);
}

int ms_unsubscribe_int() {

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

void ms_enable_stream_mode() {
	sys_outb(STAT_REG, MS_WRITE);
	sys_outb(IN_BUF, MS_STREAMMODE);
}

void ms_disable_stream_mode() {
	sys_outb(STAT_REG, MS_WRITE);
	sys_outb(IN_BUF, MS_DISABLE_STREAMMODE);
}

unsigned long ms_get_byte() {

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

void ms_print_config()
{
	printf("B1: 0x%02X\n", packet[0]);
	printf("B2: 0x%02X\n", packet[1]);
	printf("B3: 0x%02X\n", packet[2]);

	if(packet[0] & L_BUTTON)
		printf("LEFT BUTTON is pressed\n");
	else
		printf("LEFT BUTTON is not pressed\n");

	if(packet[0] & R_BUTTON)
		printf("RIGHT BUTTON is pressed\n");
	else
		printf("RIGHT BUTTON is not pressed\n");

	if(packet[0] & M_BUTTON)
		printf("MIDDLE BUTTON is pressed\n");
	else
		printf("MIDDLE BUTTON is not pressed\n");

	if(packet[0] & SCALE)
		printf("SCALING is 2:1\n");
	else
		printf("SCALING is 1\n");

	if(packet[0] & REPORT)
		printf("DATA REPORTING is enabled\n");
	else
		printf("DATA REPORTING is disabled\n");

	if(packet[0] & MODE)
		printf("REPORT MODE is enabled\n");
	else
		printf("REPORT MODE is not disabled\n");


}
//done
int check_sync(unsigned long byte) {
	int sync = 0;
	if (BIT(3) & byte)
		sync = 1;
	return sync;
}

//done
void ms_int_handler() {
	packet[ms_counter] = ms_get_byte();

	if (ms_counter == 0 && !check_sync(packet[0]))
		return;

	if (ms_counter == 2) {
		packet_counter++;
		ms_counter = 0;
	} else
		ms_counter++;
}

//done
void ms_print_code() {
	printf("B1=0x%02X   ", packet[0]);
	printf("B2=0x%02X   ", packet[1]);
	printf("B3=0x%02X   ", packet[2]);
	if (packet[0] & LEFT_B)
		printf("LB=1 ");
	else
		printf("LB=0 ");
	if (packet[0] & MIDDLE_B)
		printf("MB=1 ");
	else
		printf("MB=0 ");
	if (packet[0] & RIGHT_B)
		printf("RB=1 ");
	else
		printf("RB=0 ");
	if (packet[0] & X_OVF)
		printf("XOV=1 ");
	else
		printf("XOV=0 ");
	if (packet[0] & Y_OVF)
		printf("YOV=1 ");
	else
		printf("YOV=0 ");
	if (packet[0] & X_SIGN && packet[1] != 0) {
		printf("X=");
		printf("%03d ", packet[1] - 255);
	} else {
		printf("X=");
		printf("%03d ", packet[1]);
	}
	if (packet[0] & Y_SIGN && packet[2] != 0) {
		printf("Y=");
		printf("%03d ", packet[2] - 255);
	} else {
		printf("Y=");
		printf("%03d ", packet[2]);
	}
	printf("\n");
}

int test_packet(unsigned short cnt) {
	int r, ipc_status, irq_set = (1 << MS_NOTIFICATION_ID);
	message msg;
	unsigned short i = cnt;

	ms_subscribe_int();
	ms_enable_stream_mode();

	while (i != 0) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					/////////////////////////////////////////////////////////////////////////////////////////

					ms_int_handler();

					if (ms_counter == 2) {
						ms_print_code();
						i--;
					}
					/////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	ms_counter = 0;
	packet_counter = 0;

	int c = ms_unsubscribe_int();
	if (c != 0) {
		printf("ms_unsubscribe_int() failed to run. \n");
		return c;
	}

	printf("TEST_PACKET! \n");
}

int test_async(unsigned short idle_time) {

	int r, ipc_status, irq1 = (1 << TIMER_NOTIFICATION_ID), irq3 = (1
			<< MS_NOTIFICATION_ID);
	message msg;
	unsigned short time = idle_time;
	int global_counter = 0;
	int c;

	ms_subscribe_int();
	ms_enable_stream_mode();
	timer_subscribe_int();

	while (time != 0) {
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
					/////////////////////////////////////////////////////////////////////////////////////////
					if (global_counter % TIMER0_FREQ == 0) {
						printf("*%d seconds to end.* \n", time);
						time--;
					}
					/////////////////////////////////////////////////////////////////////////////////////////
				}
				if (msg.NOTIFY_ARG & irq3) {
					/////////////////////////////////////////////////////////////////////////////////////////
					time = idle_time;
					ms_int_handler();

					if (ms_counter == 2) {
						ms_print_code();
					}
					/////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			default:
				break;
			}
		}
	}

	ms_counter = 0;
	packet_counter = 0;

	int i = ms_unsubscribe_int();
	if (i != 0) {
		printf("ms_unsubscribe_int() failed to run. \n");
		return i;
	}

	printf("\nTEST_ASYNC! \n");
}

//TODO IN CLASS
int test_config(void) {
	int r, ipc_status, irq_set = (1 << MS_NOTIFICATION_ID);
	message msg;
	int i = 0;

	ms_subscribe_int();
	ms_enable_stream_mode();

	while (i < 3) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
/////////////////////////////////////////////////////////////////////////////////////////
					sys_outb(STAT_REG, MS_WRITE);
					sys_outb(IN_BUF, MS_STATUSREQUEST);

					ms_int_handler();

					if (ms_counter == 2) {
						ms_print_config();
					}
/////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
		i++;
	}

	ms_counter = 0;
	packet_counter = 0;

	int c = ms_unsubscribe_int();
	if (c != 0) {
		printf("ms_unsubscribe_int() failed to run. \n");
		return c;
	}

	printf("TEST_CONFIG! \n");
}


//TODO IN CLASS
int test_gesture(short length, unsigned short tolerance) {
	int r, ipc_status, irq_set = (1 << MS_NOTIFICATION_ID);
	message msg;
	short y_move;
	unsigned short x_move;
	int c;

	ms_subscribe_int();
	ms_enable_stream_mode();

	while (abs(y_move) < length ) {
		/* Get a request message. */
		if (driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
/////////////////////////////////////////////////////////////////////////////////////////
					ms_int_handler();

					if (ms_counter == 2) {
						if (packet[0] & RIGHT_B) {
							if (x_move > tolerance) {
								x_move = 0;
								y_move = 0;
							}
							y_move = y_move + (packet[2]);
							x_move = x_move + (packet[1]);
							ms_print_code();
							printf("\nY_MOVE: %02d\nX_MOVE: %02d\n", y_move, x_move);
						} else {
							x_move = 0;
							y_move = 0;
						}
					}
/////////////////////////////////////////////////////////////////////////////////////////
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
	}

	ms_counter = 0;
	packet_counter = 0;

	int i = ms_unsubscribe_int();
	if (i != 0) {
		printf("ms_unsubscribe_int() failed to run. \n");
		return i;
	}

	printf("\nTEST_GESTURE! \n");
}

