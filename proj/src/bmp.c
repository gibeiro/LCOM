#include "bmp.h"

Bitmap* bmp_load(const char* filename){

	char num[256];
	sprintf(num, "/home/proj/res/%s.bmp", filename);

	char* str = (char*) malloc(256);
	strcpy(str, num);

	Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

	FILE *filePtr;
	filePtr = fopen(str, "rb");
	if (filePtr == NULL)
		return NULL;

	BitmapFileHeader bitmapFileHeader;
	fread(&bitmapFileHeader, 2, 1, filePtr);

	if (bitmapFileHeader.type != 0x4D42) {
		fclose(filePtr);
		return NULL;
	}

	int rd;
	do {
		if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
			break;
	} while (0);

	if (rd = !1) {
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}

	BitmapInfoHeader bitmapInfoHeader;
	fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

	fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

	unsigned char* bitmapImage = (unsigned char*) malloc(
			bitmapInfoHeader.imageSize);

	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return NULL;
	}

	// close file and return bitmap image data
	fclose(filePtr);

	bmp->bitmapData = bitmapImage;
	bmp->bitmapInfoHeader = bitmapInfoHeader;

	return bmp;

}

void bmp_draw(Bitmap* bmp, int x, int y, Alignment alignment, _Bool mouse){

	char* bufferStartPos;
	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	char* buffer;

	if(mouse)
		buffer = vg_get_mouse_buffer();
	else
		buffer = vg_get_buffer();

	int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - i;

		if (pos < 0 || pos >= vg_get_vres())
			continue;

		bufferStartPos = buffer + x * 2
				+ pos * vg_get_hres() * 2;

		memcpy(bufferStartPos, bmp->bitmapData + i * width * 2, width * 2);
	}

	/*
	if (bmp == NULL)
		return;

	int width = bmp->bitmapInfoHeader.width;
	int drawWidth = width;
	int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	if (x + width < 0 || x > vg_get_hres() || y + height < 0
			|| y > vg_get_vres())
		return;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > vg_get_hres())
			drawWidth = vg_get_hres();
	} else if (x + drawWidth >= vg_get_hres()) {
		drawWidth = vg_get_hres() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= vg_get_vres())
			continue;

		if(mouse)
			bufferStartPos = vg_get_mouse_buffer();
		else
			bufferStartPos = vg_get_buffer();

		bufferStartPos += x * 2 + pos * vg_get_hres() * 2;

		imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;

		memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
	}
	 */
}

void bmp_delete(Bitmap* bmp){

	if (bmp == NULL)
		return;

	free(bmp->bitmapData);
	free(bmp);

}

void bmp_draw_filtered(Bitmap* bmp, int x, int y, unsigned short colorToFilter, Alignment alignment, _Bool mouse){

	char* buffer;

	if(mouse)
		buffer = vg_get_mouse_buffer();
	else
		buffer = vg_get_buffer();

	char* bufferStartPos;
	unsigned int width = bmp->bitmapInfoHeader.width;
	unsigned int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	int i, j;
	for (i = 0; i < height; i++) {
		int pos = y + height - i;

		if (pos < 0 || pos >= vg_get_vres())
			continue;

		bufferStartPos = buffer + x * 2
				+ (y + height - i) * vg_get_hres() * 2;

		for (j = 0; j < width * 2; j++, bufferStartPos++) {
			if (x + j < 0 || x * 2 + j >= vg_get_hres() * 2)
				continue;

			int pos = j + i * width * 2;
			unsigned short tmp1 = bmp->bitmapData[pos];
			unsigned short tmp2 = bmp->bitmapData[pos + 1];
			unsigned short temp = (tmp1 | (tmp2 << 8));

			// printf("Pixel color: %x\n", temp);
			if (temp != colorToFilter) {
	 *bufferStartPos = bmp->bitmapData[j + i * width * 2];
				j++;
				bufferStartPos++;
	 *bufferStartPos = bmp->bitmapData[j + i * width * 2];
			} else {
				j++;
				bufferStartPos++;
			}
		}
	}

}
