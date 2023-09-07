#pragma once

#include <png.h>
#include <stdlib.h>

typedef struct Image {
	int width, height;
	png_byte colorType, bitDepth;

	png_structp pngPtr;
	png_infop infoPtr;
	int passesNumber;
	png_bytepp rowPointers;
} Image;

typedef struct Pixel {
	png_byte red, green, blue, alpha;
} Pixel;

/* возвращает указатель на структуру Image с обнуленными переменными */
Image *createImg();

/* безопасно очищает поля структуры */
void clearImage(Image *img);