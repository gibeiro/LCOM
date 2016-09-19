#include "2048.h"

_2048* start_game(){

	_2048* game = (_2048*) malloc(sizeof(_2048));

	unsigned i, j;
	for(i = 0; i < MATRIX_SIZE; i++)
		for(j = 0; j < MATRIX_SIZE; j++)
			game->matrix[i][j] = EMPTY;

	game->score = 0;
	game->over = 0;
	game->add_tile = 1;

	game->tiles[0] = bmp_load("empty_tile");
	game->tiles[1] = bmp_load("2_tile");
	game->tiles[2] = bmp_load("4_tile");
	game->tiles[3] = bmp_load("8_tile");
	game->tiles[4] = bmp_load("16_tile");
	game->tiles[5] = bmp_load("32_tile");
	game->tiles[6] = bmp_load("64_tile");
	game->tiles[7] = bmp_load("128_tile");
	game->tiles[8] = bmp_load("256_tile");
	game->tiles[9] = bmp_load("512_tile");
	game->tiles[10] = bmp_load("1024_tile");
	game->tiles[11] = bmp_load("2048_tile");

	game->numbers[0] = bmp_load("0");
	game->numbers[1] = bmp_load("1");
	game->numbers[2] = bmp_load("2");
	game->numbers[3] = bmp_load("3");
	game->numbers[4] = bmp_load("4");
	game->numbers[5] = bmp_load("5");
	game->numbers[6] = bmp_load("6");
	game->numbers[7] = bmp_load("7");
	game->numbers[8] = bmp_load("8");
	game->numbers[9] = bmp_load("9");

	game->previous_mouse_xy.x = -1;
	game->previous_mouse_xy.y = -1;
	game->previous_mouse_xy.already_slided = 0;

	game->mouse = (_mouse*) new_mouse();
	game->keyboard = (_keyboard*) new_keyboard();
	game->timer = (_timer*) new_timer();

	vg_init(0x114);
	draw_game_interface();
	add_number(game);
	draw_screen(game);
	draw_mouse(game->mouse);
	vg_flip_display();

	return game;
}

void draw_game_interface(){

	vg_fill_screen(BACKGROUND_COLOR,0);
	vg_draw_rectangle(95,195,505,605,MATRIX_COLOR,0);
	bmp_draw(bmp_load("logo"),200,10,ALIGN_LEFT,0);
	bmp_draw(bmp_load("score"),500,20,ALIGN_LEFT,0);
	vg_flip_mouse_buffer();

}

void end_game(_2048* game){

	vg_exit();

	delete_timer(game->timer);
	delete_keyboard(game->keyboard);
	delete_mouse(game->mouse);

	unsigned char i;
	for(i = 0; i < 12; i++)
		bmp_delete(game->tiles[i]);
	for(i = 0; i < 10; i++)
		bmp_delete(game->numbers[i]);

	free(game);

}

void update_game(_2048* game){

	message msg;
	int ipc_status;

	while(!game->over){

		if (driver_receive(ANY, &msg, &ipc_status) != 0)
			continue;


		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE:

				if (msg.NOTIFY_ARG & game->keyboard->IRQ_SET_KEYBOARD)
					kbd_handler(game->keyboard);

				if (msg.NOTIFY_ARG & game->mouse->IRQ_SET_MOUSE)
					mouse_handler(game->mouse);

				if (msg.NOTIFY_ARG & game->timer->IRQ_SET_TIMER){
					timer_handler(game->timer);
					draw_mouse(game->mouse);
					vg_flip_display();
				}

				set_slide_flag(game);
				slide_numbers(game);
				add_number(game);
				game_over_test(game);
				draw_screen(game);

				break;

			default:
				break;
			}

		}
	}


}

void add_number(_2048* game){
	if(game->add_tile && !game->over){

		unsigned new_number, i, j;

		if(rand() % 10 == 1)
			new_number = 4;
		else
			new_number = 2;
		while(1){
			i = rand() % MATRIX_SIZE;
			j = rand() % MATRIX_SIZE;
			if(game->matrix[i][j] == EMPTY){
				game->matrix[i][j] = new_number;
				break;
			}
		}
	}
}

void slide_numbers(_2048* game){

	unsigned i, j, k;
	int tmp[MATRIX_SIZE][MATRIX_SIZE];

	for(i=0; i < MATRIX_SIZE; i++)
		for(j = 0; j < MATRIX_SIZE; j++)
			tmp[i][j] = game->matrix[i][j];

	switch(game->slide_flag){

	case UP:
		for(i = 0; i < MATRIX_SIZE; i++)
			for(j = 0; j < MATRIX_SIZE-1;){
				if(game->matrix[j][i] == EMPTY){
					_Bool end = 1;
					for(k = j; k < MATRIX_SIZE -1; k++){
						end &=game->matrix[k+1][i] == EMPTY;
						game->matrix[k][i] = game->matrix[k+1][i];
					}
					if(end)
						break;
					game->matrix[MATRIX_SIZE -1][i] = EMPTY;
				}
				else if(game->matrix[j+1][i] == EMPTY){
					_Bool end = 1;
					for(k = j+1; k < MATRIX_SIZE -1; k++){
						end &=game->matrix[k+1][i] == EMPTY;
						game->matrix[k][i] = game->matrix[k+1][i];
					}
					if(end)
						break;
					game->matrix[MATRIX_SIZE -1][i] = EMPTY;
				}
				else if(game->matrix[j+1][i] == game->matrix[j][i]){
					game->matrix[j+1][i] += game->matrix[j][i];
					game->score += (unsigned)((log10(game->matrix[j+1][i])/log10(2) -1)*pow(2,log10(game->matrix[j+1][i])/log10(2)));
					for(k = j; k < MATRIX_SIZE -1; k++)
						game->matrix[k][i] = game->matrix[k+1][i];
					game->matrix[MATRIX_SIZE -1][i] = EMPTY;
					j++;

				}
				else
					j++;
			}
		break;

	case DOWN:
		for(i = 0; i < MATRIX_SIZE; i++)
			for(j = MATRIX_SIZE -1; j > 0;){
				if(game->matrix[j][i] == EMPTY){
					_Bool end = 1;
					for(k = j; k > 0; k--){
						end &= game->matrix[k-1][i] == EMPTY;
						game->matrix[k][i] = game->matrix[k-1][i];
					}
					if(end)
						break;
					game->matrix[0][i] = EMPTY;
				}
				else if(game->matrix[j-1][i] == EMPTY){
					_Bool end = 1;
					for(k = j -1; k > 0; k--){
						end &= game->matrix[k-1][i] == EMPTY;
						game->matrix[k][i] = game->matrix[k-1][i];
					}
					if(end)
						break;
					game->matrix[0][i] = EMPTY;
				}
				else if(game->matrix[j-1][i] == game->matrix[j][i]){
					game->matrix[j-1][i] += game->matrix[j][i];
					game->score += (unsigned)((log10(game->matrix[j-1][i])/log10(2) -1)*pow(2,log10(game->matrix[j-1][i])/log10(2)));
					for(k = j; k > 0; k--)
						game->matrix[k][i] = game->matrix[k-1][i];
					game->matrix[0][i] = EMPTY;
					j--;
				}
				else
					j--;
			}
		break;

	case LEFT:
		for(i = 0; i < MATRIX_SIZE; i++)
			for(j = 0; j < MATRIX_SIZE-1;){
				if(game->matrix[i][j] == EMPTY){
					_Bool end = 1;
					for(k = j; k < MATRIX_SIZE -1; k++){
						end &=game->matrix[i][k+1] == EMPTY;
						game->matrix[i][k] = game->matrix[i][k+1];
					}
					if(end)
						break;
					game->matrix[i][MATRIX_SIZE -1] = EMPTY;
				}
				else if(game->matrix[i][j+1] == EMPTY){
					_Bool end = 1;
					for(k = j+1; k < MATRIX_SIZE -1; k++){
						end &=game->matrix[i][k+1] == EMPTY;
						game->matrix[i][k] = game->matrix[i][k+1];
					}
					if(end)
						break;
					game->matrix[i][MATRIX_SIZE -1] = EMPTY;
				}
				else if(game->matrix[i][j+1] == game->matrix[i][j]){
					game->matrix[i][j+1] += game->matrix[i][j];
					game->score += (unsigned)((log10(game->matrix[i][j+1])/log10(2) -1)*pow(2,log10(game->matrix[i][j+1])/log10(2)));
					for(k = j; k < MATRIX_SIZE -1; k++)
						game->matrix[i][k] = game->matrix[i][k+1];
					game->matrix[i][MATRIX_SIZE -1] = EMPTY;
					j++;

				}
				else
					j++;
			}
		break;

	case RIGHT:
		for(i = 0; i < MATRIX_SIZE; i++)
			for(j = MATRIX_SIZE -1; j > 0;){
				if(game->matrix[i][j] == EMPTY){
					_Bool end = 1;
					for(k = j; k > 0; k--){
						end &= game->matrix[i][k-1] == EMPTY;
						game->matrix[i][k] = game->matrix[i][k-1];
					}
					if(end)
						break;
					game->matrix[i][0] = EMPTY;
				}
				else if(game->matrix[i][j-1] == EMPTY){
					_Bool end = 1;
					for(k = j -1; k > 0; k--){
						end &= game->matrix[i][k-1] == EMPTY;
						game->matrix[i][k] = game->matrix[i][k-1];
					}
					if(end)
						break;
					game->matrix[i][0] = EMPTY;
				}
				else if(game->matrix[i][j-1] == game->matrix[i][j]){
					game->matrix[i][j-1] += game->matrix[i][j];
					game->score += (unsigned)((log10(game->matrix[i][j-1])/log10(2) -1)*pow(2,log10(game->matrix[i][j-1])/log10(2)));
					for(k = j; k > 0; k--)
						game->matrix[i][k] = game->matrix[i][k-1];
					game->matrix[i][0] = EMPTY;
					j--;
				}
				else
					j--;
			}
		break;

	default:
		game->add_tile = 0;
		return;
	}

	game->slide_flag = NO_SLIDE;

	_Bool change = 0;
	for(i=0; i < MATRIX_SIZE; i++)
		for(j = 0; j < MATRIX_SIZE; j++)
			change |= tmp[i][j] != game->matrix[i][j];

	if(change)
		game->add_tile = 1;

}

void draw_screen(_2048* game){

	draw_score(game);
	if(game->add_tile){
		int i, j, x = 205, y = 105;
		for(i = 0; i < MATRIX_SIZE; i++){
			for(j = 0; j < MATRIX_SIZE; j++){
				if(game->matrix[i][j] == EMPTY)
					bmp_draw(game->tiles[0],200+j*100,100+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 2)
					bmp_draw(game->tiles[1],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 4)
					bmp_draw(game->tiles[2],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 8)
					bmp_draw(game->tiles[3],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 16)
					bmp_draw(game->tiles[4],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 32)
					bmp_draw(game->tiles[5],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 64)
					bmp_draw(game->tiles[6],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 128)
					bmp_draw(game->tiles[7],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 256)
					bmp_draw(game->tiles[8],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 512)
					bmp_draw(game->tiles[9],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 1024)
					bmp_draw(game->tiles[10],x+j*100,y+i*100,ALIGN_LEFT,0);
				else if(game->matrix[i][j] == 2048)
					bmp_draw(game->tiles[11],x+j*100,y+i*100,ALIGN_LEFT,0);
			}
		}
		vg_flip_mouse_buffer();
		game->add_tile= 0;
	}

}

void game_over_test(_2048* game){
	_Bool over = game->over;

	if(!over){
		unsigned char i,j;
		over = 1;
		for(i = 0; i < MATRIX_SIZE; i++)
			for(j = 0; j < MATRIX_SIZE; j++){
				over &= game->matrix[i][j] != EMPTY;
				if(game->matrix[i][j] == 2048){
					game->over = 1;
					return;
				}
			}
		if(over)
			for(i = 1; i < MATRIX_SIZE -1; i++)
				for(j = 1; j < MATRIX_SIZE -1; j++)
					over &= game->matrix[i][j] != game->matrix[i+1][j]&&
					game->matrix[i][j] != game->matrix[i-1][j]&&
					game->matrix[i][j] != game->matrix[i][j+1]&&
					game->matrix[i][j] != game->matrix[i][j-1];

		game->over = over;
	}
}

void draw_score(_2048* game){

	vg_draw_rectangle(50,500,80,600,MATRIX_COLOR,0);

	int number_of_algarisms = 0;
	int number_bmp_width = 8;

	unsigned long tmp = game->score;

	while(tmp != 0){
		tmp /= 10;
		number_of_algarisms++;
	}

	tmp = game->score;

	unsigned long total_width = number_of_algarisms * number_bmp_width;
	int score_center_x = 640;
	int score_center_y = 53;
	int rightmost_bmp_x = score_center_x + total_width / 2;

	tmp = game->score;

	int x, algarism;
	for(x = rightmost_bmp_x; tmp != 0; x -= number_bmp_width){

		algarism = tmp % 10;
		tmp /= 10;

		bmp_draw_filtered(game->numbers[algarism],x, score_center_y, FILTERED_COLOR, ALIGN_RIGHT, 0);

	}

}

void set_slide_flag(_2048* game){

	switch(game->keyboard->scancode){

	case UP_ARROW:
		game->keyboard->scancode = ERROR;
		game->slide_flag = UP;
		return;

	case DOWN_ARROW:
		game->keyboard->scancode = ERROR;
		game->slide_flag = DOWN;
		return;

	case LEFT_ARROW:
		game->keyboard->scancode = ERROR;
		game->slide_flag = LEFT;
		return;

	case RIGHT_ARROW:
		game->keyboard->scancode = ERROR;
		game->slide_flag = RIGHT;
		return;

	case ESC:
		game->slide_flag = NO_SLIDE;
		game->over = 1;
		return;

	default:
		if(game->mouse->mouse1_down){
			if(!game->previous_mouse_xy.already_slided){
				int x_slide;
				int y_slide;

				if(game->previous_mouse_xy.x == -1 || game->previous_mouse_xy.y == -1){
					game->previous_mouse_xy.x = game->mouse->x;
					game->previous_mouse_xy.y = game->mouse->y;
					return;
				}
				else{
					x_slide = game->mouse->x - game->previous_mouse_xy.x;
					y_slide = game->mouse->y - game->previous_mouse_xy.y;
				}

				if(abs(y_slide) > abs(x_slide)){

					if(y_slide > MOUSE_SLIDE_THRESHOLD){
						game->slide_flag = DOWN;
						game->previous_mouse_xy.x = -1;
						game->previous_mouse_xy.y = -1;
						game->previous_mouse_xy.already_slided = 1;
					}
					else if (y_slide < - MOUSE_SLIDE_THRESHOLD){
						game->slide_flag = UP;
						game->previous_mouse_xy.x = -1;
						game->previous_mouse_xy.y = -1;
						game->previous_mouse_xy.already_slided = 1;
					}
					else
						game->slide_flag = NO_SLIDE;

				}
				else if (fabs(y_slide) < fabs(x_slide)){

					if(x_slide > MOUSE_SLIDE_THRESHOLD){
						game->slide_flag = RIGHT;
						game->previous_mouse_xy.x = -1;
						game->previous_mouse_xy.y = -1;
						game->previous_mouse_xy.already_slided = 1;
					}
					else if (x_slide < - MOUSE_SLIDE_THRESHOLD){
						game->slide_flag = LEFT;
						game->previous_mouse_xy.x = -1;
						game->previous_mouse_xy.y = -1;
						game->previous_mouse_xy.already_slided = 1;
					}
					else
						game->slide_flag = NO_SLIDE;

				}
				else
					game->slide_flag = NO_SLIDE;
			}
		}
		else{
			game->previous_mouse_xy.x = -1;
			game->previous_mouse_xy.y = -1;
			game->previous_mouse_xy.already_slided = 0;
			game->slide_flag = NO_SLIDE;
		}
		break;
	}
}





























