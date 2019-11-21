#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "MotionTracking.h"

#define SQR(x) ((x)*(x))

int main()
{
	float **Table;
	float *Xbuff1;
	float *Xbuff2;
	float *Ybuff1;
	float *Ybuff2;
	float *Zbuff1;
	float *Zbuff2;
	float *Pbuff1;
	float *Pbuff2;
	float *Rbuff1;
	float *Rbuff2;
	float *Wbuff1;
	float *Wbuff2;
	FILE  *fpt;
	int   i;
	char  c;
	int   data_size;
	float accXAvg  = 0.0;
	float accYAvg  = 0.0;
	float accZAvg  = 0.0;
	float pitchAvg = 0.0;
	float rollAvg  = 0.0;
	float yawAvg   = 0.0;
	
	// Read beginning contour points
	if ((fpt=fopen("acc_gyro.txt", "rb")) == NULL)
	{
		printf("Unable to open data file\n");
		exit(0);
	}

	i = 0;
	
	for (c = getc(fpt); c != EOF; c = getc(fpt))
	{
		if (c == '\n')
			i++;
	}
	
	data_size = i;
	
	rewind(fpt);
	
	Table  = (float **)malloc(7*sizeof(float *));
	for (i = 0; i < 7; i++)
	{
		Table[i] = (float *)malloc(data_size*sizeof(float));
	}
	
	i = 0;
	
	while(!feof(fpt) && !(i == data_size))
	{
		if (i == 0)
		{
			fscanf(fpt, "time	accX	accY	accZ	pitch	roll	yaw							\n");
		}
		else
		{
			fscanf(fpt, "%f %f %f %f %f %f %f \n", &Table[TIME][i-1], &Table[ACCX][i-1], &Table[ACCY][i-1], &Table[ACCZ][i-1], &Table[PITCH][i-1], &Table[ROLL][i-1], &Table[YAW][i-1]);
			accXAvg  += Table[ACCX][i-1];
			accYAvg  += Table[ACCY][i-1];
			accZAvg  += Table[ACCZ][i-1];
			pitchAvg += Table[PITCH][i-1];
			rollAvg  += Table[ROLL][i-1];
			yawAvg   += Table[YAW][i-1];
		}
		i++;
	}
	
	accXAvg  = (accXAvg / data_size)/4;
	accYAvg  = (accYAvg / data_size)/4;
	accZAvg  = (accZAvg / data_size)*1.2;
	pitchAvg = pitchAvg / data_size;
	rollAvg  = rollAvg / data_size;
	yawAvg   = yawAvg / data_size;
	
	printf("ACCX Average: %f\n", accXAvg);
	printf("ACCY Average: %f\n", accYAvg);
	printf("ACCZ Average: %f\n", accZAvg);
	printf("Pitch Avergae: %f\n", pitchAvg);
	printf("Roll Average: %f\n", rollAvg);
	printf("Yaw Average: %f\n", yawAvg);
	
	
	/* for (i = 0; i < data_size; i++)
	{
		
	} */
	
	return(0);
}
	