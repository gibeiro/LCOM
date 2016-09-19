#ifndef _2048_H_
#define _2048_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "keyboard.h"
#include "bmp.h"
#include "timer.h"
#include "mouse.h"

#define MATRIX_SIZE 4
#define EMPTY 0
#define TIMEOUT 3
#define TICKS_PER_SECOND 60
#define NO_SLIDE 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define MOUSE_SLIDE_THRESHOLD 80

typedef struct{
	int x;
	int y;
	_Bool already_slided;
}coordinates;

typedef struct{
	_mouse* mouse;
	_timer* timer;
	_keyboard* keyboard;

	coordinates previous_mouse_xy;

	unsigned short matrix[MATRIX_SIZE][MATRIX_SIZE];
	unsigned long score;

	_Bool over;
	_Bool add_tile;

	unsigned char slide_flag;
	/*
	 NO_SLIDE - 0
	 UP - 1
 	 DOWN - 2
 	 LEFT - 3
	 RIGHT - 4
	 */

	Bitmap *tiles[12];
	Bitmap *numbers[10];

}_2048;

_2048* start_game();

void end_game(_2048* game);

void update_game(_2048* game);

void add_number(_2048* game);

void set_slide_flag(_2048* game);

void slide_numbers(_2048* game);

void draw_screen(_2048* game);

void game_over_test(_2048* game);

void draw_score(_2048* game);

void draw_game_interface();

#endif /*_2048_H_*/
