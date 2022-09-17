CC=gcc
CFLAGS=-I. -I$(HDF5_INCLUDE_DIRS) 
LIBS=-lpthread -lhdf5 -L$(HDF5_LIBRARIES)
OBJ = lbChunkReader.o workerParameters.o printFunctions.o allocate2D.o getChunkInfo.o readDirectly.o writeHdfFile.o \
      readDirectlyWithThreads.o worker.o
DEPS = workerParameters.h printFunctions.h allocate2D.h getChunkInfo.h readDirectly.h writeHdfFile.h \
      readDirectlyWithThreads.h worker.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

lbChunkReader: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm *.o lbChunkReader

