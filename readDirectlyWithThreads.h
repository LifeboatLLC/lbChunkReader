#ifndef READ_DIRECTLY_WITH_THREADS_H
#define READ_DIRECTLY_WITH_THREADS_H

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
				 int nThreads, int nIterations, int printFlag);

#endif
