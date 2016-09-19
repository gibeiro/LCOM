#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "vbe.h"

#define BACKGROUND_COLOR vg_rgb(250,250,240)
#define MATRIX_COLOR vg_rgb(190,175,155)
#define FILTERED_COLOR vg_rgb(0,0,0)

#define SCREEN_H 600
#define SCREEN_W 800

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void * vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

unsigned short vg_rgb(unsigned char r, unsigned char g, unsigned char b);
void vg_fill_screen(unsigned short color, _Bool mouse);
void vg_draw_rectangle(unsigned long xi, unsigned long yi, long xf, long yf, unsigned short color, _Bool mouse);
int vg_draw_pixel(unsigned long x, unsigned long y, unsigned short color, _Bool mouse);
void vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned short color, _Bool mouse);

unsigned vg_get_vres();
unsigned vg_get_hres();

char* vg_get_buffer();
char* vg_get_mouse_buffer();

void vg_flip_display();
void vg_flip_mouse_buffer();


#endif /*_GRAPHICS_H_*/
