struct GT_s{
  int  Column;
  int  Row;
  char Letter;
};

void find_letter(unsigned char *MSF, int ROWS, int COLS, struct GT_s *GT, int size, unsigned char *input_image);
void thinning(unsigned char *image);
void create_copy(unsigned char *input_image, unsigned char *temp_letter, int COLS, int Row, int Column);
void threshold(unsigned char *image, unsigned char *output, int ROWS, int COLS, int thresh_val);
