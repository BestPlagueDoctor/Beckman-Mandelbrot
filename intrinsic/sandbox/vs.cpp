#include <cstdio>

int main() {
  float first = 1.0, second = 2.0, third = 3.0, fourth = 4.0, fifth = 1.0, sixth = 2.0, seven = 3.0, eight = 4.0;
  first += first + fifth;
  second += second + sixth;
  third += third + seven;
  fourth += fourth+ eight;
  printf("%.2f %.2f %.2f %.2f", first, second, third, fourth);
}

