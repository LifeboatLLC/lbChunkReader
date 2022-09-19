#ifndef READ_HDF_BY_HYPERSLAB_H
#define READ_HDF_BY_HYPERSLAB_H
clock_t readHDFByHyperslab(const char* filename, const char* dataset,
                            int **rdata,
			    int rowsPerChunk, int columnsPerChunk,
			    int nRowsOfChunks, int nColumnsOfChunks,
			    int nIterations, int printFlag);
#endif

