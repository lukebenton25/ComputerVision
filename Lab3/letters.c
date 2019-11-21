/* William Benton
 * Lab 3: This program builds on the previous lab and will reduce false
 * positives by using thinning and branchpoint and endpoint detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "letters.h"

int main()
{
  FILE *fpt;
  unsigned char *input_image;
  char header[320];
  int ROWS, COLS, BYTES;
  unsigned char *MSF;
  struct GT_s *GT;
  int lettercount = 0;

  // Open input image
  fpt = fopen("parenthood.ppm", "rb");
  if (fpt == NULL)
  {
    printf("Error opening input image\n");
    exit(0);
  }

  fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);

  if (strcmp(header, "P5") != 0 || BYTES != 255)
  {
    printf("Error: Image not a greyscale 8-bit PPM image\n");
    exit(0);
  }

  input_image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  MSF         = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

  header[0]   = fgetc(fpt);
  fread(input_image, 1, COLS*ROWS, fpt);
  fclose(fpt);

  fpt = fopen("MSF.ppm", "rb");
  if (fpt == NULL)
  {
    printf("Error opening MSF image\n");
    exit(0);
  }

  fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);

  header[0]   = fgetc(fpt);
  fread(MSF, 1, COLS*ROWS, fpt);

  fclose(fpt);

  // Read in Ground Truth
  fpt = fopen("parenthood_gt.txt", "rb");
  if (fpt == NULL)
  {
    printf("Error opening ground truth table\n");
    exit(0);
  }

  GT = (struct GT_s *)calloc(1300, sizeof(struct GT_s));

  while(!feof(fpt))
  {
    fscanf(fpt, "%c %d %d\n", &GT[lettercount].Letter, &GT[lettercount].Column, &GT[lettercount].Row);
    lettercount++;
  }

  find_letter(MSF, ROWS, COLS, GT, lettercount, input_image);

  return 1;
}
 
void threshold(unsigned char *image, unsigned char *output, int ROWS, int COLS, int thresh_val)
{
  int i;
  for (i = 0; i < ROWS*COLS; i++)
  {
    if (image[i] > thresh_val)
    {
      output[i] = 255;
    }
    else
    {
      output[i] = 0;
    }
  }

  return;
}

void create_copy(unsigned char *input_image, unsigned char *temp_letter, int COLS, int Row, int Column)
{
  int i, j, k, l;
  k = 0;
  l = 0;

  for (i = Row - 7; i <= Row + 7; i++)
  {
    k = 0;
    for (j = Column - 4; j <= Column + 4; j++)
    {
      temp_letter[l*9+k] = input_image[i*COLS+j];
      k++;
    }
    l++;
  }

  return;
}

void thinning(unsigned char *image)
{
  int  row, col;
  int  i;
  // Edge to non edge
  int  E2NE;
  int  neighbors;
  bool erase_flag;
  // Flag to keep track of edge to non edge
  bool prev;

  unsigned char *copy;

  copy = (unsigned char *)calloc(9*15, sizeof(unsigned char));

  // Threshold Image first
  for (i = 0; i < 9*15; i++)
  {
    if (image[i] < 128)
    {
      image[i] = 255;
    }
    else
    {
      image[i] = 0;
    }
  }

  erase_flag = true;
  while (erase_flag)
  {
    for (i = 0; i < 9*15; i++)
    {
      copy[i] = 0;
    }
    erase_flag = false;

    for (row = 1; row < 15; row++)
    {
      for (col = 1; col < 9; col++)
      {
        i = row*9+col;
        // Skip if pixel is not "on"
        if (!(image[i] == 0))
        {
          // Check clockwise for neighbors and E2NE transitions
          E2NE = 0;
          neighbors = 0;
          // North
          if (image[i-9] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            prev = false;
          }
          // North East
          if (image[i-9+1] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // East
          if (image[i+1] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South East
          if (image[i+1+9] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South
          if (image[i+9] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South West
          if (image[i+9-1] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // West
          if (image[i-1] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // North West
          if (image[i-1-9] == 255)
          {
            neighbors++;
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // Check the North West to North Transition for E2NE
          if ((image[i-9] == 0) && prev)
          {
            E2NE++;
          }

          // Check to see if pixel should be erased
          if (E2NE == 1)
          {
            if (neighbors >= 3 && neighbors <= 7)
            {
              if ((image[i-9] == 0) || (image[i+1] == 0) || ((image[i+9] == 0) && (image[i-1] == 0)))
              {
                copy[i] = 255;
                erase_flag = true;
              }
            }
          }
        }
      }
    }
    for (i = 0; i < 9*15; i++)
    {
      if (copy[i] == 255)
      {
        image[i] = 0;
      }
    }
  }
  return;
}
          
bool end_and_branch_detect(unsigned char *image)
{
  int  i, row, col;
  int  E2NE;
  int  endpoint, branchpoint;
  bool e_found;
  bool prev;

  endpoint    = 0;
  branchpoint = 0;

   for (row = 1; row < 15; row++)
   {
     for (col = 1; col < 9; col++)
     {
        i = row*9+col;
        // Skip if pixel is not "on"
        if (!(image[i] == 0))
        {
          // Check clockwise for neighbors and E2NE transitions
          E2NE = 0;
          // North
          if (image[i-9] == 255)
          {
            prev = true;
          }
          else
          {
            prev = false;
          }
          // North East
          if (image[i-9+1] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // East
          if (image[i+1] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South East
          if (image[i+1+9] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South
          if (image[i+9] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // South West
          if (image[i+9-1] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // West
          if (image[i-1] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // North West
          if (image[i-1-9] == 255)
          {
            prev = true;
          }
          else
          {
            if (prev)
            {
              E2NE++;
            }
            prev = false;
          }
          // Check the North West to North Transition for E2NE
          if ((image[i-9] == 0) && prev)
          {
            E2NE++;
          }
          if (E2NE == 1)
          {
            endpoint++;
          }
          else if (E2NE > 2)
          {
            branchpoint++;
          }

        }
     }
   }
   if (endpoint == 1 && branchpoint == 1)
   {
     e_found = true;
   }
   else
   {
     e_found = false;
   }

   return e_found;
}


void find_letter(unsigned char *MSF, int ROWS, int COLS, struct GT_s *GT, int Size, unsigned char *input_image)
{
  int           i, j, k;
  unsigned char *temp;
  int           thresh_val;
  FILE          *fpt1;
  FILE          *fpt2;
  int           tp, fp, tn, fn;
  bool          found;
  unsigned char *temp_letter;
  bool          e_found;

  temp        = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
  temp_letter = (unsigned char *)calloc(15*9, sizeof(unsigned char));
  for (thresh_val = 50; thresh_val < 255; thresh_val++)
  {
    threshold(MSF, temp, ROWS, COLS, thresh_val);

    // Prepare excel file
    fpt2 = fopen("ROC.csv", "w");
    fprintf(fpt2, ",TP,FP,TN,FN\n");
    fpt2 = fopen("ROC.csv", "a");

    tp = 0;
    fp = 0;
    tn = 0;
    fn = 0;

    for (i = 0; i < Size; i++)
    {
      found   = false;
      e_found = false;
      for (j = GT[i].Row - 7; j <= GT[i].Row + 7 && found == false; j++)
      {
        for (k = GT[i].Column - 4; k <= GT[i].Column + 4 && found == false; k++)
        {
          if (temp[j*COLS+k] == 255)
          {
            create_copy(input_image, temp_letter, COLS, GT[i].Row, GT[i].Column);

            thinning(temp_letter);

            e_found = end_and_branch_detect(temp_letter);

            found = true;
          }
        }
      }
      if (e_found == true && GT[i].Letter == 'e')
      {
        tp++;
      }
      else if (e_found == true && GT[i].Letter != 'e')
      {
        fp++;
      }
      else if (e_found != true && GT[i].Letter == 'e')
      {
        fn++;
      }
      else if (e_found != true && GT[i].Letter != 'e')
      {
        tn++;
      }
    }
    fprintf(fpt2, "%d, %d, %d, %d, %d\n", thresh_val, tp, fp, tn, fn);
  }

  // Added after to include examples
  create_copy(input_image, temp_letter, COLS, 25, 55);
  fpt1 = fopen("CopiedImage.ppm", "w");
  fprintf(fpt1, "P5 %d %d 255\n", 9, 15);
  fwrite(temp_letter, 9*15, 1, fpt1);
  fclose(fpt1);

  thinning(temp_letter);
  fpt1 = fopen("Thinned.ppm", "w");
  fprintf(fpt1, "P5 %d %d 255\n", 9, 15);
  fwrite(temp_letter, 9*15, 1, fpt1);
  fclose(fpt1);

  return;
}

