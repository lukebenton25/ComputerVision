typedef struct cart_coord_s
{
	double X;
	double Y;
	double Z;
} cart_coord_t;

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define SQR(x) ((x)*(x))
int ROWS;
int COLS;
int BYTES;
#define ANGLE_THRESH 0.6

void WriteGreyScaleImage(unsigned char *image, char *filename);
void SurfaceNormalCalc(cart_coord_t *Coords, cart_coord_t *SurfaceNormal);
void threshold(unsigned char *image, int thresh_val);
void RegionGrow(unsigned char *image,
	unsigned char *labels,
	int r, int c,
	int paint_over_label,
	int new_label,
	cart_coord_t *SurfaceNormal,
	int *indices,
	int *count);
void convert2XYZ(unsigned char *RangeImage, cart_coord_t *Coords);