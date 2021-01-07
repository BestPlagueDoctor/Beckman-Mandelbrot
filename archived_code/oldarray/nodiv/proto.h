//This is for prototyping all funcs and objects eh
#include <cstdio>
#include <vector>
#include <cstdint>

using Pixel = int32_t;
using Image = std::vector<std::vector<Pixel>>;

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
void imgmandel(int maxiter, int *xray,int *yray, int res, int xres, int yres);
void write_pgm(const char *filename, int *xray,int *yray, int xres, int yres, int res, int maxiter);
void rescale_colors_pgm(int *xray, int *yray, int res, int *maxiter);
void printcoord(Image &img, int xres, int yres, int res); 
