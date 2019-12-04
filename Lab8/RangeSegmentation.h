typedef struct cart_coord_s
{
	double X;
	double Y;
	double Z;
} cart_coord_t;

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */
#define SQR(x) ((x)*(x))
#define ANGLE_THRESH 0.72

void convert2XYZ(unsigned char *RangeImage, cart_coord_t *Coords, int ROWS, int COLS);
void WriteGreyScaleImage(unsigned char *image, char *filename, int ROWS, int COLS);
void SurfaceNormalCalc(cart_coord_t *Coords, cart_coord_t *SurfaceNormal, int ROWS, int COLS);
void threshold(unsigned char *image, unsigned char *output_image, int thresh_val, int ROWS, int COLS);
void RegionGrow(unsigned char *image,
	unsigned char *labels,
	int ROWS, int COLS,
	int r, int c,
	int paint_over_label,
	int new_label,
	cart_coord_t *SurfaceNormal,
	int *indices,
	int *count);
	
void WriteColorImage(unsigned char *grayscale, int ROWS, int COLS);