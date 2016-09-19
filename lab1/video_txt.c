#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr)
{
  
  char *vptr;
  vptr = video_mem;
  
  int l, c;
  for(l = 0; l < scr_lines; l++)
	  for(c = 0; c < scr_width; c++)
	  {
		  *vptr = ch;
		  vptr++;
		  *vptr = attr;
		  vptr++;
	  }
}

void vt_blank()
{
  vt_fill(' ', 0x00);
}

int vt_print_char(char ch, char attr, int r, int c)
{
	char *vptr;
	vptr = video_mem + scr_width*r*2 + 2*c;
	*vptr = ch;
	vptr++;
	*vptr = attr;

	return 0;
}

int vt_print_string(char *str, char attr, int r, int c)
{
	int s = strlen(str);
	int i;
	for(i = 0; i < s; i++)
	{
		vt_print_char(*str, attr, r, c);

		c++;
		str++;
	}

	return 0;
}

int vt_print_int(int num, char attr, int r, int c)
{
	char *vptr;
	vptr = video_mem + r*scr_width*2 + c*2;
	*vptr = num;
	vptr++;
	*vptr = attr;

	return 0;
}

int vt_draw_frame(int width, int height, char attr, int r, int c) {

	char *vptr, *topright, *topleft, *bottomright, *bottomleft;

	topleft = video_mem + r*scr_width*2 + c*2;
	topright = topleft + width*2;
	bottomleft = topleft + height*scr_width*2;
	bottomright = bottomleft + width*2;

	for(vptr = topleft; vptr <= bottomright; vptr++){

		if(vptr==topleft) *vptr = 0xDA;
		else if(vptr==topright) *vptr = 0xBF;
		else if(vptr==bottomleft) *vptr = 0xC0;
		else if(vptr==bottomright) *vptr = 0xD9;
		else if((vptr > topleft && vptr < topright) ||
				(vptr > bottomleft && vptr < bottomright))
				*vptr = 0xC4;
		else if((vptr - topleft) % scr_width == 0)
			*vptr = 0xB3;

		vptr++;
		*vptr = attr;

		}
	}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

  int r;
  struct mem_range mr;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes)(vi_p->vram_base);
  mr.mr_limit = mr.mr_base + vi_p->vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	  panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

  if(video_mem == MAP_FAILED)
	  panic("video_txt couldn't map video memory");

  /* Save text mode resolution */

  scr_lines = vi_p->scr_lines;
  scr_width = vi_p->scr_width;

  return video_mem;
}
