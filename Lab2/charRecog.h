struct coordinates_s{
  int  Column;
  int  Row;
  char Letter;
};

void Calc_MSF(unsigned char *image, unsigned char *letter, int ROWS, int COLS, int T_ROWS, int T_COLS, unsigned char *MSF);
void ZMean(int *temp_image, int ROWS, int COLS);
void Normalize(int *temp_image, int ROWS, int COLS);
void find_letter(unsigned char *image, int ROWS, int COLS, int T_ROWS, int T_COLS, struct coordinates_s *coordinates, int CurrentCount);
