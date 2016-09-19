#ifndef _BMP_H_
#define _BMP_H_

#include "graphics.h"
//
//#define EMPTY_TILE bmp_load("empty")
//#define 2_TILE bmp_load("2")
//#define 4_TILE bmp_load("4")
//#define 8_TILE bmp_load("8")
//#define 16_TILE bmp_load("16")
//#define 32_TILE bmp_load("32")
//#define 64_TILE bmp_load("64")
//#define 128_TILE bmp_load("128")
//#define 256_TILE bmp_load("256")
//#define 512_TILE bmp_load("512")
//#define 1024_TILE bmp_load("1024")
//#define 2048_TILE bmp_load("2048")

typedef enum {
	ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

typedef struct {
	unsigned short type; // specifies the file type
	unsigned int size; // specifies the size in bytes of the bitmap file
	unsigned int reserved; // reserved; must be 0
	unsigned int offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BitmapFileHeader;

typedef struct {
	unsigned int size; // specifies the number of bytes required by the struct
	int width; // specifies width in pixels
	int height; // specifies height in pixels
	unsigned short planes; // specifies the number of color planes, must be 1
	unsigned short bits; // specifies the number of bit per pixel
	unsigned int compression; // specifies the type of compression
	unsigned int imageSize; // size of image in bytes
	int xResolution; // number of pixels per meter in x axis
	int yResolution; // number of pixels per meter in y axis
	unsigned int nColors; // number of colors used by the bitmap
	unsigned int importantColors; // number of colors that are important
} BitmapInfoHeader;

typedef struct {
	BitmapInfoHeader bitmapInfoHeader;
	unsigned char* bitmapData;
} Bitmap;

Bitmap* bmp_load(const char* filename);

void bmp_draw(Bitmap* bmp, int x, int y, Alignment alignment, _Bool mouse);

void bmp_draw_filtered(Bitmap* bmp, int x, int y, unsigned short colorToFilter, Alignment alignment, _Bool mouse);

void bmp_delete(Bitmap* bmp);

const char* bmp_get_path(const char* image);


#endif /* _BMP_H_ */
