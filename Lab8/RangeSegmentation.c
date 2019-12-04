#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "RangeSegmentation.h"

int main ()
{
	FILE          *fpt;
	char          header[320];
	cart_coord_t  *Coords;
	unsigned char *RangeImage;
	unsigned char *ThreshholdImage;
	cart_coord_t  *SurfaceNormal;
	unsigned char *OutputImage;
	char          *output_filename;
	// Region Growing Variables
	int           i, j, k;
	int           r, c;
	int           *indices;
	bool          valid_seed;
	unsigned char *labels;
	int           RegionSize, TotalRegions;
	
	fpt = fopen("chair-range.ppm", "rb");
	if (fpt == NULL)
	{
		printf("Error opening range image file.\n");
		exit(0);
	}
	fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
	
	Coords = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));
	RangeImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	ThreshholdImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	SurfaceNormal = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));
	OutputImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	
	fread(RangeImage, 1, ROWS*COLS, fpt);
	fclose(fpt);
	
	for (i = 0; i < ROWS*COLS; i++)
	{
		ThreshholdImage[i] = RangeImage[i];
	}
	threshold(ThreshholdImage, 140);
	
	output_filename = "threshold.ppm";
	WriteGreyScaleImage(RangeImage, output_filename);
	
	convert2XYZ(RangeImage, Coords);
	SurfaceNormalCalc(Coords, SurfaceNormal);
	
	// Begin Region Growing
	
	indices = (int *)calloc(ROWS*COLS, sizeof(int));
	TotalRegions = 30;
	
	for (i = 2; i < ROWS-2; i++)
	{
		for (j = 2; j < COLS-2; j++)
		{
			valid_seed = true;
			for (r = -2; r <= 2; r++)
			{
				for (c = -2; c <= 2; c++)
				{
					// Remove possibility of pixel becoming a seed if it has already been marked as a region
					// or if it did not pass the threshold
					if ((ThreshholdImage[(i+r)*COLS+(j+c)] == 255) || OutputImage[(i+r)*COLS+(j+c)] != 0)
					{
						valid_seed = false;
					}
				}
			}
			if (valid_seed)
			{
				TotalRegions += 30;
				RegionGrow(RangeImage, OutputImage, i, j, 0, TotalRegions, SurfaceNormal, indices, &RegionSize);
				if (RegionSize < 100)
				{
					for (k = 0; k < RegionSize; k++)
					{
						OutputImage[indices[k]] = 0;
					}
					TotalRegions -= 30;
				}
			}
		}
	}
	
	printf("Total Regions: %d\n", TotalRegions/30);
	output_filename = "output.ppm";
	WriteGreyScaleImage(OutputImage, output_filename);
	
	return (0);
}

void WriteGreyScaleImage(unsigned char *image, char *filename)
{
	FILE *fpt;
	
	fpt = fopen(filename, "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(image, COLS*ROWS, 1, fpt);
	fclose(fpt);
	
	return;
}

void SurfaceNormalCalc(cart_coord_t *Coords, cart_coord_t *SurfaceNormal)
{
	int    r;
	int    c;
	double x1;
	double x2;
	double y1;
	double y2;
	double z1;
	double z2;
	
	for (r = 0; r < ROWS - 3; r++)
	{
		for (c = 0; c < COLS - 3; c++)
		{
			x1 = Coords[r*COLS+(c+3)].X - Coords[r*COLS+c].X;
			x2 = Coords[(r+3)*COLS+c].X - Coords[r*COLS+c].X;
			y1 = Coords[r*COLS+(c+3)].Y - Coords[r*COLS+c].Y;
			y2 = Coords[(r+3)*COLS+c].Y - Coords[r*COLS+c].Y;
			z1 = Coords[r*COLS+(c+3)].Z - Coords[r*COLS+c].Z;
			z2 = Coords[(r+3)*COLS+c].Z - Coords[r*COLS+c].Z;
			
			SurfaceNormal[r*COLS+c].X = (y1 * z2) - (z1 * y2);
			SurfaceNormal[r*COLS+c].Y = ((z1 * x2) - (z2 * x1)) * -1;
			SurfaceNormal[r*COLS+c].Z = (x1 * y2) - (y1 * x2);
		}
	}
	
	return;
}


void threshold(unsigned char *image, int thresh_val)
{
  int i;
  for (i = 0; i < ROWS*COLS; i++)
  {
    if (image[i] < thresh_val)
    {
		// Don't overwrite values cause we'll need them later
		image[i] = image[i];
    }
    else
    {
		image[i] = 255;
    }
  }

  return;
}

/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/

void RegionGrow(unsigned char *image,	/* image data */
	unsigned char *labels,	/* segmentation labels */
	int r, int c,		/* pixel to paint from */
	int paint_over_label,	/* image label to paint over */
	int new_label,		/* image label for painting */
	cart_coord_t *SurfaceNormal, /* Input: Surface Noraml Image */
	int *indices,		/* output:  indices of pixels painted */
	int *count         /* output:  count of pixels painted */
	)
{
	int	r2, c2;
	int	queue[MAX_QUEUE], qh, qt;
	int curr_pos;
	cart_coord_t average, total; // Average of each direction and total in each direction
	double dot_product;
	double tempX;
	double tempY;
	double tempZ;
	double mag1;	// Magnitude of average vector
	double mag2;	// Magnitude of curr position vector
	double angle;
	
	*count = 0;
	
	if (labels[r * COLS + c] != paint_over_label)
		return;
	labels[r * COLS + c] = new_label;
	
	average.X = SurfaceNormal[r*COLS+c].X;
	average.Y = SurfaceNormal[r*COLS+c].Y;
	average.Z = SurfaceNormal[r*COLS+c].Z;
	
	if (indices != NULL)
		indices[0] = r*COLS + c;
	
	queue[0] = r*COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	
	while (qt != qh)
	{
		for (r2 = -1; r2 <= 1; r2++)
			for (c2 = -1; c2 <= 1; c2++)
			{
				if (r2 == 0 && c2 == 0)
					continue;
				if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS - 2 ||
					(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS - 2)
					continue;
				if (labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] != paint_over_label)
					continue;
				
				curr_pos = (queue[qt] / COLS + r2) * COLS + (queue[qt] % COLS + c2);
				
				/* test criteria to join region */
				tempX = (average.X * SurfaceNormal[curr_pos].X);
				tempY = (average.X * SurfaceNormal[curr_pos].Y);
				tempZ = (average.X * SurfaceNormal[curr_pos].Z);
				
				dot_product = tempX + tempY + tempZ;
				mag1 = sqrt(SQR(average.X) + SQR(average.Y) + SQR(average.Y));
				mag2 = sqrt(SQR(SurfaceNormal[curr_pos].X) + SQR(SurfaceNormal[curr_pos].Y) + SQR(SurfaceNormal[curr_pos].Z));
				
				angle = acos(dot_product / (mag1 * mag2));
				
				if (angle > ANGLE_THRESH)
					continue;

				labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] = new_label;
				if (indices != NULL)
					indices[*count] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
				
				total.X += SurfaceNormal[curr_pos].X;
				total.Y += SurfaceNormal[curr_pos].Y;
				total.Z += SurfaceNormal[curr_pos].Z;
				
				(*count)++;
				
				average.X = total.X / (*count);
				average.Y = total.Y / (*count);
				average.Z = total.Z / (*count);
				
				queue[qh] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
				qh = (qh + 1) % MAX_QUEUE;
				if (qh == qt)
				{
					printf("Max queue size exceeded\n");
					exit(0);
				}
			}
		qt = (qt + 1) % MAX_QUEUE;

	}
}