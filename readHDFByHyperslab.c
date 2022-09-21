#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "hdf5.h"
#include "printFunctions.h"

#define N_DIMENSIONS 2

clock_t readHDFByHyperslab (const char *filename, const char *dataset,
			    int **rdata,
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

  int nArrayRows = rowsPerChunk * nRowsOfChunks;
  int nArrayColumns = columnsPerChunk * nColumnsOfChunks;

  /* 
   * Open file and dataset using the default properties.
   */
  file = H5Fopen (filename, H5F_ACC_RDONLY, H5P_DEFAULT);

  dset = H5Dopen (file, dataset, H5P_DEFAULT);

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
