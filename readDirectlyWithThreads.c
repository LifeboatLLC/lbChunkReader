#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "hdf5.h"
#include "workerParameters.h"
#include "worker.h"

// Create one or more threads to read an HDF file directly.
clock_t readDirectlyWithThreads (const char *filename,
	int **rdata,
	int rowsPerChunk,
	int columnsPerChunk,
	int nRowsOfChunks,
	int nColumnsOfChunks,
	hsize_t * chunkSizeInBytes,
	hsize_t ** allChunkOffsets,
	hsize_t maxChunkSize,
	int copyToArray,
	int nThreads, 
	int nIterations, 
        int printFlag)
{
  // Open the file
  FILE *directFile = fopen (filename, "rb");

  // Extract the file descriptor
  int fd = fileno (directFile);

  int nArrayRows = rowsPerChunk * nRowsOfChunks;
  int nArrayColumns = columnsPerChunk * nColumnsOfChunks;
  int nChunks = nRowsOfChunks * nColumnsOfChunks;

  // Allocate thread parameter structures - one for each
  // thread.
  struct WorkerParameters *params =
       malloc (sizeof (struct WorkerParameters) * nThreads);

  // Allocate array of thread read buffer pointers.
  int **threadReadBuffer = malloc (sizeof (int *) * nThreads);

  // Allocate the thread read buffers.
  int i;
  for (i = 0; i < nThreads; i++)
  {
    threadReadBuffer[i] = malloc (maxChunkSize);
  }

  // Compute the number of chunks each thread should read.
  int nChunksToRead = 1;
  int startChunkNumber = 0;
  if (nChunks < nThreads)
  {
    printf ("Warning: only %d chunks, using %d rather than %d threads\n",
	    nChunks, nChunks, nThreads);
    nThreads = nChunks;
  }

  // Compute number of chunks/thread.
  // If nChunks not divisible by nThreads,
  // excessChunks provides the number of threads
  // that will read an extra chunk.
  int minChunkPerThread = nChunks / nThreads;
  int excessChunks = nChunks % nThreads;

  // Create parameter structure for each thread
  for (i = 0; i < nThreads; ++i)
  {
    nChunksToRead = minChunkPerThread;
    if (i < excessChunks)
    {
      nChunksToRead = minChunkPerThread + 1;
    }

    // Each thread gets its own workParameter structure
    setWorkerParameters (params + i, allChunkOffsets,
			 chunkSizeInBytes, startChunkNumber,
			 nChunksToRead, rowsPerChunk,
			 columnsPerChunk, nRowsOfChunks,
			 nColumnsOfChunks, threadReadBuffer[i],
                         copyToArray,
			 rdata, fd, nIterations);
    startChunkNumber += nChunksToRead;
  }

  // Zero out the array so we can see where we wrote
  memset (rdata[0], 0, nArrayRows * nArrayColumns * sizeof (int));

  // Create an array of thread Ids
  pthread_t *id = malloc (nThreads * sizeof (pthread_t));
  printf ("Reading direct with threads\n");

  clock_t beginThread = clock ();

  // Create the threads and start them working.
  for (i = 0; i < nThreads; ++i)
  {
    pthread_create (id + i, NULL, worker, (void *) (params + i));
  }

  // Wait for all threads to complete.
  for (i = 0; i < nThreads; ++i)
  {
    pthread_join (id[i], NULL);
  }

  // Record time
  clock_t endThread = clock ();

  // Free all allocated memory.

  // Free the thread buffers.
  for (i = 0; i < nThreads; ++i)
  {
    free (threadReadBuffer[i]);
  }

  // Free the thread buffer pointers.
  free (threadReadBuffer);

  // Free the arary of thread ids.
  free (id);

  // Free the array of parameter structures.
  free (params);

  // Return the processing time.
  return endThread - beginThread;
}
