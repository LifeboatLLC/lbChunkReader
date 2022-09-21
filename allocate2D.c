#include <stdlib.h>
#include "allocate2D.h"

// Allocate a contiguous 2-dimensional array.
void **allocateContiguous2dArray (int nRows, int nColumns, int bytesPerElement)
{
  // Allocate contiguous memory.
  void *contiguousMemory = malloc (bytesPerElement * nRows * nColumns);

  // Allocate an array of pointers, one for each row.
  void **newArray = malloc (sizeof (void *) * nRows);

  // Set the pointer for each row to point to its place in
  // contiguous memory.
  int row;
  for (row = 0; row < nRows; ++row)
  {
    newArray[row] = contiguousMemory + row * nColumns * bytesPerElement;
  }

  // Return the array of pointers to rows
  return newArray;
}

void dealloc2dArray (void **array, int nRows)
{
  // Deallocate the contiguous block of memory.
  free (array[0]);

  // Deallocate the pointers to the rows.
  free (array);
}
