//This is for prototyping all funcs and objects eh
#include <stdio.h>

class complexnumber {
  float real, imag;
  public:
    void setcomp (float, float);
    void getcomp ();
    void addcomp(complexnumber, complexnumber);
    void multcomp(complexnumber, complexnumber);
    float getreal();
    float getimag();  
    float sqmagnit();
};
int pointcheck(int maxiter, float rec, float imc);
void mandlebrot_pgm(float xmin, float ymin, float xmax, float ymax, int xres, int yres, int maxiter, const char *filename);

