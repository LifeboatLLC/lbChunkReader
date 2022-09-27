#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "workerParameters.h"
#include "printFunctions.h"
#include "allocate2D.h"
#include "getChunkInfo.h"
#include "readDirectly.h"
#include "writeHdfFile.h"
#include "readDirectlyWithThreads.h"
#include "readHDFByHyperslab.h"

#define DATASET  "DS1"
#define N_DIMENSIONS 2
void processCommandLine (int argc, char **argv,
			 char **filename,
			 int *rowsPerChunk, int *columnsPerChunk,
			 int *nRowsOfChunks, int *nColumnsOfChunks,
			 int *nThreads, int *copyToArray, int *nIterations,
			 int *printFlag)
{
  const int N_ARGS = 10;

  if (argc != N_ARGS)
  {
    printf
	 ("usage: %s <filename> <rowsPerChunk> <columnsPerChunk> <nRowsOfChunks> "
	  "<nColumnsOfChunks> <nThreads> <copyToArray> <nIterations> <printDataFlag>\n",
	  argv[0]);
    exit (0);
  }
  else
  {
    *filename = argv[1], sscanf (argv[2], "%d", rowsPerChunk);
    sscanf (argv[3], "%d", columnsPerChunk);
    sscanf (argv[4], "%d", nRowsOfChunks);
    sscanf (argv[5], "%d", nColumnsOfChunks);
    sscanf (argv[6], "%d", nThreads);
    sscanf (argv[7], "%d", copyToArray);
    sscanf (argv[8], "%d", nIterations);
    sscanf (argv[9], "%d", printFlag);

    if (*printFlag)
    {
      printf ("Filename is %s\n", *filename);
      printf ("Rows per chunk = %d\n", *rowsPerChunk);
      printf ("Columns per chunk = %d\n", *columnsPerChunk);
      printf ("Number of rows of chunks = %d\n", *nRowsOfChunks);
      printf ("Number of columns of chunks = %d\n", *nColumnsOfChunks);
      printf ("Number of threads = %d\n", *nThreads);
      printf ("Copy to array = %d\n", *copyToArray);
      printf ("Number of iterations = %d\n", *nIterations);
    }
  }
}

bool fileExists (char *filename)
{
  struct stat statStruct;
  int status = stat (filename, &statStruct);
  return status == 0;
}


int main (int argc, char **argv)
{
  int rowsPerChunk;
  int columnsPerChunk;
  int nRowsOfChunks;
  int nColumnsOfChunks;
  int nThreads;
  int nIterations;
  int copyToArray;
  int printFlag;
  char *filename;

  processCommandLine (argc, argv,
		      &filename,
		      &rowsPerChunk, &columnsPerChunk,
		      &nRowsOfChunks, &nColumnsOfChunks,
		      &nThreads, &copyToArray, &nIterations, &printFlag);

  int nArrayRows = nRowsOfChunks * rowsPerChunk;
  int nArrayColumns = nColumnsOfChunks * columnsPerChunk;
  if (printFlag)
  {
    printf ("nArrayRows = %d\n", nArrayRows);
    printf ("nArrayColumns = %d\n", nArrayColumns);
  }

  /* Dynamically allocate 2D arrays. */
  int **wdata = (int **) allocateContiguous2dArray (nArrayRows,
						    nArrayColumns,
						    sizeof (int));
  int **rdata = (int **) allocateContiguous2dArray (nArrayRows,
						    nArrayColumns,
						    sizeof (int));

  /* Write the HDF file that we will read in the tests. */
  if (!fileExists (filename))
  {
    printf ("Writing new HDF5 file %s.\n", filename);
    writeHdfFile (filename, DATASET, wdata, rowsPerChunk,
		  columnsPerChunk, nRowsOfChunks, nColumnsOfChunks, printFlag);
  }
  else
  {
    printf ("Using existing HDF5 file %s.\n", filename);
  }
  /* Read the data by HDF hyperslab */
  clock_t hdfSpan = readHDFByHyperslab (filename, DATASET, rdata, rowsPerChunk,
					columnsPerChunk, nRowsOfChunks,
					nColumnsOfChunks, nIterations,
					printFlag);

  printf ("HDF time to read = %ld\n\n", hdfSpan);


  hsize_t **allChunkOffsets;
  hsize_t maxChunkSize;
  hsize_t nChunks = nRowsOfChunks * nColumnsOfChunks;

  /* Allocate an array to hold the number of bytes in each chunk. */
  hsize_t *chunkSizeInBytes = (hsize_t *) malloc (nChunks * sizeof (hsize_t));

  /* Allocate a 2d array with nRowsOfChunks rows and nColumnOfChunks columns. */
  /* This array will store the offset in the file for each chunk.  Chunks */
  /* will be accessed using the row, column of the chunk. */
  allChunkOffsets = (hsize_t **)
       allocateContiguous2dArray (nRowsOfChunks, nColumnsOfChunks,
				  sizeof (hsize_t));


  /* Fill in the chunkSizeInBytes and allChunkOffsets arrays with  */
  /* information from the HDF file. */
  getChunkInfo (filename, DATASET, rowsPerChunk, columnsPerChunk,
		nRowsOfChunks, nColumnsOfChunks,
		chunkSizeInBytes, allChunkOffsets, &maxChunkSize, printFlag);

  /* Use the chunk info to read the chunks without the use of the HDF  */
  /* library. */
  printf ("Reading direct\n");
  clock_t directSpan = readDirectly (filename,
				     rdata,
				     rowsPerChunk, columnsPerChunk,
				     nRowsOfChunks, nColumnsOfChunks,
				     chunkSizeInBytes,
				     allChunkOffsets,
				     maxChunkSize,
				     copyToArray,
				     nIterations,
				     printFlag);


  printf ("Direct time = %ld\n\n", directSpan);
  if (printFlag)
  {
    print2dArray ("Data as read using direct read",
		  rdata, nArrayRows, nArrayColumns);
  }

  /* Perform direct reads with threads.  A subset of chunks is  */
  /* read by each thread. */
  clock_t threadSpan = readDirectlyWithThreads (filename,
						rdata,
						rowsPerChunk, columnsPerChunk,
						nRowsOfChunks, nColumnsOfChunks,
						chunkSizeInBytes,
						allChunkOffsets,
						maxChunkSize, copyToArray,
						nThreads,
						nIterations, printFlag);

  if (printFlag)
  {
    print2dArray ("Data as read threaded direct read",
		  rdata, nArrayRows, nArrayColumns);
  }

  printf ("Thread time is %ld\n\n", threadSpan);
  printf ("HDF/Direct is %lf\n", ((double) hdfSpan) / (double) directSpan);
  printf ("HDF/Thread is %lf\n", ((double) hdfSpan) / (double) threadSpan);

  /* Deallocate all dynamically allocated memory. */

  free (chunkSizeInBytes);
  dealloc2dArray ((void **) wdata, nArrayRows);
  dealloc2dArray ((void **) rdata, nArrayRows);
  dealloc2dArray ((void **) allChunkOffsets, nRowsOfChunks);
  return 0;
}
