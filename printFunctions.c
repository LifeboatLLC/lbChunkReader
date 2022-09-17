#include <stdio.h>
#include "printFunctions.h"

// Print a small 2d array.
void print2dArray (const char *message, int **data, int nRows, int nColumns)
{
  int i, j;
  printf ("\n%s:\n", message);
  for (i = 0; i < nRows; i++)
  {
    printf (" [");
    for (j = 0; j < nColumns; j++)
      printf (" %3d", data[i][j]);
    printf ("]\n");
  }
  printf ("\n");
}


