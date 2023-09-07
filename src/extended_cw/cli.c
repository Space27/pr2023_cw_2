#include "cli.h"

void printHelp()
{
	puts("\nНАИМЕНОВАНИЕ");
	puts("\tcw_png - утилита для обработки PNG изображений");
	puts("СИНТАКСИС");
	puts("\tcw_png [имя исходного файла] [ключи-конфигураторы] [ключ функции 1] [...] [опционально: имя выходного файла]");
	puts("ОПИСАНИЕ");
	puts("\tДанная утилита позволяет обработать один PNG файл несколькими функциями поочередно. То есть одним вызовом можно применить несколько функций, притом для каждой функции можно отдельно изменить настройки.");
	puts("ФУНКЦИИ");
	puts("\t-d/--duplicate - копирование заданной области по координатам левого верхнего (-f), правого нижнего (-s) углов области-источника, а также левого верхнего (-t) угла области-получателя.");
	puts("\t-r/--repaint - замена всех пикселей изображения заданного цвета (-c) на другой заданный цвет (-С).");
	puts("\t-H/--high-pix - рисование вокруг всех пикселей изображения заданного цвета (-c) залитого квадрата заданного цвета (-С).");
	puts("\t-l/--locate-rect - поиск всех залитых прямоугольников заданного цвета (-c) и их обводка линиями заданного цвета (-C) определенной ширины (-w).");
	puts("\t-b/--bezel - рисование рамки ширины (-w) (при маленьких значениях рамка может пропасть) и цвета (-C) по краям изображения. Вид рамки можно менять несколькими настройками:");
	puts("\t   -Тип используемого фрактала для получения узора (-T): значение от 0 до 5. По умолчанию 0.");
	puts("\t\t-0-2 - фрактальные узоры. 1 задаёт множество Жюлиа (хорошие узоры при стиле около 5). 2 задаёт биоморфов.");
	puts("\t\t-2-5 - узоры простых чисел. 3 - узор микросхемы, 4 - подобие фрактальных треугольников, 5 - нечто среднее.");
	puts("\t   -Инвертирование узора (-I).");
	puts("\t   -Параметр стиля узора (-S): Допустимы вещественные числа. Данный параметр качественно меняет узор. Каждый узор меняется по-своему.");
	puts("\t   -Форма заливки узора (-F). Без флага рамка строится из блоков-узоров. С флагом рамка менее однородная, поскольку получена обрезкой одного узора.");
	puts("КОНФИГУРАЦИИ");
	puts("\t-f/--first <x.y> - координаты первой точки, требуемой функцией. По умолчанию 0.0.");
	puts("\t-s/--second <x.y> - координаты второй точки, требуемой функцией. По умолчанию 0.0.");
	puts("\t-t/--third <x.y> - координаты третьей точки, требуемой функцией. По умолчанию 0.0.");
	puts("\t-w/--width <value> - задаёт толщину отрезка/рамки. По умолчанию 1.");
	puts("\t-c/--src-col <R.G.B/R.G.B.A> - задаёт искомый цвет. По умолчанию 0.0.0.255.");
	puts("\t-c/--dst-col <R.G.B/R.G.B.A> - задаёт конечный цвет. По умолчанию 0.0.0.255.");
	puts("\t-S/--style <value> - рекомендуется значения от 0 до 10, допустимы вещественные. Хорошие узоры получаются в диапазоне 7 - 9. По умолчанию 8.");
	puts("\t-T/--type <0-5> - меняет тип узора.");
	puts("\t-I/--invert - инвертирует узор.");
	puts("\t-F/--form - рамка образуется обрезкой одного фрактального узора, без флага рамка строится из нескольких блоков-узоров.");
	puts("ПРОЧИЕ");
	puts("\t-o/--output <имя выходного файла> - переназначает выходной файл. По умолчанию изменяется исходный файл.");
	puts("\t-i/--info - выводит информацию об выходном PNG файле.");
	puts("\t-h/--help - выводит данную справку.");
}

Configs createConfig()
{
	Configs config = {
			.x1Src = 0, .y1Src = 0,
			.x2Src = 0, .y2Src = 0,
			.xDst = 0, .yDst = 0,
			.width = 1,
			.srcColor = {0, 0, 0, 255},
			.dstColor = {0, 0, 0, 255},
			.frameType = 0,
			.frameStyle = 8.,
			.frameInvert = 0,
			.frameForm = 0,
			.info = 0,
			.cpyArea = 0,
			.repaint = 0,
			.circleRects = 0,
			.drawFrame = 0,
			.drawSqrPix = 0,
			.output = NULL,
	};

	return config;
}

int processOpt(int opt, Configs *config)
{
	switch (opt)
	{
		case 'f':
			if (sscanf(optarg, "%d.%d", &config->x1Src, &config->y1Src) != 2)
			{
				panic("Incorrect coordinate input format");
				return -1;
			}
			break;
		case 's':
			if (sscanf(optarg, "%d.%d", &config->x2Src, &config->y2Src) != 2)
			{
				panic("Incorrect coordinate input format");
				return -1;
			}
			break;
		case 't':
			if (sscanf(optarg, "%d.%d", &config->xDst, &config->yDst) != 2)
			{
				panic("Incorrect coordinate input format");
				return -1;
			}
			break;
		case 'c':
			if (fillPixel(optarg, &config->srcColor))
			{
				panic("Incorrect color input format");
				return -1;
			}
			break;
		case 'C':
			if (fillPixel(optarg, &config->dstColor))
			{
				panic("Incorrect color input format");
				return -1;
			}
			break;
		case 'w':
			if (sscanf(optarg, "%d", &config->width) != 1)
			{
				panic("Incorrect width input format");
				return -1;
			}
			break;
		case 'S':
			if (sscanf(optarg, "%lf", &config->frameStyle) != 1)
			{
				panic("Incorrect frame style input format");
				return -1;
			}
			break;
		case 'T':
			if (sscanf(optarg, "%d", &config->frameType) != 1)
			{
				panic("Incorrect frame type input format");
				return -1;
			}
			break;
		case 'I':
			config->frameInvert = 1;
			break;
		case 'F':
			config->frameForm = 1;
			break;
		case 'o':
			config->output = optarg;
			break;
		case 'r':
			config->repaint = 1;
			return 1;
		case 'd':
			config->cpyArea = 1;
			return 1;
		case 'l':
			config->circleRects = 1;
			return 1;
		case 'b':
			config->drawFrame = 1;
			return 1;
		case 'H':
			config->drawSqrPix = 1;
			return 1;
		case 'i':
			config->info = 1;
			break;
		case 'h':
			printHelp();
			break;
		default:
			break;
	}

	return 0;
}

int fillPixel(const char *colorString, Pixel *pix)
{
	int colorCom[4] = {-1, -1, -1, -1};
	pix->alpha = 255;

	if (sscanf(colorString, "%d.%d.%d.%d", colorCom, colorCom + 1, colorCom + 2, colorCom + 3) < 3)
		return 1;

	for (int i = 0; i < 3; i++)
		if (colorCom[i] < 0 || colorCom[i] > 255)
			return 1;

	pix->red = colorCom[0];
	pix->green = colorCom[1];
	pix->blue = colorCom[2];
	if (colorCom[3] >= 0 && colorCom[3] <= 255) pix->alpha = colorCom[3];

	return 0;
}