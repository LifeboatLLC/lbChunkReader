#include <stdlib.h>
#include "hdf5.h"
#include "getChunkInfo.h"
#include "allocate2D.h"
#define N_DIMENSIONS 2

/* Extract the chunk locations and chunk sizes from an */
/* hdf file.*/
void getChunkInfo (const char *filename,
		   const char *datasetName,
		   int rowsPerChunk, int columnsPerChunk,
		   int nRowsOfChunks, int nColumnsOfChunks,
		   haddr_t * chunkSizeInBytes,
		   hsize_t ** allChunkOffsets, hsize_t * maxChunkSize,
		   int printFlag)
{
  hid_t file, space, dset, dcpl;	
  herr_t status;

  hsize_t start[N_DIMENSIONS];
  hsize_t stride[N_DIMENSIONS];
  hsize_t block[N_DIMENSIONS];
  hsize_t count[N_DIMENSIONS];

  /* Determine the total number of chunks */
  hsize_t nChunks = nRowsOfChunks * nColumnsOfChunks;

  /* Open the file and the data set. */
  file = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  dset = H5Dopen (file, datasetName, H5P_DEFAULT);

  /* Retrieve the dataset creation property list. */
  dcpl = H5Dget_create_plist (dset);

  /* Define and select the hyperslab to use for reading. */
  space = H5Dget_space (dset);

  start[0] = 0;
  start[1] = 0;
  stride[0] = rowsPerChunk;
  stride[1] = columnsPerChunk;
  count[0] = 1;
  count[1] = 1;
  block[0] = rowsPerChunk;
  block[1] = columnsPerChunk;
  status = H5Sselect_hyperslab (space, H5S_SELECT_SET, start, stride, count,
				block);

  hsize_t chunkOffset[N_DIMENSIONS];
  unsigned filter_mask;

  hsize_t index;

  /* Find the maximum chunk size. */
  /* In this example all chunks should be the same size. */
  *maxChunkSize = 0;

  if (printFlag)
  {
    printf ("\nExtracted HDF5 chunk info: \n");
  }

  int chunkRow = 0;
  int chunkColumn = 0;
  for (index = 0; index < nChunks; index++)
  {


    status = H5Dget_chunk_info (dset, space, index, chunkOffset,
				&filter_mask,
				&allChunkOffsets[chunkRow][chunkColumn],
				chunkSizeInBytes + index);

    if (printFlag)
    {
      printf
	   ("\tchunkRow = %d: chunkColumn =%d: index = %ld: nBytes %ld: address: %ld\n",
	    chunkRow, chunkColumn, index, chunkSizeInBytes[index],
	    allChunkOffsets[chunkRow][chunkColumn]);
    }

    if (chunkSizeInBytes[index] > *maxChunkSize)
    {
      *maxChunkSize = chunkSizeInBytes[index];
    }

    ++chunkColumn;
    if (chunkColumn >= nColumnsOfChunks)
    {
      chunkColumn = 0;
      ++chunkRow;
    }
  }

  if (printFlag)
  {
    int i;
    int j;
    int index = 0;
    for (i = 0; i < nRowsOfChunks; i++)
    {
      for (j = 0; j < nColumnsOfChunks; j++)
      {
	printf
	     ("\tchunkRow = %d: chunkColumn =%d: index = %d: nBytes %ld: address: %ld\n",
	      i, j, index, chunkSizeInBytes[index++], allChunkOffsets[i][j]);
      }
    }
    printf ("\n");
  }

}
