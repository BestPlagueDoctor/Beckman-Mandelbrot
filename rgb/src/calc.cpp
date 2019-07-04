#include "proto.h"
#include <cmath>
void looper(int &iters, complexnumber z, complexnumber c);
/*float findx(int xres, int i) {
  float xcoord = floor(i%xres);
  return xcoord;
}
float findy(int xres, int i) {
  float ycoord = floor(i/xres);
  return ycoord;
}*/

int findidx(int xres, int x, int y) {
  return y*xres + x;
}

void setvals(std::vector<int> &img, int xres, int yres) {
  int x, y;
  for (y = 0; y < yres; y++) {
    for (x = 0; x < xres; x++) {
      int idx = findidx(xres, x, y);
      img[idx] = idx;
    }
  }
}

/*void printcoord(int *img, int xres, int yres, int res) {
  int i, x, y;
  for (i = 0; i < res; i++) {
    x = findx(xres, i);
    y = findy(yres, i);
    int idx = findidx(xres, x, y);
    printf("(%d %c %d)", x, ',', y);
    if (x == (xres-1)) {
      printf("\n");
    }
  }
}*/

/*void imgmandel(int maxiter, int *img, int res, int xres, int yres) {
  int i = 0;
  int newit = 0;
  setvals(img, xres, yres);
  complexnumber c;
  complexnumber z;
  complexnumber zsq;
  for (i = 0; i < res; i++) {
    //printf("I am iterating the loop for the %dth time \n", i);
    float rec = findx(xres, i);
    float imc = findy(xres, i);
    z.setcomp(0.0, 0.0); //z=(0+0i)
//    zsq.setcomp(0.0, 0.0);
    float nrec = (((rec*3)/xres)-2), nimc = (((imc*2.4)/yres)-1.2);
    c.setcomp(nrec, nimc);
    int iters = 0;
    for (iters = 0; iters < 4096; iters++) {
      //if (iters == 4095) {
        //newit+=1;
        //printf("I hit the ceiling for the %dth time\n", newit);
      //}
      z.multcomp(z, z); //zsq=z*z
      z.addcomp(z, c); //z=zsq+c
      if (z.sqmagnit() >= 4.0) {
        break;
      }
    }
    //printf("%d", iters);
    img[i] = iters;  
  }
}*/

/*void imgmandel(int maxiter, int *img, int res, int xres, int yres) {
  setvals(img, xres, yres);
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  //printf("%.10f, %.10f", recdiv, imcdiv);
  complexnumber c;
  complexnumber z;
  int y, i;
  for (y = 0; y < yres; y++) {
    int x;
      for (x = 0; x < xres; x++) {
        //next line is crucial bc idk
        float rec = x, imc = y;
        float nimc = ((imc*imcdiv)-1.2);
        float nrec = (((rec*recdiv)/xres)-2); 
        c.setcomp(nrec, nimc);
        z.setcomp(0.0,0.0);
        int iters;
        for (iters = 0; iters < maxiter; iters++) {
          z.multcomp(z, z);
          z.addcomp(z, c);
          //printf("%d ", iters);
          if (z.sqmagnit() >= 4.0) {
            break;
          }
        }
        i++;
        img[i] = iters;
      }
    }
  }*/


void imgmandel(int maxiter, std::vector<int> &img, int res, int xres, int yres) {
  setvals(img, xres, yres);
  float recdiv = 3.0/(xres);
  float imcdiv = 2.4/yres;
  //printf("%.10f, %.10f", recdiv, imcdiv);
  complexnumber c;
  complexnumber z;
  complexnumber save;
  int y;
  for (y = 0; y < yres; y++) {
    int x;
      for (x = 0; x < xres; x++) {
        //next line is crucial bc idk
        float rec = x, imc = y;
        float nimc = ((imc*imcdiv)-1.2);
        float nrec = (((rec*recdiv)-2)); 
        c.setcomp(nrec, nimc);
        z.setcomp(0.0,0.0);
        save.setcomp(0.0, 0.0);
        int iters;
        for (iters = 0; iters < maxiter; iters+=4) {
          save.setcomp(z.getreal(), z.getimag());
          z.multcomp(z, z);
          z.addcomp(z, c);
          z.multcomp(z, z);
          z.addcomp(z, c);
          z.multcomp(z, z);
          z.addcomp(z, c);
          z.multcomp(z, z);
          z.addcomp(z, c);
          if (z.sqmagnit() >= 4.0) {
            iters -= 4;
            z.setcomp(save.getreal(), save.getimag());
            looper(iters, z, c);
            break;
          }
        }
        img[((y*xres)+x)] = iters;
      }
    }
  }

void looper(int &iters, complexnumber z, complexnumber c) {
  for (int i = 0; i <= 4; i ++) {
    z.multcomp(z, z);
    z.addcomp(z, c);
    if (z.sqmagnit() >= 4) {
      break;
    }
    iters++;
  }
}

