#include "vbe.h"

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	struct reg86u reg86;
	mmap_t address;

	lm_init();
	lm_alloc(sizeof(vbe_mode_info_t), &address);

	reg86.u.w.ax = 0x4F01;
		reg86.u.w.es = PB2BASE(address.phys);
	reg86.u.w.di = PB2OFF(address.phys);
	reg86.u.w.cx = 1 << 14 | mode;
	reg86.u.b.intno = 0x10;

	sys_int86(&reg86);

	*vmi_p = *((vbe_mode_info_t*) address.virtual);

	lm_free(&address);
	return 0;
}
