#include "png_process_support.h"

int pixCmp(Pixel first, Pixel second, int CPP)
{
	if (CPP == 4)
		return first.red == second.red && first.green == second.green && first.blue == second.blue &&
			   first.alpha == second.alpha;
	else
		return first.red == second.red && first.green == second.green && first.blue == second.blue;
}

void swap(void *first, void *second, size_t size)
{
	void *buf = (void *) malloc(size);

	memcpy(buf, first, size);
	memcpy(first, second, size);
	memcpy(second, buf, size);

	free(buf);
}

double fractalKali(double x, double y, double cx, double cy, int width, int height)
{
	double m;
	x = (2 * x - width) / width;
	y = (2 * y - height) / height;

	for (int i = 0; i < 10; i++)
	{
		x = fabs(x);
		y = fabs(y);
		m = (x * x + y * y);
		x = x / m - cx / width;
		y = y / m - cy / height;
	}

	return x + y + sqrt(x * x + y * y) / 2 > 1.5 ? 1 : 0;
}

double fractalJulia(double x, double y, double cx, double cy, int width, int height)
{
	double complex z = (2 * y - height) / height * 1.5 + I * (2 * x - width) / width * 1.5;
	double complex c = cy / height + I * cx / width;
	double R = (1 + sqrt(1 + 4 * cabs(c))) / 2;

	for (int i = 0; i < 32; i++)
	{
		z = z * z + c;
		if (cabs(z) > R)
			return sin(i / 5.) + sin(i / 6.) + sin(i / 7.) > 1.5 ? 1 : 0;
	}

	return 0;
}

double fractalBio(double x, double y, double cx, double cy, int width, int height)
{
	double complex z = (2 * x - width) / width * 1.5 + I * (2 * y - height) / height * 1.5;
	double complex c = 1.07 + I * 0.0001;

	for (int i = 0; (fabs(creal(z)) < 80 || fabs(cimag(z)) < 80 || cabs(z) < 80) && i < 50; i++)
		z = cpow(z, 2 + 2 * (cx / width + cy / height)) + c;

	return fabs(creal(z)) < 50 || fabs(cimag(z)) < 50 * 50 ? 1 : 0;
}

int cmpGreat(double first, double second)
{
	return first > second ? 1 : first == second ? 0 : -1;
}

int cmpLess(double first, double second)
{
	return first > second ? -1 : first == second ? 0 : 1;
}

int isPrime(int n)
{
	for (int i = 2; i <= sqrt(n); i++)
		if (n % i == 0)
			return 0;
	return 1;
}

int XOR(int a, int b)
{
	return a ^ b;
}

int AND(int a, int b)
{
	return a & b;
}

int OR(int a, int b)
{
	return a | b;
}