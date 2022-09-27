#include <unistd.h>
#include "hdf5.h"
#include "workerParameters.h"

void *worker (void *inStruct)
{

  /* Extract the parameters */
  struct WorkerParameters *params = (struct WorkerParameters *) inStruct;

  /* Determine where to start writing the data in the 2d Array */
  /* Determine the row */
  int startArrayRow = (params->startChunkNumber / params->nColumnsOfChunks)
       * params->rowsPerChunk;

  /* Determine the column */
  int startArrayColumn = (params->startChunkNumber % params->nColumnsOfChunks)
       * params->columnsPerChunk;

  /* Repeat the whole process for the specified number of iterations. */
  int iteration;
  for (iteration = 0; iteration < params->nIterations; ++iteration)
  {
    int arrayColumn = startArrayColumn;
    int arrayRow = startArrayRow;
    int chunkBufferIndex;
    int addressRow = params->startChunkNumber / params->nColumnsOfChunks;
    int addressColumn = params->startChunkNumber % params->nColumnsOfChunks;

    int j;
    for (j = 0; j < params->nChunksToRead; j++)
    {
      /* Read a chunk into the chunkBuffer */
      pread (params->fd, params->readBuffer, params->chunkSizeInBytes[j],
	     (off_t) params->chunkLocationInFile[addressRow][addressColumn]);

      ++addressColumn;
      if (addressColumn == params->nColumnsOfChunks)
      {
	addressColumn = 0;
	++addressRow;
      }

      int chunkRow;
      int chunkColumn;

      /* Copy the data from the chunk to the array */
      if (params->copyToArray)
      {
	chunkBufferIndex = 0;
	for (chunkRow = 0; chunkRow < params->rowsPerChunk; ++chunkRow)
	{
	  for (chunkColumn = 0; chunkColumn < params->columnsPerChunk;
	       ++chunkColumn)
	  {
	    params->bigArray[arrayRow + chunkRow][arrayColumn +
						  chunkColumn] =
		 params->readBuffer[chunkBufferIndex++];
	  }
	}

	arrayColumn += params->columnsPerChunk;

	if (arrayColumn >= params->columnsPerChunk * params->nColumnsOfChunks)
	{
	  arrayColumn = 0;
	  arrayRow += params->rowsPerChunk;
	}
      }
    }
  }
}
