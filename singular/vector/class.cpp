#include "proto.h"
void complexnumber::setcomp (float x, float y) {
  real = x;
  imag = y;
}
void complexnumber::addcomp(complexnumber x, complexnumber y) {
  real = x.getreal() + y.getreal();  
  imag = x.getimag() + y.getimag();  
}
void complexnumber::multcomp(complexnumber x, complexnumber y) {
  real = ((x.getreal() * y.getreal()) - (x.getimag() * y.getimag()));
  imag = ((x.getreal() * y.getimag()) + (x.getimag() * y.getreal()));
}  
float complexnumber::getreal () {return real;}
float complexnumber::getimag() {return imag;} 
float complexnumber::sqmagnit() {return ((real * real) + (imag * imag));}
