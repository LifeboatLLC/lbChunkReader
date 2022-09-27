#include <stdio.h>
#include "hdf5.h"

#define N_DIMENSIONS 2

// Write an hdf file representing an int 2d array.
// The file is written in chunks.

void writeHdfFile (const char *filename, const char *datasetName,
		   int **wdata,
		   int rowsPerChunk, int columnsPerChunk,
		   int nRowsOfChunks, int nColumnsOfChunks, int printFlag)
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

  start[0] = 0;
  start[1] = 0;
  stride[0] = rowsPerChunk;
  stride[1] = columnsPerChunk;
  count[0] = nRowsOfChunks;
  count[1] = nColumnsOfChunks;
  block[0] = rowsPerChunk;
  block[1] = columnsPerChunk;
  dims[0] = nArrayRows;
  dims[1] = nArrayColumns;
  chunk[0] = rowsPerChunk;
  chunk[1] = columnsPerChunk;

  /* 
   * Initialize data to row*nColumns+column, to make it easier to see the selections.
   */
  int i, j;
  for (i = 0; i < nArrayRows; i++)
    for (j = 0; j < nArrayColumns; j++)
      wdata[i][j] = i * nArrayColumns + j;

  /* 
   * Create a new file using the default properties.
   */
  file = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  /* 
   * Create dataspace.  Setting maximum size to NULL sets the maximum
   * size to be the current size.
   */
  space = H5Screate_simple (N_DIMENSIONS, dims, NULL);

  /* 
   * Create the dataset creation property list, and set the chunk
   * size.
   */
  // Create a dataset property list
  dcpl = H5Pcreate (H5P_DATASET_CREATE);
  // Set the chunk size (propertyListId, # of dims in chunk, chunkDims);
  status = H5Pset_chunk (dcpl, N_DIMENSIONS, chunk);

  /* 
   * Create the chunked dataset.
   */
  // H5Dcreate(file, datasetName, datatype, dataspaceId, 
  dset = H5Dcreate (file, datasetName, H5T_STD_I32LE, space, H5P_DEFAULT, dcpl,
		    H5P_DEFAULT);

  status = H5Sselect_hyperslab (space, H5S_SELECT_SET, start, stride, count,
				block);

  /* 
   * Write the data to the dataset.
   */
  status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, space, H5P_DEFAULT,
		     wdata[0]);

  /* 
   * Close and release resources.
   */
  status = H5Pclose (dcpl);
  status = H5Dclose (dset);
  status = H5Sclose (space);
  status = H5Fclose (file);
}
