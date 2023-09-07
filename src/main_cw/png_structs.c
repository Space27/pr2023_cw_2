#include "png_structs.h"

Image *createImg()
{
	Image *img = (Image *) malloc(sizeof(Image));

	img->width = 0;
	img->height = 0;
	img->passesNumber = 0;
	img->colorType = 0;
	img->bitDepth = 0;
	img->pngPtr = NULL;
	img->infoPtr = NULL;
	img->rowPointers = NULL;

	return img;
}

void clearImage(Image *img)
{
	png_destroy_read_struct(&img->pngPtr, &img->infoPtr, NULL);
	if (img->rowPointers)
	{
		for (int y = 0; y < img->height; y++)
			png_free(img->pngPtr, img->rowPointers[y]);
		png_free(img->pngPtr, img->rowPointers);
	}

	free(img);
}