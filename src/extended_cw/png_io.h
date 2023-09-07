#pragma once

#include <png.h>
#include "png_structs.h"
#include "error.h"

#define CORD_IN(X1, Y1, W, H) ((X1) >= 0 && (Y1) >= 0 && (X1) < (W) && (Y1) < (H))

/* принимает имя файла и указатель на изображение, после чего сохраняет изображение в файл */
void writePngFile(const char *fileName, Image *img);

/* принимает имя файла и указатель на изображение, после чего заполняет его из файла */
void readPngFile(const char *fileName, Image *img);

/* возвращает структуру Pixel соответствующей координаты */
Pixel getPixel(int x, int y, Image *img);

/* ставит пиксель в соответствующую координату изображения */
void putPixel(int x, int y, Pixel, Image *img);

/* печатает основную информацию о PNG файле */
void printInfo(Image *img);