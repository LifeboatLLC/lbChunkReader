#ifndef WRITE_HDF_FILE_H
#define WRITE_HDF_FILE_H
void writeHdfFile (const char *filename, const char* datasetName,
                   int **wdata,
		   int rowsPerChunk, int columnsPerChunk,
		   int nRowsOfChunks, int nColumnsOfChunks, int printFlag);
#endif
