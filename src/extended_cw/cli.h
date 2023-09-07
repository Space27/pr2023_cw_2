#pragma once

#include <unistd.h>
#include "error.h"
#include "png_structs.h"

typedef struct Configs {
	int x1Src, y1Src;
	int x2Src, y2Src;
	int xDst, yDst;
	int width;
	Pixel srcColor;
	Pixel dstColor;
	int frameType;
	double frameStyle;
	int frameInvert;
	int frameForm;
	int info;
	int cpyArea;
	int repaint;
	int circleRects;
	int drawFrame;
	int drawSqrPix;
	char *output;
} Configs;

/* выводит справку о программе */
void printHelp();

/* возвращает заполненный значениями по умолчанию экземпляр структуры Configs */
Configs createConfig();

/* по опции и указателю на структуру Configs обрабатывает входные данные и заполняет структуру,
 * возвращает статус проверки (1 - требуется обработка; -1 - ошибка чтения) */
int processOpt(int opt, Configs *config);

/* заполняет структуру пикселя из строки формата "R.G.B.<A>", возвращает 1 при некорректном вводе цвета */
int fillPixel(const char *colorString, Pixel *pix);