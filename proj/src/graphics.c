#include "graphics.h"

static char* vram_addr;
static char* buffer;
static char* mouse_buffer;

static unsigned vmem_size;
static unsigned scr_h;
static unsigned scr_w;
static unsigned bits_per_pixel;

int vg_exit(void) {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10;

	reg86.u.b.ah = 0x00;
	reg86.u.b.al = 0x03;

	if( sys_int86(&reg86) != OK ) {
		return 1;
	} else
		return 0;
}


void *vg_init(unsigned short mode){

	int r;
	struct mem_range mr;
	struct reg86u reg86;
	vbe_mode_info_t mode_info;

	reg86.u.b.intno = 0x10;
	reg86.u.b.al = 0x02;
	reg86.u.b.ah = 0x4F;
	reg86.u.w.bx = mode | (1 << 14) | (0 << 15);

	sys_int86(&reg86) ;

	vbe_get_mode_info(mode, &mode_info);

	bits_per_pixel = mode_info.BitsPerPixel;
	scr_h = mode_info.YResolution;
	scr_w = mode_info.XResolution;

	vmem_size = scr_h * scr_w * bits_per_pixel / 8;

	mr.mr_base = (phys_bytes) mode_info.PhysBasePtr;
	mr.mr_limit = mr.mr_base + vmem_size;

	if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK)
		panic("\tvg_init(): sys_privctl() failed to run.\n", reg86);

	vram_addr = vm_map_phys(SELF,(void *) mr.mr_base, vmem_size);

	if(vram_addr == MAP_FAILED)
		panic("\tvg_init(): couldn't map video memory.",reg86);

	buffer = (char*) malloc(vmem_size);
	mouse_buffer = (char*) malloc(vmem_size);

	return vram_addr;


}

int vg_draw_pixel(unsigned long x, unsigned long y, unsigned short color, _Bool mouse) {

	unsigned i,j;

	if(x<0 || x >= scr_w || y < 0 || y >= scr_h)
		return -1;

	unsigned char lsB = (unsigned char) ( (color >> 8) & 0x00FF );
	unsigned char msB = (unsigned char) ( color & 0x00FF );

	int index = ( x + y*scr_w ) * bits_per_pixel / 8;

	if(mouse){
		mouse_buffer[index] = msB;
		mouse_buffer[index+1] = lsB;
	}
	else{
		buffer[index] = msB;
		buffer[index+1] = lsB;
	}

}

unsigned vg_get_vres(){
	return scr_h;
}

unsigned vg_get_hres(){
	return scr_w;
}

char* vg_get_buffer(){
	return buffer;
}

char* vg_get_mouse_buffer(){
	return mouse_buffer;
}

unsigned short vg_rgb(unsigned char r, unsigned char g, unsigned char b){
	if (r < 0 || 255 < r || g < 0 || 255 < g || b < 0 || b > 255)
		return -1;

	int red, green, blue;
	red = r * 31 / 255;
	green = g * 63 / 255;
	blue = b * 31 / 255;

	return (red << 11) | (green << 5) | blue;

}

void vg_fill_screen(unsigned short color, _Bool mouse){

	char *ptr;

	unsigned char lsB = (unsigned char) ( (color >> 8) & 0x00FF );
	unsigned char msB = (unsigned char) ( color & 0x00FF );

	int i, end = scr_w * scr_h * bits_per_pixel / 8;

	if(mouse)
		ptr = mouse_buffer;
	else
		ptr = buffer;


	for (i = 0; i < end - 1; i++){
		*(ptr+i) = msB;
		i++;
		*(ptr+i) = lsB;
	}

}

void vg_draw_rectangle(unsigned long xi, unsigned long yi, long xf, long yf, unsigned short color, _Bool mouse){

	int i, j;

	if (yi > yf){
		long tmp = yf;
		yf = yi;
		yi = tmp;
	}


	if (xi > xf){
		long tmp = xf;
		xf = xi;
		xi = tmp;
	}

	for(i = yi; i < yf; i++)
		for(j = xi; j < xf; j++)
			vg_draw_pixel(i,j,color,mouse);

}

void vg_flip_display(){
	memcpy(vram_addr, mouse_buffer, vmem_size);
}

void vg_flip_mouse_buffer(){
	memcpy(mouse_buffer, buffer, vmem_size);
}

void vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf, unsigned long yf, unsigned short color, _Bool mouse){

		if(xi < 0)
			xi = 0;
		if(yi < 0)
			yi = 0;
		if(xf < 0)
			xf = 0;
		if(yf < 0)
			yf = 0;

		if(xi > scr_w)
			xi = scr_w;
		if(xf > scr_w)
			xf = scr_w;
		if(yi > scr_w)
			yi = scr_h;
		if(yf > scr_w)
			yf = scr_h;

	int dx = abs(xf-xi);
	int dy = abs(yf-yi);

	int slope_x = xi<xf ? 1 : -1;
	int slope_y = yi<yf ? 1 : -1;

	int erro = (dx>dy ? dx : -dy)/2;
	int erro2;

	int x,y;
	for(x = xi, y = yi; x != xf || y != yf; ){

		vg_draw_pixel(x,y,color,mouse);

		erro2 = erro;

		if (erro2 > -dx) {
			x += slope_x;
			erro -= dy;
		}

		if (erro2 < dy) {
			y += slope_y;
			erro += dx;
		}
	}
}

