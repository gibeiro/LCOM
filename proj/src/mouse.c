#include "mouse.h"

static int mouse_hook_id;

_mouse* new_mouse(){

	_mouse* mouse = (_mouse*) malloc(sizeof(_mouse));

	mouse->x = SCREEN_W/2;
	mouse->y = SCREEN_H/2;
	mouse->dx = 0;
	mouse->dy = 0;
	mouse->x_signal = 0;
	mouse->y_signal = 0;
	mouse->mouse1_down = 0;
	mouse->packet_index = 0;

	unsigned char i;
	for(i = 0; i < 3; i++)
		mouse->packet[i] = 0;

	mouse->mouse_pointer = bmp_load("mouse_pointer");

	mouse->IRQ_SET_MOUSE = mouse_subscribe_int();
	mouse_write(SET_STREAM_MODE );
	mouse_write(EN_DATA_REP);

	return mouse;

}

void delete_mouse(_mouse* mouse){

	mouse_write(DIS_DATA_REP);
	mouse_write(SET_DEF );
	mouse_unsubscribe_int();
	bmp_delete(mouse->mouse_pointer);
	free(mouse);

}


int mouse_subscribe_int(void)
{
	mouse_hook_id = MOUSE_IRQ;

	int tmp = BIT(mouse_hook_id);

	if (
			sys_irqsetpolicy(MOUSE_IRQ,(IRQ_EXCLUSIVE | IRQ_REENABLE),&mouse_hook_id) != 0 ||
			sys_irqenable(&mouse_hook_id) != 0
	)
		return -1;

	else
		return tmp;

}
int mouse_unsubscribe_int(void) {

	if (
			sys_irqdisable(&mouse_hook_id) != 0 ||
			sys_irqrmpolicy(&mouse_hook_id) != 0
	)
		return -1;

	else
		return 0;
}

unsigned long mouse_read() {

	unsigned long stat, data ;

	while (1) {
		sys_inb(IN_BUF, &stat);
		if (stat & OBF) {
			sys_inb(OUT_BUF, &data);
			if ( ( stat & (Y_OF  | X_OF) ) == 0)
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int mouse_write(unsigned char command) {

	unsigned long stat;

	while (1) {
		sys_inb(IN_BUF, &stat);

		if (stat & IBF) {
			tickdelay(micros_to_ticks(DELAY_US));
			continue;
		}

		else{
			sys_outb(IN_BUF,KBCTOMOUSE);
			sys_outb(OUT_BUF, command);
			sys_inb(IN_BUF, &stat);
			if (stat & OBF)
				sys_inb(OUT_BUF, &stat);
			return 0;
		}

	}

}

void mouse_handler(_mouse* mouse){

	while(1){

		unsigned char tmp = mouse_read();

		if(
				mouse->packet_index == 0 &&
				(
						tmp == ACK ||
						tmp & BIT(3) == 0
				)
		)
			continue;

		mouse->packet[mouse->packet_index] = tmp;

		break;

	}

	if(mouse->packet_index == 2){

		mouse->mouse1_down = mouse->packet[0] & LEFT_BTN;
		mouse->x_signal = mouse->packet[0] & XSIGN;
		mouse->y_signal = ~mouse->packet[0] & YSIGN;


		if(mouse->x_signal){
			mouse->dx = ~mouse->packet[1];
			mouse->dx++;
			mouse->x -= (int) mouse->dx;
		}
		else{
			mouse->dx = mouse->packet[1];
			mouse->x += (int) mouse->dx;
		}

		if(mouse->y_signal){
			mouse->dy = mouse->packet[2];
			mouse->y -= (int) mouse->dy;
		}
		else{
			mouse->dy = ~mouse->packet[2];
			mouse->dy++;
			mouse->y += (int) mouse->dy;
		}

		if(mouse->x < 0)
			mouse->x=0;
		else if(mouse->x > SCREEN_W)
			mouse->x = SCREEN_W;

		if(mouse->y < 0)
			mouse->y=0;
		else if(mouse->y > SCREEN_H)
			mouse->y = SCREEN_H;

		mouse->packet_index = 0;

	}
	else
		mouse->packet_index++;

}

void draw_mouse(_mouse* mouse){

	vg_flip_mouse_buffer();

	bmp_draw_filtered(mouse->mouse_pointer,mouse->x,mouse->y,FILTERED_COLOR,ALIGN_RIGHT,1);

}

void mouse_reset(_mouse* mouse){

	mouse->x = SCREEN_W/2;
	mouse->y = SCREEN_H/2;
	mouse->packet_index = 0;
	mouse->packet[0] = 0;
	mouse->packet[1] = 0;
	mouse->packet[2] = 0;
	mouse->x_signal = 0;
	mouse->y_signal = 0;
	mouse->mouse1_down = 0;
	mouse->dx = 0;
	mouse->dy = 0;

}
