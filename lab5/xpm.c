#include "xpm.h"

char *read_xpm(char* map[], int *w, int *h){
	int width, height, colors;
		char sym[256];
		int  col;
		int i, j;
		char *pix, *pixtmp, *tmp, *line;
		char symbol;

		/* read width, height, colors */
		if (sscanf(map[0],"%d %d %d", &width, &height, &colors) != 3) {
			printf("\tread_xpm(): incorrect width, height, colors.\n");
			return NULL;
		}
	#ifdef DEBUG
		printf("%d %d %d\n", width, height, colors);
	#endif
		if (width > vg_scr_w() || height > vg_scr_h() || colors > 256) {
			printf("\tread_xpm(): incorrect width, height, colors.\n");
			return NULL;
		}

		*w = width;
		*h = height;

		for (i=0; i<256; i++)
			sym[i] = 0;

		/* read symbols <-> colors */
		for (i=0; i<colors; i++) {
			if (sscanf(map[i+1], "%c %d", &symbol, &col) != 2) {
				printf("read_xpm: incorrect symbol, color at line %d\n", i+1);
				return NULL;
			}
	#ifdef DEBUG
			printf("%c %d\n", symbol, col);
	#endif
			if (col > 256) {
				printf("read_xpm: incorrect color at line %d\n", i+1);
				return NULL;
			}
			sym[col] = symbol;
		}

		/* allocate pixmap memory */
		pix = pixtmp = malloc(width*height);

		/* parse each pixmap symbol line */
		for (i=0; i<height; i++) {
			line = map[colors+1+i];
	#ifdef DEBUG
			printf("\nparsing %s\n", line);
	#endif
			for (j=0; j<width; j++) {
				tmp = memchr(sym, line[j], 256);  /* find color of each symbol */
				if (tmp == NULL) {
					printf("read_xpm: incorrect symbol at line %d, col %d: %c\n", colors+i+1, j, line[j]);
					return NULL;
				}
				*pixtmp++ = tmp - sym; /* pointer arithmetic! back to books :-) */
	#ifdef DEBUG
				printf("%c:%d ", line[j], tmp-sym);
	#endif
			}
		}

		return pix;
}
