#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <math.h>
#include <string.h>
#include <cstdlib>

int main() {
	int res = 4*4;
  FILE *ifp;
  if ((ifp = fopen("list.txt", "r")) == NULL) {
    perror("Failed to open output file\n");
    return 0;
  }
  int *pxl = (int*) malloc(res*3*sizeof(int));
  int i, temp;
  int doesntmatter;
  char whocares;
  fscanf(ifp, "%c%d\n", &whocares, &doesntmatter);
  fscanf(ifp, "%d %d \n", &doesntmatter, &doesntmatter);
  fscanf(ifp, "%d\n", &doesntmatter);
  while ( fscanf(ifp, "%d", &temp) != EOF) {
    printf("%d ", i);
    pxl[i] = temp;
    printf("%d ", pxl[i]);
    i++;
  }
  fclose(ifp);
  for (int q = 0; q <= res*3; q+=3) {
    printf("\n");
    printf("%d %d %d ", pxl[q], pxl[q+1], pxl[q+2]);
  }
  char filename[1024];
  strcpy(filename, "listtest.ppm");
  FILE *ofp;
  if ((ofp = fopen(filename, "w")) == NULL) {
    perror("Failed to open output file\n");
    return 0;
  }
  fprintf(ofp, "P3\n");
  fprintf(ofp, "%d %d \n", 4, 4);
  fprintf(ofp, "%d \n", 255);
  for (int i = 0; i < res*3; i+=3) {
    int r, g, b;
    r = 15*pxl[i];
    g = 15*pxl[i+1];
    b = 15*pxl[i+2];
    fprintf(ofp, "%d %d %d  ", r, g, b);
    if ((i+1*3)%4 == 0) {
      fprintf(ofp, "\n");
    }
  }



  
  free(pxl);

/*do
{
  statements;
} while();
*/

}
