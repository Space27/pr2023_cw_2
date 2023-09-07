#pragma once

#include <math.h>
#include <string.h>
#include <complex.h>
#include "png_structs.h"

/* сравнивает пиксели по каждой компоненте, возвращает 1, если равны, и 0 в обратном случае */
int pixCmp(Pixel first, Pixel second, int CPP);

/* меняет значения двух переменных с известной длиной */
void swap(void *first, void *second, size_t size);

double fractalKali(double x, double y, double cx, double cy, int width, int height);

double fractalJulia(double x, double y, double cx, double cy, int width, int height);

double fractalBio(double x, double y, double cx, double cy, int width, int height);

/* возвращает 1, если первое число больше второго, 0, если равны, и -1, если второе больше первого */
int cmpGreat(double first, double second);

/* возвращает 1, если второе число больше первого, 0, если равны, и -1, если первое больше второго */
int cmpLess(double first, double second);

/* возвращает 1, если n - простое число; 0 - если нет */
int isPrime(int n);

/* возвращает результат "исключающего или" двух входных чисел */
int XOR(int a, int b);

/* возвращает результат "побитового и" двух входных чисел */
int AND(int a, int b);

/* возвращает результат "побитового или" двух входных чисел */
int OR(int a, int b);