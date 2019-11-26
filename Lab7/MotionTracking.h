enum {
	TIME,
	ACCX,
	ACCY,
	ACCZ,
	PITCH,
	ROLL,
	YAW
};

int DATA_SIZE;

#define VAR_WINDOW 10
#define ACC_THRESH 0.0001
#define PITCH_THRESH 0.005
#define ROLL_THRESH 0.05
#define YAW_THRESH 0.005
#define SAMPLE_TIME 0.05
#define GRAVITY 9.81

float variance (float *input, int curr_index);
void PrintData(FILE *fp, float distance[7], float start_time, float end_time, int start_index, int end_index);
float integrate_acc(float *input, int start, int end);
float integrate(float *input, int start, int end);
bool isMoving(float var_arr[7]);