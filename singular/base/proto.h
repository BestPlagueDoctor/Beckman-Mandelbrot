//This is for prototyping all funcs and objects eh
#include <stdio.h>
#include <vector>

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
void rescale_colors_global(float &zeroimg, int &img, int xres, int yres, int &maxval, int res);
void imgmandel(int maxiter, int &img, int xres, int yres);
void write_pgm(const char *filename, int &img, int xres, int yres, int res, int maxiter);
void rescale_colors_pgm(int &img, int res, int maxiter);
void printcoord(int &img, int xres, int yres, int res); 
//float rescale_r_ppm(std::vector<float> &zeroimg, int i, int &r); 
//float rescale_g_ppm(std::vector<float> &zeroimg, int i, int &g); 
//float rescale_b_ppm(std::vector<float> &zeroimg, int i, int &b); 
void write_ppm(const char *filename, float &zeroimg, int xres, int yres, int res, int maxval);
