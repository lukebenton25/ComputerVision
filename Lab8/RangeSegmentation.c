#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "RangeSegmentation.h"

int main ()
{
	int           i, j;
	int           r, c;
	FILE          *fpt;
	cart_coord_t  *Coords;
	unsigned char *RangeImage;
	cart_coord_t  *SurfaceNormal;
	unsigned char *OutputImage;
	
	Coords = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));
	RangeImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char);
	RangeImageThresh = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	SurfaceNormal = (cart_coord_t *)calloc(ROWS*COLS, sizeof(cart_coord_t));
	OutputImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	
	fpt = fopen("chair-range.ppm", "rb");
	if (fpt == NULL)
	{
		printf("Error opening range image file.\n");
		exit(0);
	}
	fread(RangeImage, 1, ROWS*COLS, fpt);
	fclose(fpt);
	threshold(RangeImage, 137);
	
	convert2XYZ(Coords, SurfaceNormal);
	for (i = 0; i < ROWS*COLS; i++)
	{
		
	
	return (0);
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
	
	for (r = 0; r < ROWS - 2; r++)
	{
		for (c = 0; c < COLS - 2; c++)
		{
			x1 = Coords[r*COLS+(c+2)].X - Coords[r*COLS+c].X;
			x2 = Coords[(r+2)*COLS+c].X - Coords[r*COLS+c].X;
			y1 = Coords[r*COLS+(c+2)].Y - Coords[r*COLS+c].Y;
			y2 = Coords[(r+2)*COLS+c].Y - Coords[r*COLS+c].Y;
			z1 = Coords[r*COLS+(c+2)].Z - Coords[r*COLS+c].Z;
			z2 = Coords[(r+2)*COLS+c].Z - Coords[r*COLS+c].Z;
			
			SurfaceNormal[r*COLS+c].X = (y1 * z2) - (z1 * y2);
			SurfaceNormal[r*COLS+c].Y = (z1 * x2) - (z2 * x1);
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
		image[i] = 0;
    }
  }

  return;
}

void preGrow()
{
	unsigned char *image, *labels;
	int		      r, c, r2, c2;
	int		      *indices, i;
	int		      RegionSize, *RegionPixels, TotalRegions;
	cart_coord_t  avg;
	void		  RegionGrow();

	image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	/* segmentation image = labels; calloc initializes all labels to 0 */
	image = OriginalImage;
	labels = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	/* used to quickly erase small grown regions */
	indices = (int *)calloc(ROWS*COLS, sizeof(int));

	TotalRegions = 0;

	avg = var = 0.0;	/* compute average and variance in 7x7 window */
	for (r2 = -3; r2 <= 3; r2++)
		for (c2 = -3; c2 <= 3; c2++)
			avg += (double)(image[(r + r2)*COLS + (c + c2)]);
	avg /= 49.0;
	
	if (var < 1.0)	/* condition for seeding a new region is low var */
	{
		// printf("%d,%d avg=%lf var=%lf\n",r,c,avg,var);
		TotalRegions++;
		RegionGrow(image, labels, ROWS, COLS, r, c, 0, TotalRegions,
			indices, &RegionSize);
		if (RegionSize < 100)
		{	/* erase region (relabel pixels back to 0) */
			for (i = 0; i<RegionSize; i++)
				labels[indices[i]] = 0;
			TotalRegions--;
		}
	}
}

/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/

void RegionGrow(unsigned char *image,	/* image data */
	unsigned char *labels,	/* segmentation labels */
	int ROWS, int COLS,	/* size of image */
	int r, int c,		/* pixel to paint from */
	int paint_over_label,	/* image label to paint over */
	int new_label,		/* image label for painting */
	int *indices,		/* output:  indices of pixels painted */
	int *count)		/* output:  count of pixels painted */
{
	int	r2, c2;
	int	queue[MAX_QUEUE], qh, qt;
	int	average, total;	/* average and total intensity in growing region */
	int i;
	int XCenter;
	int YCenter;
	int rSum, cSum;
	float distance = 0.0;
	
	*count = 0;
	if (labels[r*COLS + c] != paint_over_label)
		return;
	labels[r*COLS + c] = new_label;
	average = total = (int)image[r*COLS + c];
	if (indices != NULL)
		indices[0] = r*COLS + c;
	queue[0] = r*COLS + c;
	qh = 1;	/* queue head */
	qt = 0;	/* queue tail */
	(*count) = 1;
	XCenter = r;
	YCenter = c;
	while (qt != qh)
	{
		if ((*count) % 50 == 0)	/* recalculate average after each 50 pixels join */
		{
			average = total / (*count);
			for (i = 0; i < *count; i++)
			{
				rSum += indices[i]/COLS;
				cSum += indices[i]%COLS;
			}
			XCenter = rSum / (*count);
			YCenter = cSum / (*count);
			rSum = 0;
			cSum = 0;
			// printf("new avg=%d\n",average);
		}
		for (r2 = -2; r2 <= 2; r2++)
			for (c2 = -2; c2 <= 2; c2++)
			{
				if (r2 == 0 && c2 == 0)
					continue;
				if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2) >= ROWS ||
					(queue[qt] % COLS + c2) < 0 || (queue[qt] % COLS + c2) >= COLS)
					continue;
				if (labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] != paint_over_label)
					continue;
				/* test criteria to join region */
				distance = sqrt(SQR((queue[qt] / COLS + r2) - XCenter) + SQR(((queue[qt] % COLS + c2) - YCenter)));
				if ((abs((int)(image[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2])
					- average) > u_difference))
					continue;

				labels[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] = new_label;
				if (indices != NULL)
					indices[*count] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
				total += image[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2];
				(*count)++;
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