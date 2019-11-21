#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SQR(x) ((x)*(x))

int main()
{
	float *time;
	float *accX;
	float *accY;
	float *accZ;
	float *pitch;
	float *roll;
	float *yaw;
	FILE  *fpt;
	int   i;
	
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
	
	rewind(fpt);
	
	time  = (float *)malloc(i-1, sizeof(float));
	accX  = (float *)malloc(i-1, sizeof(float));
	accY  = (float *)malloc(i-1, sizeof(float));
	accZ  = (float *)malloc(i-1, sizeof(float));
	pitch = (float *)malloc(i-1, sizeof(float));
	roll  = (float *)malloc(i-1, sizeof(float));
	yaw   = (float *)malloc(i-1, sizeof(float));
	
	i = 0;
	
	while(!feof(fpt))
	{
		if (i != 0)
		{
			fscanf(fpt, "%f %f %f %f %f %f %f\n", &time[i-1], &accX[i-1], &accY[i-1], &accZ[i-1], &pitch[i-1], &roll[i-1], &yaw[i-1]);
		}
		i++;
	}
	
	
		
}
	