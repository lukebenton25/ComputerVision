/* William Benton
 * Lab 2: Optical Character Recognition
 * This program will take an image of text and look for a specific character
 * based on an image of the specific letter.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "charRecog.h"

int main()
{
  FILE          *fpt;
  unsigned char *letter;
  unsigned char *input_image;
  char          header[320];
  int           ROWS, COLS, BYTES;
  //     Template Header
  char          T_header[320];
  //     Template Rows, Columns, and Bytes
  int           T_ROWS, T_COLS, T_BYTES;
  unsigned char *MSF;

  // Open input image
  if ((fpt=fopen("parenthood.ppm","rb")) == NULL)
  {
	  printf("Unable to open image for reading.\n");
	  exit(0);
  }
  fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
  if (strcmp(header, "P5") != 0 || BYTES != 255)
  {
	  printf("Not a greyscale 8-bit PPM image\n");
	  exit(0);
  }
  input_image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  header[0] = fgetc(fpt); // Ensure the whitespace is skipped
  fread(input_image, 1, COLS*ROWS, fpt);
  fclose(fpt);

  // Open Templatate
  if ((fpt=fopen("parenthood_e_template.ppm","rb")) == NULL)
  {
	  printf("Unable to open image for reading.\n");
	  exit(0);
  }
  fscanf(fpt, "%s %d %d %d", T_header, &T_COLS, &T_ROWS, &T_BYTES);
  if (strcmp(T_header, "P5") != 0 || T_BYTES != 255)
  {
	  printf("Not a greyscale 8-bit PPM image\n");
	  exit(0);
  }
  letter = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  header[0] = fgetc(fpt); // Ensure the whitespace is skipped
  fread(letter, 1, T_COLS*T_ROWS, fpt);
  fclose(fpt);
  
  // Allocate Space for MSF image
  MSF	 = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  Calc_MSF(input_image, letter, ROWS, COLS, T_ROWS, T_COLS, MSF);

  // Create the Normalized MSF image for output
  fpt = fopen("MSF.ppm", "w");
  fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
  fwrite(MSF, COLS*ROWS, 1, fpt);
  fclose(fpt);

  // Ground Truth Processing
  fpt = fopen("parenthood_gt.txt", "rb");
  if (fpt == NULL)
  {
    printf("Error opening ground truth\n");
    exit(0);
  }

  struct coordinates_s *coordinates;
  int    CurrentCount = 0;

  // Allocate space for the ground truth data
  coordinates = (struct coordinates_s*)calloc(1300,sizeof(struct coordinates_s));
  // Loop through until the end of file and store data in array of structs
  while (!feof(fpt))
  {
    fscanf(fpt, "%c %d %d\n", &coordinates[CurrentCount].Letter, &coordinates[CurrentCount].Column, &coordinates[CurrentCount].Row);
    CurrentCount++;
  }

  find_letter(MSF, ROWS, COLS, T_ROWS, T_COLS, coordinates, CurrentCount);

  return(1);
}

void Calc_MSF(unsigned char *image, unsigned char *letter, int ROWS, int COLS, int T_ROWS, int T_COLS, unsigned char *MSF)
{
  int i, j;
  int i2, j2;
  int r, c;
  int *temp;
  int *temp_letter;
  int pixel;

  temp_letter = (int *)calloc(T_ROWS * T_COLS, sizeof(int));
  temp        = (int *)calloc(ROWS * COLS, sizeof(int));

  for (i = 0; i < T_ROWS * T_COLS; i++)
  {
    temp_letter[i] = (int)letter[i];
  }

  // Calculate the zero mean filter of the template
  ZMean(temp_letter, T_ROWS, T_COLS);

  // Loop through the image but cut off the edge pixels that can't be centered at the template.
  for (i = T_ROWS/2; i < (ROWS-(T_ROWS/2)); i++)
  {
    for (j = T_COLS/2; j < (COLS-(T_COLS/2)); j++)
    {
      pixel = 0;
      r     = 0;
	  // Convolve the zero mean filtered image with the input image
      for (i2 = (i - T_ROWS/2); i2 <= (i + T_ROWS/2); i2++)
      {
        c = 0;
        for (j2 = (j - T_COLS/2); j2 <= (j + T_COLS/2); j2++)
        {
          pixel += (int)image[i2*COLS+j2] * temp_letter[r*T_COLS+c];
          c++;
        }
        r++;
      }
      temp[i * COLS + j] = pixel;
    }
  }

  // Normalize the MSF image
  Normalize(temp, ROWS, COLS);

  // Set the "output" which is the pointer that was passed to this function
  for (i = 0; i < ROWS*COLS; i++)
  {
    MSF[i] = (unsigned char)temp[i];
  }

  free(temp_letter);
  free(temp);

  return;
}

void ZMean(int *temp_image, int ROWS, int COLS)
{
  int i;
  int average;
  int total = 0;

  for (i = 0; i < ROWS*COLS; i++)
  {
    total += temp_image[i];
  }

  average = total/(ROWS * COLS);

  for (i = 0; i < ROWS*COLS; i++)
  {
    temp_image[i] -= average;
  }
 
  return;
}

void Normalize(int *temp, int ROWS, int COLS)
{
  int i;
  int max, min;

  // Determine Max and Min
  max = 0;
  min  = 2000;
  for (i = 0; i < ROWS * COLS; i++)
  {
    if (temp[i] < min)
    {
      min = temp[i];
    }
    if (temp[i] > max)
    {
      max = temp[i];
    }
  }

  // Function for nomalization found online
  for (i = 0; i < ROWS*COLS; i++)
  {
    temp[i] = (temp[i] - min)*255/(max-min);
  }

  return;
}

void threshold(unsigned char *image, unsigned char *temp_image, int ROWS, int COLS, int thresh_val)
{
  int i;
  for (i = 0; i < ROWS*COLS; i++)
  {
    if (image[i] > thresh_val)
    {
      temp_image[i] = 255;
    }
    else
    {
      temp_image[i] = 0;
    }
  }

  return;
}

void find_letter(unsigned char *image, int ROWS, int COLS, int T_ROWS, int T_COLS, struct coordinates_s *Coordinates, int CurrentCount)
{
  int i, j, k;
  unsigned char *temp;
  int           thresh_val;
  FILE          *fpt1;
  FILE          *fpt2;
  int           tp, fp, tn, fn;
  bool          found;

  temp = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

  // Test every threshold value from 0 through 255
  for (thresh_val = 0; thresh_val < 256; thresh_val++)
  {
    threshold(image, temp, ROWS, COLS, thresh_val);

	// Prepare the .csv file for the data
    fpt2 = fopen("ROC.csv", "w");
    fprintf(fpt2, ",TP,FP,TN,FN\n");
    fpt2 = fopen("ROC.csv", "a");

    tp = 0;
    fp = 0;
    tn = 0;
    fn = 0;

	// Loop through the ground truth data
    for (i = 0; i < CurrentCount; i++)
    {
      found = false;
	  // Loop through 9x15 area around the coordinates from the ground truth data
      for (j = Coordinates[i].Row - 7; j <= Coordinates[i].Row + 7; j++)
      {
        for (k = Coordinates[i].Column - 4; k <= Coordinates[i].Column + 4; k++)
        {
          if (temp[j*COLS+k] == 255)
          {
            found = true;
          }
        }
      }
	  // Determine true positive, false positive, true negative, and false negative
      if (found == true && Coordinates[i].Letter == 'e')
      {
        tp++;
      }
      else if (found == true && Coordinates[i].Letter != 'e')
      {
        fp++;
      }
      else if (found != true && Coordinates[i].Letter == 'e')
      {
        fn++;
      }
      else if (found != true && Coordinates[i].Letter != 'e')
      {
        tn++;
      }
    }
    fprintf(fpt2, "%d,%d,%d,%d,%d\n", thresh_val, tp, fp, tn, fn);
	// Print the totals for the TP, FP, TN, and FN to the .csv file 
  }

  // Added after calculating the best threshold value.
  // Prints the best threshold image
  threshold(image, temp, ROWS, COLS, 206);

  fpt1 = fopen("out.ppm", "w");
  fprintf(fpt1, "P5 %d %d 255\n", COLS, ROWS);
  fwrite(temp, COLS*ROWS, 1, fpt1);
  fclose(fpt1);

  free(temp);

  return;
}

