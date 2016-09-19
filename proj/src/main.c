#include "2048.h"
#include "mouse.h"

int main(){

	sef_startup();

	srand(time(NULL));

	_2048* game = (_2048*) start_game();

	update_game(game);

	end_game(game);

	return 0;

/*
	vg_init(0x114);

	vg_fill_screen(MATRIX_COLOR,0);

	vg_flip_mouse_buffer();

	bmp_draw(bmp_load("score"),0,0,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("0"),5, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("1"),20, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("2"),35, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("3"),50, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("4"),65, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("5"),80, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	bmp_draw_filtered(bmp_load("6"),95, 60,FILTERED_COLOR,ALIGN_RIGHT,1);
	//bmp_draw(bmp_load("score"),100, 200,FILTERED_COLOR,ALIGN_RIGHT,1);


	vg_flip_display();

	sleep(5);

	vg_exit();
	return 0;
*/
	/*
	//MOUSE TEST



	_mouse* mouse = (_mouse*) new_mouse();
	_timer * timer = (_timer*) new_timer();

	vg_init(0x114);

	vg_fill_screen(BACKGROUND_COLOR,0);



	message msg;
	int ipc_status;
	int time = 5;

	while(time > 0){

		if (driver_receive(ANY, &msg, &ipc_status) != 0)
			continue;


		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & timer->IRQ_SET_TIMER){

					unsigned long tmp = timer_handler(timer);

					if(tmp % 1){
						draw_mouse(mouse);
						vg_flip_display();
					}

					if(tmp % 60 == 0)
						time--;
				}

				if (msg.NOTIFY_ARG & mouse->IRQ_SET_MOUSE){
					mouse_handler(mouse);
					timer_reset_counter(timer);
					time = 5;
				}

				break;

			default:

				break;
			}
		}

	}


	vg_exit();
	delete_timer(timer);
	delete_mouse(mouse);

	printf("DONE!\n");
	return 0;

	 */

	/*
	//KEYBOARD TEST
		_keyboard* keyboard = (_keyboard*) new_keyboard();
		int IRQ_SET_TIMER = timer_subscribe_int();

		message msg;
		int ipc_status;
		int time = 5;

		while(time > 0){

			if (driver_receive(ANY, &msg, &ipc_status) != 0)
				continue;


			if (is_ipc_notify(ipc_status)) {
				switch (_ENDPOINT_P(msg.m_source)) {

				case HARDWARE:

					if (msg.NOTIFY_ARG & IRQ_SET_TIMER)
						if(timer_int_handler() % 60 == 0){
							time--;
							printf("\t%d segundos para terminar;\n",time);
						}

					if (msg.NOTIFY_ARG & keyboard->IRQ_SET_KEYBOARD){
						kbd_handler(keyboard);
						timer_reset_counter();
						time = 5;
					}

					break;

				default:

					break;
				}
			}

		}

		timer_unsubscribe_int();
		delete_keyboard(keyboard);

		printf("DONE!\n");
	 */

	/*
	//MOUSE AND KEYBOARD TEST
	_mouse* mouse = (_mouse*) new_mouse();
	_keyboard* keyboard = (_keyboard*) new_keyboard();
	_timer* timer = (_timer*) new_timer();

	message msg;
	int ipc_status;
	int time = 5;

	while(time > 0){

		if (driver_receive(ANY, &msg, &ipc_status) != 0)
			continue;


		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & timer->IRQ_SET_TIMER)
					if(timer_handler(timer) % 60 == 0){
						time--;
						printf("\t%d segundos para terminar;\n",time);
					}

				if (msg.NOTIFY_ARG & keyboard->IRQ_SET_KEYBOARD){
					kbd_handler(keyboard);
					timer_reset_counter(timer);
					time = 5;
				}

				if (msg.NOTIFY_ARG & mouse->IRQ_SET_MOUSE){
					mouse_handler(mouse);
					timer_reset_counter(timer);
					time = 5;
				}

				break;

			default:

				break;
			}
		}

	}

	delete_timer(timer);
	delete_keyboard(keyboard);
	delete_mouse(mouse);

	printf("DONE!\n");
	 */

}
