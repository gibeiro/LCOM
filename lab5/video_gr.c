#include "video_gr.h"

static char *vram_addr;
static unsigned scr_h;
static unsigned scr_w;
static unsigned bits_per_pixel;

int vg_exit(void) {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10;

	reg86.u.b.ah = 0x00;
	reg86.u.b.al = 0x03;

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
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

	reg86.u.b.ah = 0x4F;
	reg86.u.b.al = 0x02;
	reg86.u.w.bx = mode | (1 << 14) | (0 << 15);
	sys_int86(&reg86) ;

	vbe_get_mode_info(mode, &mode_info);

	scr_w = mode_info.XResolution;
	scr_h = mode_info.YResolution;
	bits_per_pixel = mode_info.BitsPerPixel;

	mr.mr_base = (phys_bytes)mode_info.PhysBasePtr;
	mr.mr_limit = mr.mr_base + scr_w*scr_h*bits_per_pixel/8;

	sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr);

	vram_addr = vm_map_phys(SELF,(void *) mr.mr_base, scr_w*scr_h*bits_per_pixel/8);

	if(vram_addr == MAP_FAILED)
		panic("vg_init(): vm_map_phys() failed to run.\n");

	return vram_addr;
}

int vg_draw_pixel(unsigned long x, unsigned long y, unsigned long color) {

	unsigned i,j;

	if(x<0 || x >= scr_w || y < 0 || y >= scr_h){
		printf("Invalid parameters.\n");
		return -1;
	}

	else {

		char* ptr = vram_addr + (y*scr_w + x)*bits_per_pixel/8;
		int c;
		for (c = 0; c < bits_per_pixel/8; c++) {
			*(ptr + bits_per_pixel/8-1-c) = (char) color;
			color = color >> 8;
		}
		return 0;
	}
}

void vg_clear_screen(){
	int x;
	int limite = scr_h*scr_w*(bits_per_pixel/8);
	for( x = 0; x < limite; x++)
		*(vram_addr + x) = 0x00;
}

unsigned vg_scr_h(){
	return scr_h;
}
unsigned vg_scr_w(){
	return scr_w;
}
