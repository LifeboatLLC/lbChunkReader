#ifndef GETCHUNKINFO_H
#define GETCHUNKINFO_H
void getChunkInfo (const char *filename,
		   const char *datasetName,
		   int rowsPerChunk, int columnsPerChunk,
		   int nRowsOfChunks, int nColumnsOfChunks,
		   haddr_t * chunkSizeInBytes,
		   hsize_t ** allChunkOffsets, hsize_t * maxChunkSize,
		   int printFlag);
#endif
