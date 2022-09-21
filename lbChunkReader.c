#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "workerParameters.h"
#include "printFunctions.h"
#include "allocate2D.h"
#include "getChunkInfo.h"
#include "readDirectly.h"
#include "writeHdfFile.h"
#include "readDirectlyWithThreads.h"
#include "readHDFByHyperslab.h"

#define FILENAME "bigChunk.h5"
#define DATASET  "DS1"
#define N_DIMENSIONS 2
void processCommandLine (int argc, char **argv,
			 int *rowsPerChunk, int *columnsPerChunk,
			 int *nRowsOfChunks, int *nColumnsOfChunks,
			 int *nThreads, int* copyToArray, int *nIterations, int *printFlag)
{
  const int N_ARGS = 9;

  if (argc != N_ARGS)
  {
    printf
	 ("usage: %s <rowsPerChunk> <columnsPerChunk> <nRowsOfChunks> "
	  "<nColumnsOfChunks> <nThreads> <copyToArray> <nIterations> <printDataFlag>\n", argv[0]);
    exit (0);
  }
  else
  {
    sscanf (argv[1], "%d", rowsPerChunk);
    sscanf (argv[2], "%d", columnsPerChunk);
    sscanf (argv[3], "%d", nRowsOfChunks);
    sscanf (argv[4], "%d", nColumnsOfChunks);
    sscanf (argv[5], "%d", nThreads);
    sscanf (argv[6], "%d", copyToArray);
    sscanf (argv[7], "%d", nIterations);
    sscanf (argv[8], "%d", printFlag);

    if (*printFlag)
    {
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

  processCommandLine (argc, argv,
		      &rowsPerChunk, &columnsPerChunk,
		      &nRowsOfChunks, &nColumnsOfChunks,
		      &nThreads, &copyToArray, &nIterations, 
		      &printFlag);


  hid_t file, space, dset, dcpl;	/* Handles */
  herr_t status;
  H5D_layout_t layout;
  int nArrayRows = nRowsOfChunks * rowsPerChunk;
  int nArrayColumns = nColumnsOfChunks * columnsPerChunk;
  if (printFlag)
  {
    printf ("nArrayRows = %d\n", nArrayRows);
    printf ("nArrayColumns = %d\n", nArrayColumns);
  }

  hsize_t dims[N_DIMENSIONS] = { nArrayRows, nArrayColumns },
       chunk[N_DIMENSIONS] = { rowsPerChunk, columnsPerChunk },
       start[N_DIMENSIONS],
       stride[N_DIMENSIONS], count[N_DIMENSIONS], block[N_DIMENSIONS];
  int i, j;

  // Dynamically allocate 2D arrays.
  int **wdata = (int **) allocateContiguous2dArray (nArrayRows,
						    nArrayColumns,
						    sizeof (int));
  int **rdata = (int **) allocateContiguous2dArray (nArrayRows,
						    nArrayColumns,
						    sizeof (int));
  // Write the HDF file that we will read in the tests.
  writeHdfFile (FILENAME, DATASET, wdata, rowsPerChunk,
		columnsPerChunk, nRowsOfChunks, nColumnsOfChunks, printFlag);
  // Read the data by HDF hyperslab
  clock_t hdfSpan = readHDFByHyperslab (FILENAME, DATASET, rdata, rowsPerChunk,
					columnsPerChunk, nRowsOfChunks,
					nColumnsOfChunks, nIterations,
					printFlag);

  printf ("HDF time to read = %ld\n\n", hdfSpan);
  hsize_t **allChunkOffsets;
  hsize_t maxChunkSize;
  hsize_t nChunks = nRowsOfChunks * nColumnsOfChunks;

  hsize_t* chunkSizeInBytes = (hsize_t *) malloc (nChunks * sizeof (hsize_t));

  allChunkOffsets = (hsize_t **)
       allocateContiguous2dArray (nChunks, N_DIMENSIONS, sizeof (hsize_t));


  getChunkInfo (FILENAME, DATASET, rowsPerChunk, columnsPerChunk,
		nRowsOfChunks, nColumnsOfChunks,
		chunkSizeInBytes, allChunkOffsets, &maxChunkSize, printFlag);


  printf ("Reading direct\n");
  clock_t directSpan = readDirectly (FILENAME,
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

  clock_t threadSpan = readDirectlyWithThreads (FILENAME,
						rdata,
						rowsPerChunk, columnsPerChunk,
						nRowsOfChunks, nColumnsOfChunks,
						chunkSizeInBytes,
						allChunkOffsets,
						maxChunkSize, copyToArray, 
                                                nThreads,
						nIterations, printFlag);

  printf ("Thread time is %ld\n\n", threadSpan);
  printf ("HDF/Direct is %lf\n", ((double) hdfSpan) / (double) directSpan);
  printf ("HDF/Thread is %lf\n", ((double) hdfSpan) / (double) threadSpan);

  // Deallocate all dynamically allocated memory.

  
  free(chunkSizeInBytes);
  dealloc2dArray ((void **) wdata, nArrayRows);
  dealloc2dArray ((void **) rdata, nArrayRows);
  dealloc2dArray ((void **) allChunkOffsets, nChunks);
  return 0;
}
