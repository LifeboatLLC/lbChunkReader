#ifndef READ_DIRECTLY_H
#define READ_DIRECTLY_H

// Read an HDF file using direct reads, rather than using 
// the HDF library.
clock_t readDirectly (const char *filename, int **rdata,
		      int rowsPerChunk, int columnsPerChunk,
		      int nRowsOfChunks, int nColumnsOfChunks,
		      hsize_t * chunkSizeInBytes,
		      hsize_t ** chunkLocationInFile,
		      int nIterations, int printFlag);
#endif
