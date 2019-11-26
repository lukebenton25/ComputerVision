#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "MotionTracking.h"

#define SQR(x) ((x)*(x))

int main()
{
	float **Table;
	FILE  *fpt;
	FILE  *fpt2;
	FILE  *Output;
	bool  moving;
	int   i, j;
	char  c;
	float var;
	float dist_arr[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	float var_arr[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	float start_time = 0.0;
	float end_time = 0.0;
	float startIndex = 0;
	float endIndex = 0;

	
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
	
	DATA_SIZE = i;
	
	rewind(fpt);
	
	Table  = (float **)malloc(7*sizeof(float *));
	for (i = 0; i < 7; i++)
	{
		Table[i] = (float *)malloc(DATA_SIZE*sizeof(float));
	}
	
	i = 0;
	
	while(!feof(fpt) && !(i == DATA_SIZE))
	{
		if (i == 0)
		{
			fscanf(fpt, "time	accX	accY	accZ	pitch	roll	yaw							\n");
		}
		else
		{
			fscanf(fpt, "%f %f %f %f %f %f %f \n", &Table[TIME][i-1], &Table[ACCX][i-1], &Table[ACCY][i-1], &Table[ACCZ][i-1], &Table[PITCH][i-1], &Table[ROLL][i-1], &Table[YAW][i-1]);
		}
		i++;
	}
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		Table[ACCZ][i] += 1;
	}
	
	fpt2 = fopen("variance.csv", "w");
	fprintf(fpt2, "TIME,ACCX,ACCY,ACCZ,PITCH,ROLL,YAW\n");
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		fprintf(fpt2, "%f,", Table[TIME][i]);
		for (j = ACCX; j <= YAW; j++)
		{
			var = variance(Table[j], i);
			fprintf(fpt2, "%f,", var);
		}
		fprintf(fpt2, "\n");
	}
	fclose(fpt2);
	
	Output = fopen("Results.txt", "w");
	fprintf(Output, "RESULTS:\n");
	
	for (i = 0; i < DATA_SIZE; i++)
	{
		for (j = ACCX; j <= YAW; j++)
		{
			var_arr[j] = variance(Table[j], i);
		}
		moving = isMoving(var_arr);
		if (moving && start_time == 0.0)
		{
			start_time = Table[TIME][i];
			startIndex = i;
		}
		else if (!moving && start_time != 0.0 && end_time == 0.0)
		{
			moving = false;
			end_time = Table[TIME][i];
			endIndex = i;
		}
		if (start_time != 0.0 && end_time != 0.0)
		{
			dist_arr[ACCX] = integrate_acc(Table[ACCX], startIndex, endIndex);
			dist_arr[ACCY] = integrate_acc(Table[ACCY], startIndex, endIndex);
			dist_arr[ACCZ] = integrate_acc(Table[ACCZ], startIndex, endIndex);
			dist_arr[PITCH] = integrate(Table[PITCH], startIndex, endIndex);
			dist_arr[ROLL] = integrate(Table[ROLL], startIndex, endIndex);
			dist_arr[YAW] = integrate(Table[YAW], startIndex, endIndex);
			
			PrintData(Output, dist_arr, start_time, end_time, startIndex, endIndex);
			start_time = 0.0;
			end_time = 0.0;
		}
		
	}
	fclose(Output);
	return(0);
}

void PrintData(FILE *fp, float distance[7], float start_time, float end_time, int start_index, int end_index)
{
	fprintf(fp, "########################################################\n");
	fprintf(fp, "X Movement: %f [m]\nY Movement: %f [m]\nZ Movement: %f [m]\n", distance[ACCX], distance[ACCY], distance[ACCZ]);
	fprintf(fp, "Pitch Movement: %f [rad]\nRoll Movement: %f [rad]\nYaw Movement: %f [rad]\n", distance[PITCH], distance[ROLL], distance[YAW]);
	fprintf(fp, "Start Time: %0.2f\t\tEnd Time: %0.2f\n", start_time, end_time);
	fprintf(fp, "Start Index: %d\t\tEnd Index: %d\n", start_index, end_index);
	fprintf(fp, "\n\n");
	
	return;
}

float integrate_acc(float *input, int start, int end)
{
	int i;
	float prev_velocity = 0.0;
	float velocity = 0.0;
	float distance = 0.0;
	
	for (i = start; i <= end; i++)
	{
		prev_velocity = velocity;
		velocity += input[i] * GRAVITY * SAMPLE_TIME;
		distance += ((velocity + prev_velocity) / 2) * SAMPLE_TIME;
	}
	
	return (distance);
}

float integrate(float *input, int start, int end)
{
	float ret_val = 0.0;
	int   i;
	for (i = start; i <= end; i++)
	{
		ret_val += input[i] * SAMPLE_TIME;
	}
	
	return (ret_val);
}

float variance (float *input, int curr_index)
{
	int   i;
	int   local_var_window;
	float mean = 0;
	float var;
	if (curr_index + VAR_WINDOW <= DATA_SIZE)
	{
		local_var_window = curr_index + VAR_WINDOW;
	}
	else
	{
		local_var_window = DATA_SIZE;
	}
	for (i = curr_index; i < local_var_window; i++)
	{
		mean += input[i];
	}
	mean = mean / (VAR_WINDOW  + 1);
	
	for (i = curr_index; i < local_var_window; i++)
	{
		var += SQR(input[i] - mean);
	}
	var = var / (VAR_WINDOW + 1);
	
	return var;
}

bool isMoving(float var_arr[7])
{
	bool ret_val = false;
	if (var_arr[ACCX] > ACC_THRESH || var_arr[ACCY] > ACC_THRESH || var_arr[ACCZ] > ACC_THRESH)
	{
		ret_val = true;
	}
	if (var_arr[PITCH] > PITCH_THRESH || var_arr[ROLL] > ROLL_THRESH || var_arr[YAW] > YAW_THRESH)
	{
		ret_val = true;
	}
	
	return (ret_val);
}
