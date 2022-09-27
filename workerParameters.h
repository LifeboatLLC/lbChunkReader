#ifndef WORKER_PARAMETER_H
#define WORKDER_PARAMETER_H

struct WorkerParameters
{
  haddr_t **chunkLocationInFile;
  hsize_t *chunkSizeInBytes;
  int startChunkNumber;
  int nChunksToRead;
  int rowsPerChunk;
  int columnsPerChunk;
  int nRowsOfChunks;
  int nColumnsOfChunks;
  int *readBuffer;
  int copyToArray;
  int **bigArray;
  int fd;
  int nIterations;
};

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
			  int **bigArray, int fd, int nIterations);

void freeWorkerParameters (struct WorkerParameters *params);

#endif
