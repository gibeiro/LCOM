#include "AnimSprite.h"

int animate_asprite(AnimSprite *fig, char *base){

}
void destroy_asprite(AnimSprite *fig, char *base){

}
/*
AnimSprite * create_asprite(char *base, char *pic1[], ...) {
	AnimSprite *asp = malloc(sizeof(AnimSprite));
	// create a standard sprite with first pixmap
	asp->sp = create_sprite(pic1, base);
	// find out the number of variable arguments
	va_list var_args; // variable arguments
	int args;
	// find out the length of the va_args list
	va_start(va_args, pic1); // initialize va_args list
	// iterate over that list
	for(args = 0; va_arg(var_args, char**) != NULL; args++);
	va_end(var_args); // done with va_args list, for now
	// allocate array of pointers to pixmaps
	asp->map = malloc((args+1) * sizeof(char *));
	// initialize the first pixmap
	asp->map[0] = asp->sp->map;
	// continues in next transparency
	// initialize the remainder with the variable arguments
	// iterate over the var_args list again
	va_start(var_args, pic1);
	for( i = 1; i <args+1; i++ ) {
		char **tmp = va_arg(var_args, char **);
		asp->map[i] = read_xpm(tmp, &w, &h);
		if( asp->map[i] == NULL
				|| w != asp->sp->width || h != asp->sp->height) {
			// failure: realease allocated memory
			for(j = 1; j<i;j ++)
				free(asp->map[i]);
			free(asp->map);
			destroy_sprite(asp->sp);
			free(asp);
			va_end(var_args);
			return NULL;
		}
	}
	va_end(var_args);
	...

}
*/
