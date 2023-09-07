#include "png_process.h"

void changeColor(Pixel src, Pixel dst, Image *img)
{
	for (int y = 0; y < img->height; y++)
		for (int x = 0; x < img->width; x++)
			if (pixCmp(src, getPixel(x, y, img), png_get_channels(img->pngPtr, img->infoPtr)))
				putPixel(x, y, dst, img);
}

void copyArea(int x1Src, int y1Src, int x2Src, int y2Src, int xDst, int yDst, Image *img)
{
	if (x1Src > x2Src) swap(&x1Src, &x2Src, sizeof(int));
	if (y1Src > y2Src) swap(&y1Src, &y2Src, sizeof(int));
	if (x1Src < 0) x1Src = 0;
	if (y1Src < 0) y1Src = 0;
	if (x1Src >= img->width) x1Src = img->width - 1;
	if (y1Src >= img->height) y1Src = img->height - 1;
	if (x2Src >= img->width) x2Src = img->width - 1;
	if (y2Src >= img->height) y2Src = img->height - 1;
	int areaWidth = x2Src - x1Src + xDst < img->width ? x2Src - x1Src + 1 : img->width - xDst;
	int areaHeight = y2Src - y1Src + yDst < img->height ? y2Src - y1Src + 1 : img->height - yDst;
	int CPP = png_get_channels(img->pngPtr, img->infoPtr); /* кол-во компонент на пиксель */

	png_bytepp areaBuf = (png_bytepp) malloc(areaHeight * sizeof(png_bytep));
	for (int y = y1Src; y < y1Src + areaHeight; y++)
	{
		areaBuf[y - y1Src] = (png_bytep) malloc(CPP * areaWidth * sizeof(png_byte));
		memcpy(areaBuf[y - y1Src], img->rowPointers[y] + x1Src * CPP, CPP * areaWidth * sizeof(png_byte));
	}

	for (int y = yDst > 0 ? yDst : 0; y < yDst + areaHeight; y++)
	{
		if (xDst > 0)
			memcpy(img->rowPointers[y] + xDst * CPP, areaBuf[y - yDst], CPP * areaWidth * sizeof(png_byte));
		else if (-xDst < areaWidth)
			memcpy(img->rowPointers[y], areaBuf[y - yDst] - xDst * CPP, CPP * (areaWidth + xDst) * sizeof(png_byte));
	}

	for (int i = 0; i < areaHeight; i++)
		free(areaBuf[i]);
	free(areaBuf);
}

void drawStraightLine(int x1, int y1, int x2, int y2, Pixel color, Image *img)
{
	if (x1 > x2) swap(&x1, &x2, sizeof(int));
	if (y1 > y2) swap(&y1, &y2, sizeof(int));

	if (x1 == x2 && x1 >= 0 && x1 < img->width)
		for (int y = y1 > 0 ? y1 : 0; y <= y2 && y < img->height; y++)
			putPixel(x1, y, color, img);
	else if (y1 == y2 && y1 >= 0 && y1 < img->height)
		for (int x = x1 > 0 ? x1 : 0; x <= x2 && x < img->width; x++)
			putPixel(x, y1, color, img);
}

int isDotInRects(int x, int y, int **cords, int cordLen)
{
	for (int i = 0; i < cordLen; i++)
		if (x >= cords[i][0] && x <= cords[i][2] && y >= cords[i][1] && y <= cords[i][3])
			return 1;

	return 0;
}

void circleFilledRect(Pixel rectCol, Pixel strokeCol, int width, Image *img)
{
	int cordCount = 0;
	int CPP = png_get_channels(img->pngPtr, img->infoPtr);
	int xCheck, yCheck;
	int x1, y1, x2, y2;
	int **cordArr = (int **) malloc(CORD_BUF * sizeof(int *));
	for (int i = 0; i < CORD_BUF; i++)
		cordArr[i] = (int *) malloc(4 * sizeof(int));

	for (int y = 0; y < img->height; y++)
	{
		for (int x = 0; x < img->width; x++)
		{
			if (pixCmp(getPixel(x, y, img), rectCol, CPP) && !isDotInRects(x, y, cordArr, cordCount))
			{
				x1 = x, x2 = x;
				y1 = y, y2 = y;
				xCheck = 1, yCheck = 1;
				while (xCheck || yCheck)
				{
					if (yCheck)
					{
						for (int i = x1; i <= x2 && yCheck; i++)
							if (y2 + 1 >= img->height || !pixCmp(getPixel(i, y2 + 1, img), rectCol, CPP) ||
								isDotInRects(i, y2 + i, cordArr, cordCount))
								yCheck = 0;
						if (yCheck)
							y2++;
					}
					if (xCheck)
					{
						for (int i = y1; i <= y2 && xCheck; i++)
							if (x2 + 1 >= img->width || !pixCmp(getPixel(x2 + 1, i, img), rectCol, CPP) ||
								isDotInRects(x2 + 1, i, cordArr, cordCount))
								xCheck = 0;
						if (xCheck)
							x2++;
					}
				}
				x = x2;

				if (cordCount && cordCount % CORD_BUF == 0)
				{
					int **tmp = (int **) realloc(cordArr, (cordCount + CORD_BUF) * sizeof(int *));
					if (tmp == NULL)
						crash("Memory reallocate error", img);
					cordArr = tmp;
					for (int i = cordCount; i < cordCount + CORD_BUF; i++)
						cordArr[i] = (int *) malloc(4 * sizeof(int));
				}
				cordArr[cordCount][0] = x1, cordArr[cordCount][1] = y1, cordArr[cordCount][2] = x2, cordArr[cordCount][3] = y2;
				cordCount++;
			}
		}
	}

	for (int i = 0; i < cordCount; i++)
	{
		x1 = cordArr[i][0], y1 = cordArr[i][1], x2 = cordArr[i][2], y2 = cordArr[i][3];
		for (int j = width >= 0 ? 1 : 0; abs(j) <= abs(width + (width >= 0 ? 0 : 1)); j += width > 0 ? 1 : -1)
		{
			if (x1 - j <= x2 + j)
			{
				drawStraightLine(x1 - j, y1 - j, x2 + j, y1 - j, strokeCol, img);
				drawStraightLine(x1 - j, y2 + j, x2 + j, y2 + j, strokeCol, img);
			}
			if (y1 - j <= y2 + j)
			{
				drawStraightLine(x1 - j, y1 - j, x1 - j, y2 + j, strokeCol, img);
				drawStraightLine(x2 + j, y1 - j, x2 + j, y2 + j, strokeCol, img);
			}
		}
	}

	for (int i = 0; i < ((cordCount - 1) / CORD_BUF + 1) * CORD_BUF; i++)
		free(cordArr[i]);
	free(cordArr);
}

void expandImg(Image *img, int width)
{
	int CPP = png_get_channels(img->pngPtr, img->infoPtr); /* кол-во компонент на пиксель */
	img->width += width * 2;
	img->height += width * 2;
	png_bytepp tmp = img->rowPointers;
	img->rowPointers = png_malloc(img->pngPtr, img->height * sizeof(png_bytep));

	for (int i = 0; i < img->height; i++)
	{
		img->rowPointers[i] = png_malloc(img->pngPtr, CPP * img->width * sizeof(png_byte));
		memset(img->rowPointers[i], 0, CPP * img->width * sizeof(png_byte));
	}
	for (int y = width; y < img->height - width; y++)
		memcpy(img->rowPointers[y] + width * CPP, tmp[y - width], CPP * (img->width - 2 * width) * sizeof(png_byte));

	for (int i = 0; i < img->height - 2 * width; i++)
		png_free(img->pngPtr, tmp[i]);
	png_free(img->pngPtr, tmp);
}

void drawFrameFrac(double (*frac)(double x, double y, double cx, double cy, int width, int height),
				   int (*inv)(double first, double second),
				   double style, int form, Pixel color, int width, Image *img)
{
	if (-width * 2 > img->height || -width * 2 > img->width)
		width = -img->height / 2;

	if (width > 0)
		expandImg(img, width);
	else
		width = -width;

	double fracSum;
	int fracWidth = form ? img->width : width;
	int fracHeight = form ? img->height : width;
	int balancedFracWidth;
	int balancedFracHeight = img->height - 2 * width;
	if (img->width >= fracWidth && fracWidth)
		balancedFracWidth = (img->width) / (img->width / fracWidth);
	if (img->height - 2 * width * (form ? 0 : 1) >= fracHeight && fracHeight)
		balancedFracHeight = (img->height - 2 * width * (form ? 0 : 1)) /
							 ((img->height - 2 * width * (form ? 0 : 1)) / fracHeight);

	for (int x = 0; x < img->width; x++)
	{
		for (int y = 0; y < width; y++)
		{
			fracSum = frac(x % balancedFracWidth, y, balancedFracWidth * style / 10.0, fracHeight * style / 10.0,
						   balancedFracWidth, fracHeight);
			if (inv(fracSum, 0.5) > 0)
			{
				putPixel(x, y, color, img);
				putPixel(x, img->height - y - 1, color, img);
			}
		}
	}
	for (int x = 0; x < width; x++)
	{
		for (int y = width; y < img->height - width; y++)
		{
			if (form)
				fracSum = frac(x, y, fracWidth * style / 10.0, fracHeight * style / 10.0, fracWidth, fracHeight);
			else
				fracSum = frac((y - width) % balancedFracHeight, x, balancedFracHeight * style / 10.0,
							   fracWidth * style / 10.0, balancedFracHeight, fracWidth);
			if (inv(fracSum, 0.5) > 0)
			{
				putPixel(x, y, color, img);
				putPixel(img->width - x - 1, y, color, img);
			}
		}
	}
}

void drawFramePrime(int (*bitOp)(int a, int b), int (*inv)(double first, double second),
					double style, int form, Pixel color, int width, Image *img)
{
	if (-width * 2 > img->height || -width * 2 > img->width)
		width = -img->height / 2;

	if (width > 0)
		expandImg(img, width);
	else
		width = -width;

	int patternWidth = form ? img->width : width;
	int patternHeight = form ? img->height : width;
	int balancedPatternWidth = 0;
	int balancedPatternHeight = img->height - 2 * width;
	if (img->width >= patternWidth && patternWidth)
		balancedPatternWidth = (img->width) / (img->width / patternWidth);
	if (img->height - 2 * width * (form ? 0 : 1) >= patternHeight && patternHeight)
		balancedPatternHeight = (img->height - 2 * width * (form ? 0 : 1)) /
								((img->height - 2 * width * (form ? 0 : 1)) / patternHeight);

	for (int x = 0; x < img->width; x++)
	{
		for (int y = 0; y < width; y++)
		{
			if (inv(isPrime(bitOp((int) ((x % balancedPatternWidth) * style / 10), (int) (y * style / 10))), 0.5) >
				0)
			{
				putPixel(x, y, color, img);
				if (form == 0)
					putPixel(x, img->height - width + y, color, img);
			}
			if (form && inv(isPrime(bitOp((int) ((x % balancedPatternWidth) * style / 10),
										  (int) ((img->height - y - 1) * style / 10))), 0.5) > 0)
				putPixel(x, img->height - y - 1, color, img);
		}
	}
	for (int x = 0; x < width; x++)
	{
		for (int y = width; y < img->height - width; y++)
		{
			if (inv(isPrime(bitOp((int) ((y % balancedPatternHeight) * style / 10),
								  (int) (x * style / 10))), 0.5) > 0)
			{
				putPixel(x, y, color, img);
				if (form == 0)
					putPixel(img->width - width + x, y, color, img);
			}
			if (form && inv(isPrime(bitOp((int) ((y % balancedPatternHeight) * style / 10),
										  (int) ((img->width - x - 1) * style / 10))), 0.5) > 0)
				putPixel(img->width - x - 1, y, color, img);
		}
	}
}

void drawSquare(Pixel color, int xc, int yc, int width, Image *img)
{
	for (int y = yc - width / 2 + (width % 2 == 0 ? 1 : 0); y <= yc + width / 2; y++)
	{
		drawStraightLine(xc - width / 2 + (width % 2 == 0 ? 1 : 0), y, xc + width / 2, y, color, img);
	}
}

void drawSquareNextPixels(Pixel pixColor, Pixel sqrColor, int width, Image *img)
{
	int pixCount;
	int CPP = png_get_channels(img->pngPtr, img->infoPtr);
	int **cordArr = (int **) malloc(CORD_BUF * sizeof(int *));
	for (int i = 0; i < CORD_BUF; i++)
		cordArr[i] = (int *) malloc(2 * sizeof(int));

	for (int y = 0; y < img->height; y++)
	{
		for (int x = 0; x < img->width; x++)
		{
			if (pixCmp(pixColor, getPixel(x, y, img), CPP))
			{
				if (pixCount && pixCount % CORD_BUF == 0)
				{
					int **tmp = (int **) realloc(cordArr, (pixCount + CORD_BUF) * sizeof(int *));
					if (tmp == NULL)
						crash("Memory reallocate error", img);
					cordArr = tmp;
					for (int i = pixCount; i < pixCount + CORD_BUF; i++)
						cordArr[i] = (int *) malloc(2 * sizeof(int));
				}
				cordArr[pixCount][0] = x;
				cordArr[pixCount][1] = y;
				pixCount++;
			}
		}
	}

	for (int i = 0; i < pixCount; i++)
	{
		drawSquare(sqrColor, cordArr[i][0], cordArr[i][1], width, img);
		putPixel(cordArr[i][0], cordArr[i][1], pixColor, img);
	}
	for (int i = 0; i < pixCount; i++)
	{
		putPixel(cordArr[i][0], cordArr[i][1], pixColor, img);
	}

	for (int i = 0; i < ((pixCount - 1) / CORD_BUF + 1) * CORD_BUF; i++)
		free(cordArr[i]);
	free(cordArr);
}