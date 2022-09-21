#include <stdlib.h>
#include "hdf5.h"
#include "workerParameters.h"

// The worker threads receive all their parameters in
// a struct WorderParameters structure.  setWorkerParameters
// loads the struct with the values that provided.
void setWorkerParameters (struct WorkerParameters *params,
			  haddr_t ** chunkLocationInFile,
			  hsize_t * chunkSizeInBytes,
			  int startChunkNumber,
			  int nChunksToRead,
			  int rowsPerChunk,
			  int columnsPerChunk,
			  int nRowsOfChunks,
			  int nColumnsOfChunks,
			  int *readBuffer,
                          int copyToArray,
			  int **bigArray, int fd, int nIterations)
{
  params->chunkLocationInFile = chunkLocationInFile;
  params->chunkSizeInBytes = chunkSizeInBytes;
  params->startChunkNumber = startChunkNumber;
  params->nChunksToRead = nChunksToRead;
  params->rowsPerChunk = rowsPerChunk;
  params->columnsPerChunk = columnsPerChunk;
  params->nRowsOfChunks = nRowsOfChunks;
  params->nColumnsOfChunks = nColumnsOfChunks;
  params->readBuffer = readBuffer;
  params->copyToArray = copyToArray;
  params->bigArray = bigArray;
  params->fd = fd;
  params->nIterations = nIterations;
}

void freeWorkerParameters (struct WorkerParameters *params)
{
  free (params->chunkLocationInFile);
  free (params->chunkSizeInBytes);
}

