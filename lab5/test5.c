#include "test5.h"

void *test_init(unsigned short mode, unsigned short delay) {

	char *phys_mem = vg_init(mode);

	if(phys_mem == NULL){
		printf("\ttest_init(): vg_init() failed to run.\n");
		return NULL;
	}

	if(timer_subscribe_int() == -1){
		printf("\ttest_init(): timer_subscribe_int() failed to run.\n");
		return NULL;
	}

	message msg;
	int ipc_status;
	char irq_set = BIT(timer_hook_id);
	float timer_counter = 0;

	while(delay > 0){

		if (driver_receive(ANY, &msg, &ipc_status) != 0) {

			printf("\ttest_init(): driver_receive() failed to run.\n");
			continue;

		}

		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & irq_set) {

					timer_counter++;

					if (fmod(timer_counter, DEFAULT_TICKS_PER_SECOND) == 0)
						delay--;

				}
			}
		}
	}

	timer_unsubscribe_int();
	vg_exit();

	printf("\tPhysical memory address: 0x%x\n", phys_mem);

	return phys_mem;

}

int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	if(vg_init(VBE_GRAPHIC_MODE) == NULL){
		vg_exit();
		printf("\ttest_square(): vg_init() failed to run.\n");
		return 1;
	}

	unsigned int i, n;

	for (i = x; i < x + size; i++) {
		for (n = y; n < y + size; n++) {
			vg_draw_pixel(i,n,color);
		}
	}

	if(timer_subscribe_int() == -1){
		vg_exit();
		timer_unsubscribe_int();
		printf("\ttest_square(): timer_subscribe_int() failed to run.\n");
		return 1;
	}

	/*
	if(timer_set_square(DEFAULT_TICKS_PER_SECOND, TIMER_FREQ) == 1){
		vg_exit();
		timer_unsubscribe_int();
		printf("\ttest_move(): timer_set_square() failed to run.\n");
		return 1;
	}
	 */
	message msg;
	int ipc_status;
	char irq_set = BIT(timer_hook_id);
	unsigned int delay = DEFAULT_DELAY;
	float timer_counter = 0;

	while(delay > 0){

		if (driver_receive(ANY, &msg, &ipc_status) != 0) {

			printf("\ttest_init(): driver_receive() failed to run.\n");
			continue;

		}

		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & irq_set) {

					timer_counter++;

					if (fmod(timer_counter, DEFAULT_TICKS_PER_SECOND) == 0)
						delay--;

				}
			}
		}
	}

	timer_unsubscribe_int();
	vg_exit();

	return 0;

}

int test_line(unsigned short xi, unsigned short yi,	unsigned short xf, unsigned short yf, unsigned long color) {

	if (kbd_subscribe_int() == 1)
	{
		printf("\ttest_line(): kbd_subscribe_int() failed to run.\n");
		return 1;
	}

	if(vg_init(VBE_GRAPHIC_MODE) == NULL){
		vg_exit();
		printf("\ttest_line(): vg_init() failed to run.\n");
		return 1;
	}

	if(xi > xf){
		unsigned tmp = xf;
		xf = xi;
		xi = tmp;
		tmp = yf;
		yf = yi;
		yi = yf;
	}

	int slope;
	int dx = xf-xi;
	int dy = yf-yi;

	if(dy < 0){
		slope = -1;
		dy = -dy;
	}
	else
		slope = 1;

	int incE = 2 * dy;
	int incNE = 2 * dy - 2 * dx;
	int d = 2 * dy - dx;
	int y = yi;
	int x;

	for (x = xi; x <= xf; x++){
		vg_draw_pixel(x, y, color);
		if (d <= 0){
			d += incE;
		}
		else{
			d += incNE;
			y += slope;
		}
	}

	while(xi <= xf){
		if(abs(slope) > 1){
			if(vg_draw_pixel(xi,yi,color) == -1){
				vg_exit();
				kbd_unsubscribe_int();
				printf("\ttest_line(): vg_draw_pixel() failed to run.\n");
				return 1;
			}
		}
		else{

		}
		xi++;
		yi = floor(yi+slope);
	}

	unsigned long key_code = 0;
	int ipc_status, irq_set = (1 << KBD_DEFAULT_HOOK_ID);
	message msg;


	while (key_code != KBD_ESC_KEY) {

		if (driver_receive(ANY, &msg, &ipc_status) != 0) {

			printf("\ttest_line(): driver_receive() failed to run.");
			continue;

		}
		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set)
					key_code = kbd_read();
				break;

			default:
				break;
			}
		}
	}

	vg_exit();

	if(kbd_unsubscribe_int() != 0)
		printf("\ttest_line: kbd_unsubscribe_int() failed to run.\n");

	return 0;

}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]){

	if(vg_init(VBE_GRAPHIC_MODE) == NULL){

		vg_exit();

		printf("\ttest_xpm(): vg_init() failed to run.\n");

		return 1;

	}

	if(timer_subscribe_int() == -1){

		printf("\ttest_xpm(): timer_subscribe_int() failed to run.\n");

		return 1;

	}
	/*
	if(timer_set_square(DEFAULT_TICKS_PER_SECOND, TIMER_FREQ) == 1){
		vg_exit();
		timer_unsubscribe_int();
		printf("\ttest_move(): timer_set_square() failed to run.\n");
		return 1;
	}
	 */
	int xpm_width, xpm_height;

	char *map = read_xpm(xpm, &xpm_width, &xpm_height);

	unsigned i, j, k = 0;

	for (i=0; i < xpm_height; i++)

		for (j=0; j< xpm_width; j++){

			vg_draw_pixel(xi+j, yi+i, map[k]);

			k++;

		}

	message msg;

	int ipc_status;

	char irq_set = BIT(timer_hook_id);

	unsigned delay = DEFAULT_DELAY;

	float timer_counter = 0;

	while(delay > 0){

		if (driver_receive(ANY, &msg, &ipc_status) != 0) {

			printf("\ttest_xpm(): driver_receive() failed to run.\n");

			continue;

		}

		if (is_ipc_notify(ipc_status)) {

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & irq_set) {

					timer_counter++;

					if (fmod(timer_counter, DEFAULT_TICKS_PER_SECOND) == 0)

						delay--;

				}

			}

		}

	}

	timer_unsubscribe_int();

	vg_exit();

	return 0;

}


int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time){

	if(timer_subscribe_int() == -1){
		timer_unsubscribe_int();
		printf("\ttest_move(): timer_subscribe_int() failed to run.\n");
		return 1;
	}
	/*
	if(timer_set_square(DEFAULT_TICKS_PER_SECOND, TIMER_FREQ) == 1){
		timer_unsubscribe_int();
		printf("\ttest_move(): timer_set_square() failed to run.\n");
		return 1;
	}
	 */
	if(vg_init(VBE_GRAPHIC_MODE) == NULL){
		vg_exit();
		timer_unsubscribe_int();
		printf("\ttest_move(): vg_init() failed to run.\n");
		return 1;
	}

	int ipc_status;
	message msg;
	char irq_set = BIT(timer_hook_id);
	unsigned delay = time;
	float timer_counter = 0;
	float divisor = DEFAULT_TICKS_PER_SECOND*(float)time / (float)delta;

	int xpm_width, xpm_height;
	char *map = read_xpm(xpm, &xpm_width, &xpm_height);

	if(hor == 0){

		unsigned yf = yi + delta;

		if(xi + xpm_width > vg_scr_w() || yf + xpm_height> vg_scr_h()){

			vg_exit();
			timer_unsubscribe_int();
			printf("\ttest_move(): parameters out of range.\n");
			return 1;

		}

		while(delay > 0 || yi < yf){

			//vg_clear_screen();

			if (driver_receive(ANY, &msg, &ipc_status) != 0) {

				printf("\ttest_xpm(): driver_receive() failed to run.\n");
				continue;

			}

			if (is_ipc_notify(ipc_status)) {

				switch (_ENDPOINT_P(msg.m_source)) {

				case HARDWARE:

					if (msg.NOTIFY_ARG & irq_set) {

						timer_counter++;

						if (fmod(timer_counter, divisor) == 0){

							delay--;

							unsigned j, k, l = 0;

							for (j=0; j < xpm_height; j++)
								for (k=0; k< xpm_width; k++){
									vg_draw_pixel(xi+k, yi+j, map[l]);
									l++;
								}

							yi++;

						}
					}
				}
			}
		}
	}
	else{

		unsigned xf = xi + delta;

		if( ((xf + xpm_width) > vg_scr_w()) || ((yi + xpm_height) > vg_scr_h())){

			vg_exit();
			timer_unsubscribe_int();
			printf("\ttest_move(): parameters out of range.\n");
			return 1;

		}

		while(delay > 0 || xi < xf){

			//vg_clear_screen();

			if (driver_receive(ANY, &msg, &ipc_status) != 0) {

				printf("\ttest_xpm(): driver_receive() failed to run.\n");
				continue;

			}

			if (is_ipc_notify(ipc_status)) {

				switch (_ENDPOINT_P(msg.m_source)) {

				case HARDWARE:

					if (msg.NOTIFY_ARG & irq_set) {

						timer_counter++;

						if (fmod(timer_counter, divisor) == 0){

							delay--;

							unsigned j, k, l = 0;

							for (j=0; j < xpm_height; j++)
								for (k=0; k< xpm_width; k++){
									vg_draw_pixel(xi+k, yi+j, map[l]);
									l++;
								}

							xi++;
						}
					}
				}
			}
		}
	}

	timer_unsubscribe_int();
	vg_exit();

	return 0;

}






