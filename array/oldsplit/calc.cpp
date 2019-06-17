#include "proto.h"
int pointcheck(int maxiter, float rec, float imc) {
  complexnumber c;
  complexnumber z;
  complexnumber zsq;
  z.setcomp(0.0, 0.0); //z=(0+0i)
  zsq.setcomp(0.0, 0.0); //zsq=(0+0i)
  c.setcomp(rec, imc); //c=(r+i)
  int i;
  for (i = 0; i < maxiter; i++) {
    zsq.multcomp(z, z); //zsq=z*z
    z.addcomp(zsq, c); //z=zsq+c
    if (z.sqmagnit() >= 4) {break;}
  }
  return i;
}
