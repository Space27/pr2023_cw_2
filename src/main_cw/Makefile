CC=gcc
CFLAGS=-c -Wall -lpng -lm

all: cw_png clean

cw_png: main.o png_io.o png_structs.o png_process_support.o  png_process.o cli.o error.o
	$(CC) main.o png_io.o png_structs.o png_process_support.o png_process.o cli.o error.o -o cw_png -lpng -lm

main.o: main.c png_io.h png_structs.h cli.h error.h
	$(CC) $(CFLAGS) main.c

error.o: error.c png_structs.h
	$(CC) $(CFLAGS) error.c

png_io.o: png_io.c error.h png_structs.h
	$(CC) $(CFLAGS) png_io.c

png_structs.o: png_structs.c
	$(CC) $(CFLAGS) png_structs.c

png_process_support.o: png_process_support.c png_structs.h
	$(CC) $(CFLAGS) png_process_support.c

cli.o: cli.c error.h png_structs.h
	$(CC) $(CFLAGS) cli.c

png_process.o: png_process.c png_process_support.h png_structs.h png_io.h
	$(CC) $(CFLAGS) png_process.c

clean:
	rm *.o