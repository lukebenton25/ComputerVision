/*
** This program reads bridge.ppm, a 512 x 512 PPM image.
** It smooths it using a standard 3x3 mean filter.
** The program also demonstrates how to time a piece of code.
**
** To compile, must link using -lrt  (man clock_gettime() function).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "time-smooth.h"

void main()
{
  printf("Standard 7x7 Filter Running: \n");
  standard_filter();
  printf("======================================\n");
  printf("Separable Filter Running: \n");
  separable_filter();
  printf("======================================\n");
  printf("Sliding Window Running: \n");
  slide_window();
  printf("======================================\n");
}

void standard_filter()
{
  FILE		*fpt;
  unsigned char	*image;
  unsigned char	*smoothed;
  char		header[320];
  int		ROWS,COLS,BYTES;
  int		r,c,r2,c2,sum;
  struct timespec	tp1,tp2;

	/* read image */
  if ((fpt=fopen("bridge.ppm","rb")) == NULL)
  {
    printf("Unable to open bridge.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

	/* allocate memory for smoothed version of image */
  smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp1);
  printf("Start Time: %ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

	/* smooth image, skipping the border points */
  for (r=3; r<ROWS-3; r++)
  {
    for (c=3; c<COLS-3; c++)
    {
      sum=0;
      for (r2=-3; r2<=3; r2++)
      {
        for (c2=-3; c2<=3; c2++)
        {
          sum+=image[(r+r2)*COLS+(c+c2)];
        }
      }
      smoothed[r*COLS+c]=sum/49;
    }
  }

	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp2);
  printf("End Time: %ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
  printf("Difference in ns: %ld\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
  fpt=fopen("smoothed.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed,sizeof(char),COLS*ROWS, fpt);
  fclose(fpt);
 
  return;
}

void separable_filter()
{
  FILE		*fpt;
  unsigned char	*image;
  float	        *smoothed;
  unsigned char *smoothed2;
  char		header[320];
  int		ROWS,COLS,BYTES;
  int		r,c,r2,c2;
  float         sum;
  struct timespec	tp1,tp2;

	/* read image */
  if ((fpt=fopen("bridge.ppm","rb")) == NULL)
  {
    printf("Unable to open bridge.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

	/* allocate memory for smoothed version of image */
  smoothed=(float *)calloc(ROWS*COLS,sizeof(float));
  smoothed2=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp1);
  printf("Start Time: %ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

	/* smooth image, skipping the border points */

  for (r = 0; r < ROWS; r++)
  {
    for (c = 3; c < COLS-3; c++)
    {
      sum = 0;
      for (c2 = -3; c2 <= 3; c2++)
      {
        sum += image[r*COLS+(c+c2)];
      }
      smoothed[r*COLS+c] = sum;
    }
  }
  
  for (r = 3; r < ROWS-3; r++)
  {
    for (c = 3; c < COLS-3; c++)
    {
      sum = 0;
      for (r2 = -3; r2 <= 3; r2++)
      {
        sum += smoothed[(r+r2)*COLS+c];
      }
      smoothed2[r*COLS+c] = sum/49;
    }
  }


	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp2);
  printf("End Time: %ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
  printf("Difference in ns: %ld\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
  fpt=fopen("separable_smoothed.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed2,COLS*ROWS,1,fpt);
  fclose(fpt);

  free(image);
  free(smoothed);
  free(smoothed2);

  return;
}

void slide_window()
{
  FILE		*fpt;
  unsigned char	*image;
  float	        *smoothed;
  unsigned char *smoothed2;
  char		header[320];
  int		ROWS,COLS,BYTES;
  int		r,c,r2,c2;
  float         sum;
  struct timespec	tp1,tp2;

	/* read image */
  if ((fpt=fopen("bridge.ppm","rb")) == NULL)
  {
    printf("Unable to open bridge.ppm for reading\n");
    exit(0);
  }
  fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  header[0]=fgetc(fpt);	/* read white-space character that separates header */
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

	/* allocate memory for smoothed version of image */
  smoothed=(float *)calloc(ROWS*COLS,sizeof(float));
  smoothed2=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp1);
  printf("Start Time: %ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

	/* smooth image, skipping the border points */

  for (r = 0; r < ROWS; r++)
  {
    for (c = 3; c < COLS-3; c++)
    {
      if (c == 3)
      {
        sum = 0;
        for (c2 = -3; c2 <= 3; c2++)
        {
          sum += image[r * COLS + (c + c2)];
        }
      }
      else
      {
        sum -= image[r * COLS + (c - 4)];
        sum += image[r * COLS + (c + 3)];
      }

      smoothed[r * COLS + c] = sum;
    }
  }
  
  for (c = 3; c < COLS-3; c++)
  {
    for (r = 3; r < ROWS-3; r++)
    {
      if (r == 3)
      {
        sum = 0;
        for (r2 = -3; r2 <= 3; r2++)
        {
          sum += smoothed[(r+r2)*COLS+c];
        }
      }
      else
      {
        sum -= smoothed[(r-4) * COLS + c];
        sum += smoothed[(r+3) * COLS + c];
      }
      smoothed2[r*COLS+c] = sum/49;
    }
  }


	/* query timer */
  clock_gettime(CLOCK_REALTIME,&tp2);
  printf("End Time: %ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
  printf("Difference in ns: %ld\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
  fpt=fopen("sliding_smoothed.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(smoothed2,COLS*ROWS,1,fpt);
  fclose(fpt);

  free(image);
  free(smoothed);
  free(smoothed2);

  return;
}
