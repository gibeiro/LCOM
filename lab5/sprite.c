#include "sprite.h"

Sprite * create_sprite(char *pic[], char *base){

	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
	return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
	free(sp);
	return NULL;
	}

	return sp;


}
int animate_sprite(Sprite *fig, char *base){

}
void destroy_sprite(Sprite *fig, char *base){

	if( fig == NULL )
	return;
	free(fig->map);
	free(fig);
	fig = NULL; // hopeless: pointer is passed by value

}
void move_cursor(Sprite *fig, int xstep, int ystep, char *base){

}
