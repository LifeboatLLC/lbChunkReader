#ifndef ALLOCATE2D_H
#define ALLOCATE2D_H
void dealloc2dArray (void **array, int nRows);
void **allocateContiguous2dArray (int nRows, int nColumns, int bytesPerElement);
#endif
