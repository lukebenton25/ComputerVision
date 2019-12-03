typedef struct cart_coord_s
{
	double X;
	double Y;
	double Z;
} cart_coord_t;

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define SQR(x) ((x)*(x))
#define ROWS	128
#define COLS	128