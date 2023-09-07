#include <getopt.h>
#include "png_io.h"
#include "error.h"
#include "png_structs.h"
#include "png_process.h"
#include "cli.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printHelp();
		return 1;
	}
	char *input = argv[1];

	Configs config = createConfig();
	const char *opts = "f:s:t:w:c:C:T:S:o:FIrlbdih";
	const struct option longOpts[] = {
			{"first",       required_argument, NULL, 'f'},
			{"second",      required_argument, NULL, 's'},
			{"third",       required_argument, NULL, 't'},
			{"width",       required_argument, NULL, 'w'},
			{"src-col",     required_argument, NULL, 'c'},
			{"dst-col",     required_argument, NULL, 'C'},
			{"style",       required_argument, NULL, 'S'},
			{"type",        required_argument, NULL, 'T'},
			{"invert",      no_argument,       NULL, 'I'},
			{"form",        no_argument,       NULL, 'F'},
			{"output",      required_argument, NULL, 'o'},
			{"help",        no_argument,       NULL, 'h'},
			{"info",        no_argument,       NULL, 'i'},
			{"duplicate",   no_argument,       NULL, 'd'},
			{"repaint",     no_argument,       NULL, 'r'},
			{"bezel",       no_argument,       NULL, 'b'},
			{"locate-rect", no_argument,       NULL, 'l'},
			{NULL, 0,                          NULL, 0}
	};
	int opt;
	int longIndex;
	int condition;

	Image *img = createImg();
	readPngFile(input, img);

	opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	while (opt != -1)
	{
		condition = processOpt(opt, &config);
		if (condition == 1)
		{
			if (config.cpyArea)
			{
				config.cpyArea = 0;
				copyArea(config.x1Src, config.y1Src, config.x2Src, config.y2Src, config.xDst, config.yDst, img);
			}
			else if (config.repaint)
			{
				config.repaint = 0;
				changeColor(config.srcColor, config.dstColor, img);
			}
			else if (config.circleRects)
			{
				config.circleRects = 0;
				circleFilledRect(config.srcColor, config.dstColor, config.width, img);
			}
			else if (config.drawFrame)
			{
				config.drawFrame = 0;
				if (config.frameType <= 2)
					drawFrameFrac(
							config.frameType == 1 ? fractalJulia : config.frameType == 2 ? fractalBio : fractalKali,
							config.frameInvert ? cmpLess : cmpGreat, config.frameStyle, config.frameForm,
							config.dstColor, config.width, img);
				else if (config.frameType <= 5)
					drawFramePrime(config.frameType == 3 ? XOR : config.frameType == 4 ? AND : OR,
								   config.frameInvert ? cmpLess : cmpGreat, config.frameStyle, config.frameForm,
								   config.dstColor, config.width, img);
				else
					panic("This type of frame does not exist");
			}
		}
		else if (condition == -1)
			break;

		opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	}

	if (config.info)
		printInfo(img);

	argc -= optind;
	argv += optind;

	if (argc && !config.output && strstr(argv[argc - 1], ".png"))
		config.output = argv[argc - 1];
	else if (!config.output)
		config.output = input;

	writePngFile(config.output, img);

	clearImage(img);

	return 0;
}