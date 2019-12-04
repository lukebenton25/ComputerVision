CC = gcc
CFLAGS = -g -Wall

lab8: RangeSegmentation.c RangeSegmentation.h convert2XYZ.c WriteColorImage.c
	$(CC) $(CFLAGS) -o lab8 RangeSegmentation.c convert2XYZ.c WriteColorImage.c -lm

clean:
	rm lab8
