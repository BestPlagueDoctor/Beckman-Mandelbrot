#include "proto.h"
int pointcheck(int maxiter, float rec, float imc) {
  complexnumber c;
  complexnumber z;
  complexnumber zsq;
  z.setcomp(0.0, 0.0); //z=(0+0i)
  zsq.setcomp(0.0, 0.0); //zsq=(0+0i)
  c.setcomp(rec, imc); //c=(r+i)
  int i;
  for (i = 0; i < 2000; i++) {
    zsq.multcomp(z, z); //zsq=z*z
    //printf("(%.2f %.2f)   ",zsq.getreal(), zsq.getimag()); 
    z.addcomp(zsq, c); //z=zsq+c
    //printf("  (%.2f %.2f) ",z.getreal(), z.getimag()); 
    if (z.sqmagnit() >= 4) {break;}
  }
  return i;
}
