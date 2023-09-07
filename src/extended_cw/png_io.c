#include "png_io.h"

void readPngFile(const char *fileName, Image *img)
{
	char header[8];

	FILE *fp = fopen(fileName, "rb");
	if (!fp)
		crash("Error during opening file for reading", img);

	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep) header, 0, 8))
		crash("File is not a PNG", img);

	img->pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!img->pngPtr)
		crash("Creating read struct failed", img);

	img->infoPtr = png_create_info_struct(img->pngPtr);
	if (!img->infoPtr)
		crash("Creating info struct failed", img);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during init_io", img);

	png_init_io(img->pngPtr, fp);
	png_set_sig_bytes(img->pngPtr, 8);

	png_read_info(img->pngPtr, img->infoPtr);

	img->width = (int) png_get_image_width(img->pngPtr, img->infoPtr);
	img->height = (int) png_get_image_height(img->pngPtr, img->infoPtr);
	img->colorType = png_get_color_type(img->pngPtr, img->infoPtr);
	img->bitDepth = png_get_bit_depth(img->pngPtr, img->infoPtr);

	img->passesNumber = png_set_interlace_handling(img->pngPtr);
	png_read_update_info(img->pngPtr, img->infoPtr);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during reading img", img);

	img->rowPointers = png_malloc(img->pngPtr, img->height * sizeof(png_bytep));
	for (int i = 0; i < img->height; i++)
		img->rowPointers[i] = png_malloc(img->pngPtr, png_get_rowbytes(img->pngPtr, img->infoPtr));

	png_read_image(img->pngPtr, img->rowPointers);

	fclose(fp);
}

void writePngFile(const char *fileName, Image *img)
{
	FILE *fp = fopen(fileName, "wb");
	if (!fp)
		crash("Error during opening file for writing", img);

	img->pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!img->pngPtr)
		crash("Creating write struct failed", img);

	img->infoPtr = png_create_info_struct(img->pngPtr);
	if (!img->infoPtr)
		crash("Creating info struct failed", img);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during init_io", img);

	png_init_io(img->pngPtr, fp);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during header writing", img);

	png_set_IHDR(img->pngPtr, img->infoPtr, img->width, img->height, img->bitDepth,
				 img->colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(img->pngPtr, img->infoPtr);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during writing bytes", img);

	png_write_image(img->pngPtr, img->rowPointers);

	if (setjmp(png_jmpbuf(img->pngPtr)))
		crash("An error occurred during end of write", img);

	png_write_end(img->pngPtr, NULL);

	fclose(fp);
}

Pixel getPixel(int x, int y, Image *img)
{
	if (!CORD_IN(x, y, img->width, img->height))
		crash("Image overstepping error", img);

	int colorChannels = png_get_channels(img->pngPtr, img->infoPtr);

	png_bytep ptr = img->rowPointers[y] + x * colorChannels;

	Pixel pix = {ptr[0], ptr[1], ptr[2], colorChannels > 3 ? ptr[3] : 255};

	return pix;
}

void putPixel(int x, int y, Pixel pix, Image *img)
{
	if (!CORD_IN(x, y, img->width, img->height))
		crash("Image overstepping error", img);

	int colorChannels = png_get_channels(img->pngPtr, img->infoPtr);

	png_bytep ptr = img->rowPointers[y] + x * colorChannels;

	ptr[0] = pix.red;
	ptr[1] = pix.green;
	ptr[2] = pix.blue;
	if (colorChannels > 3)
		ptr[3] = pix.alpha;
}

void printInfo(Image *img)
{
	printf("WIDTH: %d\n", img->width);
	printf("HEIGHT: %d\n", img->height);
	printf("COLOR TYPE: %d\n", img->colorType);
	printf("BIT DEPTH: %d\n", img->bitDepth);
	printf("COLOR CHANNELS: %d\n", png_get_channels(img->pngPtr, img->infoPtr));
}