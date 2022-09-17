#include <unistd.h>
#include "hdf5.h"
#include "workerParameters.h"

void *worker (void *inStruct)
{
  int iteration;
  int i, j;
  int chunkBufferIndex;

  // Extract the parameters
  struct WorkerParameters *params = (struct WorkerParameters *) inStruct;

  // Allocate a buffer for reading chunks
  // int *chunkBuffer =
  // malloc (sizeof (int) * params->rowsPerChunk * params->columnsPerChunk);

  // Determine where to start writing the data in the 2d Array
  // Determine the row
  int startArrayRow = (params->startChunkNumber / params->nColumnsOfChunks)
       * params->rowsPerChunk;

  // Determine the column
  int startArrayColumn = (params->startChunkNumber % params->nColumnsOfChunks)
       * params->columnsPerChunk;

  // Repeat the whole process for the specified number of iterations.
  for (iteration = 0; iteration < params->nIterations; ++iteration)
  {
    int arrayColumn = startArrayColumn;
    int arrayRow = startArrayRow;
    for (j = 0; j < params->nChunksToRead; j++)
    {

      // Read a chunk into the chunkBuffer
      pread (params->fd, params->readBuffer, params->chunkSizeInBytes[j],
	     (off_t) params->chunkLocationInFile[params->startChunkNumber + j]);

      // Copy the data to the chunk to the array
      chunkBufferIndex = 0;
      for (int chunkRow = 0; chunkRow < params->rowsPerChunk; ++chunkRow)
      {
	for (int chunkColumn = 0; chunkColumn < params->columnsPerChunk;
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
