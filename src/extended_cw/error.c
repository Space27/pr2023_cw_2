#include "error.h"

void panic(const char *errMsg)
{
	fprintf(stderr, "%s\n", errMsg);
}

void crash(const char *errMsg, Image *img)
{
	panic(errMsg);
	clearImage(img);
	exit(EXIT_FAILURE);
}