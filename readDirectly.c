#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"

// Read an HDF file using direct reads, rather than using 
// the HDF library.
clock_t readDirectly (const char *filename, int **rdata,
		      int rowsPerChunk, int columnsPerChunk,
		      int nRowsOfChunks, int nColumnsOfChunks,
		      hsize_t * chunkSizeInBytes,
		      hsize_t ** chunkLocationInFile,
		      int nIterations, int printFlag)
{
  int nArrayRows = rowsPerChunk * nRowsOfChunks;
  int nArrayColumns = columnsPerChunk * nColumnsOfChunks;
  int nChunks = nRowsOfChunks * nColumnsOfChunks;
  /* 
   * Initialize the read array.
   */
  memset (rdata[0], 0, nArrayRows * nArrayColumns * sizeof (int));
  /* 
   * Read the data using the previously defined hyperslab.
   */
  // Open the file
  FILE *directFile = fopen (filename, "rb");

  // Extract the file descriptor
  int fd = fileno (directFile);

  // Find the maximum chunk size.
  // In this example all chunks should be the same size.
  int maxChunkSize = 0;
  int j;

  for (j = 0; j < nChunks; j++)
    maxChunkSize = (maxChunkSize < chunkSizeInBytes[j]) ?
	 chunkSizeInBytes[j] : maxChunkSize;

  int *chunkBuffer = malloc (maxChunkSize * sizeof (int));
  int row;
  int column;
  printf ("Reading direct\n");
  clock_t beginDirect = clock ();
  int iteration;
  for (iteration = 0; iteration < nIterations; ++iteration)
  {
    int arrayRow = 0;
    int arrayColumn = 0;
    int chunkRow = 0;
    int chunkColumn = 0;
    int chunkIndex = 0;
    int readBufferIndex = 0;

    int readRow = 0;
    int readColumn = 0;
    int nBytes = 0;

    for (readRow = 0; readRow < nRowsOfChunks; ++readRow)
    {
      for (readColumn = 0; readColumn < nColumnsOfChunks; ++readColumn)
      {
	nBytes = pread (fd, chunkBuffer, chunkSizeInBytes[chunkIndex++],
			(off_t) chunkLocationInFile[readRow][readColumn]);

	readBufferIndex = 0;
	for (int chunkRow = 0; chunkRow < rowsPerChunk; ++chunkRow)
	{
	  for (int chunkColumn = 0; chunkColumn < columnsPerChunk;
	       ++chunkColumn)
	  {
	    rdata[arrayRow + chunkRow][arrayColumn + chunkColumn] =
		 chunkBuffer[readBufferIndex++];
	  }
	}

	arrayColumn += columnsPerChunk;
	if (arrayColumn >= columnsPerChunk * nColumnsOfChunks)
	{
	  arrayColumn = 0;
	  arrayRow += rowsPerChunk;
	}
      }
    }
  }

  clock_t endDirect = clock ();
  clock_t directSpan = endDirect - beginDirect;
  fclose (directFile);
  free (chunkBuffer);
  return directSpan;
}


