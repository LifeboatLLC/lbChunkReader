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

#define FILENAME "bigChunk.h5"
#define DATASET  "DS1"
#define N_DIMENSIONS 2
clock_t readHDFByHyperslab (const char *filename, int **rdata,
			    int rowsPerChunk, int columnsPerChunk,
			    int nRowsOfChunks, int nColumnsOfChunks,
			    int nIterations, int printFlag)
{
  hid_t file, space, dset, dcpl;	/* Handles */
  herr_t status;

  hsize_t start[N_DIMENSIONS];
  hsize_t stride[N_DIMENSIONS];
  hsize_t block[N_DIMENSIONS];
  hsize_t count[N_DIMENSIONS];
  hsize_t dims[N_DIMENSIONS];
  hsize_t chunk[N_DIMENSIONS];

  int nArrayRows = rowsPerChunk * nRowsOfChunks;
  int nArrayColumns = columnsPerChunk * nColumnsOfChunks;
  int nChunks = nRowsOfChunks * nColumnsOfChunks;

  /* 
   * Open file and dataset using the default properties.
   */
  file = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  dset = H5Dopen (file, DATASET, H5P_DEFAULT);

  /* 
   * Retrieve the dataset creation property list, and print the
   * storage layout.
   */
  dcpl = H5Dget_create_plist (dset);

  /* 
   * Read the data using the default properties.
   */
  // rdata[0] is the address of the contiguous block of memory
  status = H5Dread (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
		    rdata[0]);

  /* 
   * Output the data to the screen - only useful for small data sets
   */
  if (printFlag)
  {
    print2dArray ("Data as written to disk by hyperslabs",
		  rdata, nArrayRows, nArrayColumns);
  }

  /* 
   * Initialize the read array.
   */
  memset (rdata[0], 0, nArrayRows * nArrayColumns * sizeof (int));

  /* 
   * Define and select the hyperslab to use for reading.
   */
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
  haddr_t *chunkLocationInFile =
       (haddr_t *) malloc (nChunks * sizeof (haddr_t));
  hsize_t *chunkSizeInBytes = (haddr_t *) malloc (nChunks * sizeof (hsize_t));


  hsize_t **allChunkOffsets = (hsize_t **) allocateContiguous2dArray (nChunks,
								      N_DIMENSIONS,
								      sizeof
								      (hsize_t));

  hsize_t index;


  for (index = 0; index < nChunks; index++)
  {
    status = H5Dget_chunk_info (dset, space, index, chunkOffset,
				&filter_mask, chunkLocationInFile + index,
				chunkSizeInBytes + index);
  }

  /* 
   * Read the data using the previously defined hyperslab.
   */
  printf ("Reading with hdf5\n");
  int iteration;
  int chunkCount = 0;

  clock_t beginHDF = clock ();

  // Read all the data, one chunk at a time.
  for (iteration = 0; iteration < nIterations; ++iteration)
  {
    for (start[0] = 0; start[0] < nRowsOfChunks * rowsPerChunk;
	 start[0] += rowsPerChunk)
    {
      for (start[1] = 0; start[1] < nColumnsOfChunks * columnsPerChunk;
	   start[1] += columnsPerChunk)
      {
	status = H5Sselect_hyperslab (space, H5S_SELECT_SET, start,
				      stride, count, block);
	status =
	     H5Dread (dset, H5T_NATIVE_INT, H5S_ALL, space, H5P_DEFAULT,
		      rdata[0]);
	++chunkCount;
      }
    }
  }

  clock_t endHDF = clock ();
  clock_t hdfSpan = endHDF - beginHDF;

  /* 
   * Output the data to the screen.
   */
  if (printFlag)
  {
    print2dArray ("Data as read from disk by hyperslab",
		  rdata, nArrayRows, nArrayColumns);
  }

  /* 
   * Close and release resources.
   */
  status = H5Pclose (dcpl);
  status = H5Dclose (dset);
  status = H5Sclose (space);
  status = H5Fclose (file);
  return hdfSpan;
}
void processCommandLine (int argc, char **argv,
			 int *rowsPerChunk, int *columnsPerChunk,
			 int *nRowsOfChunks, int *nColumnsOfChunks,
			 int *nThreads, int *nIterations, int *printFlag)
{
  const int N_ARGS = 8;

  if (argc != N_ARGS)
  {
    printf
	 ("usage: %s <rowsPerChunk> <columnsPerChunk> <nRowsOfChunks> "
	  "<nColumnsOfChunks> <nThreads> <nIterations> <printDataFlag>\n", argv[0]);
    exit (0);
  }
  else
  {
    sscanf (argv[1], "%d", rowsPerChunk);
    sscanf (argv[2], "%d", columnsPerChunk);
    sscanf (argv[3], "%d", nRowsOfChunks);
    sscanf (argv[4], "%d", nColumnsOfChunks);
    sscanf (argv[5], "%d", nThreads);
    sscanf (argv[6], "%d", nIterations);
    sscanf (argv[7], "%d", printFlag);

    if (*printFlag)
    {
      printf ("Rows per chunk = %d\n", *rowsPerChunk);
      printf ("Columns per chunk = %d\n", *columnsPerChunk);
      printf ("Number of rows of chunks = %d\n", *nRowsOfChunks);
      printf ("Number of columns of chunks = %d\n", *nColumnsOfChunks);
      printf ("Number of threads = %d\n", *nThreads);
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
  int printFlag;

  processCommandLine (argc, argv,
		      &rowsPerChunk, &columnsPerChunk,
		      &nRowsOfChunks, &nColumnsOfChunks,
		      &nThreads, &nIterations, &printFlag);


  hid_t file, space, dset, dcpl;	/* Handles */
  herr_t status;
  H5D_layout_t layout;
  int nArrayRows = nRowsOfChunks * rowsPerChunk;
  int nArrayColumns = nColumnsOfChunks * columnsPerChunk;
  int nChunks = nRowsOfChunks * nColumnsOfChunks;
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
  clock_t hdfSpan = readHDFByHyperslab (FILENAME, rdata, rowsPerChunk,
					columnsPerChunk, nRowsOfChunks,
					nColumnsOfChunks, nIterations,
					printFlag);

  printf ("HDF time to read = %ld\n\n", hdfSpan);
  hsize_t **allChunkOffsets;
  hsize_t *chunkSizeInBytes;
  hsize_t maxChunkSize;

  getChunkInfo (FILENAME, DATASET, rowsPerChunk, columnsPerChunk,
		nRowsOfChunks, nColumnsOfChunks,
		&chunkSizeInBytes, &allChunkOffsets, &maxChunkSize, printFlag);


  clock_t directSpan = readDirectly (FILENAME,
				     rdata,
				     rowsPerChunk, columnsPerChunk,
				     nRowsOfChunks, nColumnsOfChunks,
				     chunkSizeInBytes,
				     allChunkOffsets,
				     nIterations, printFlag);


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
						maxChunkSize, nThreads,
						nIterations, printFlag);

  printf ("Thread time is %ld\n\n", threadSpan);
  printf ("HDF/Direct is %lf\n", ((double) hdfSpan) / (double) directSpan);
  printf ("HDF/Thread is %lf\n", ((double) hdfSpan) / (double) threadSpan);

  // Deallocate all dynamically allocated memory.

  dealloc2dArray ((void **) wdata, nArrayRows);
  dealloc2dArray ((void **) rdata, nArrayRows);
  dealloc2dArray ((void **) allChunkOffsets, nChunks);
  return 0;
}
