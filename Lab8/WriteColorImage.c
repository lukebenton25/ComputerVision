#include <stdio.h>
#include <math.h>
#include "RangeSegmentation.h"

void WriteColorImage(unsigned char *grayscale, int ROWS, int COLS)
{
	FILE *fpt;
	int  r, c;
	unsigned char color[3];
	
	fpt = fopen("Colored.ppm", "wb");
	fprintf(fpt, "P6 %d %d 255\n", COLS, ROWS);
	
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			color[0] = (grayscale[r*COLS+c] * 4) % 256;
			color[1] = (grayscale[r*COLS+c] * 2) % 256;
			color[2] = (grayscale[r*COLS+c] * 3) % 256;
			fwrite(color, 1, 3, fpt);
		}
	}
	fclose(fpt);
	
	return;
}