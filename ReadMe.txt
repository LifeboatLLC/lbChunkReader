lbChunkReader <filename> <rowsPerChunk> <columnPerChunk> 
                         <nRowsOfChunks> <nColumnsOfChunks> 
			 <nThreads> <copyToArray> <nIterations>
			 <printDataFlag>
			  
lbChunkReader reads chunks into a 2-dimensional array using 3 different methods:
	1.) HDF5 hyperslab reads,
	2.) Direct disk reads using information extracted via H5Dget_chunk_info,
	3.) Separate threads using direct disk reads from information extracted 
            via H5Dget_chunk_info.  In this mode each thread is responsible for
            reading a subset of the chunks. 

If <filename> does not exist, the program writes the file with the specified parameters.  
Otherwise, the program uses the existing file.  In either case, the file data 
represents the data in a 2-dimensional array written in chunks.

<rowsPerChunK> is the number of array rows in a single chunk.
<columnsPerChunk> is the number of array columns in a single chunk.
<nRowsOfChunks> is the number of chunks in an array column.
<nColumnsOfChunks> is the number of chunks in an array row.
<nThreads> is the number of threads to employ for threaded reads.
<copyToArray> if true, copies the chunked read to the corresponing cells in the array.
<nIterations> is the number of times the entire of collection of chunks should be read.
<printDataFlag> if true, prints data to the screen.  Typically this flag should be 0, unless
                the data set is small enough to fit on the screen.

Example1:  lbChunkReader example1.h5 2 3 4 5 2 1 10000 1 

produces the following:

Filename is example1.h5
Rows per chunk = 2
Columns per chunk = 3
Number of rows of chunks = 4
Number of columns of chunks = 5
Number of threads = 2
Copy to array = 1
Number of iterations = 100000
nArrayRows = 8
nArrayColumns = 15
Writing new HDF5 file example1.h5.

Data as written to disk by hyperslabs:
 [   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14]
 [  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29]
 [  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44]
 [  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59]
 [  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74]
 [  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89]
 [  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104]
 [ 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119]

Reading with hdf5

Data as read from disk by hyperslab:
 [   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14]
 [  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29]
 [  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44]
 [  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59]
 [  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74]
 [  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89]
 [  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104]
 [ 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119]

HDF time to read = 2296078

Extracted HDF5 chunk info: 
	chunkRow = 0: chunkColumn =0: index = 0: nBytes 24: address: 4016
	chunkRow = 0: chunkColumn =1: index = 1: nBytes 24: address: 4040
	chunkRow = 0: chunkColumn =2: index = 2: nBytes 24: address: 4064
	chunkRow = 0: chunkColumn =3: index = 3: nBytes 24: address: 4088
	chunkRow = 0: chunkColumn =4: index = 4: nBytes 24: address: 4112
	chunkRow = 1: chunkColumn =0: index = 5: nBytes 24: address: 4136
	chunkRow = 1: chunkColumn =1: index = 6: nBytes 24: address: 4160
	chunkRow = 1: chunkColumn =2: index = 7: nBytes 24: address: 4184
	chunkRow = 1: chunkColumn =3: index = 8: nBytes 24: address: 4208
	chunkRow = 1: chunkColumn =4: index = 9: nBytes 24: address: 4232
	chunkRow = 2: chunkColumn =0: index = 10: nBytes 24: address: 4256
	chunkRow = 2: chunkColumn =1: index = 11: nBytes 24: address: 4280
	chunkRow = 2: chunkColumn =2: index = 12: nBytes 24: address: 4304
	chunkRow = 2: chunkColumn =3: index = 13: nBytes 24: address: 4328
	chunkRow = 2: chunkColumn =4: index = 14: nBytes 24: address: 4352
	chunkRow = 3: chunkColumn =0: index = 15: nBytes 24: address: 4376
	chunkRow = 3: chunkColumn =1: index = 16: nBytes 24: address: 4400
	chunkRow = 3: chunkColumn =2: index = 17: nBytes 24: address: 4424
	chunkRow = 3: chunkColumn =3: index = 18: nBytes 24: address: 4448
	chunkRow = 3: chunkColumn =4: index = 19: nBytes 24: address: 4472
	chunkRow = 0: chunkColumn =0: index = 1: nBytes 24: address: 4016
	chunkRow = 0: chunkColumn =1: index = 2: nBytes 24: address: 4040
	chunkRow = 0: chunkColumn =2: index = 3: nBytes 24: address: 4064
	chunkRow = 0: chunkColumn =3: index = 4: nBytes 24: address: 4088
	chunkRow = 0: chunkColumn =4: index = 5: nBytes 24: address: 4112
	chunkRow = 1: chunkColumn =0: index = 6: nBytes 24: address: 4136
	chunkRow = 1: chunkColumn =1: index = 7: nBytes 24: address: 4160
	chunkRow = 1: chunkColumn =2: index = 8: nBytes 24: address: 4184
	chunkRow = 1: chunkColumn =3: index = 9: nBytes 24: address: 4208
	chunkRow = 1: chunkColumn =4: index = 10: nBytes 24: address: 4232
	chunkRow = 2: chunkColumn =0: index = 11: nBytes 24: address: 4256
	chunkRow = 2: chunkColumn =1: index = 12: nBytes 24: address: 4280
	chunkRow = 2: chunkColumn =2: index = 13: nBytes 24: address: 4304
	chunkRow = 2: chunkColumn =3: index = 14: nBytes 24: address: 4328
	chunkRow = 2: chunkColumn =4: index = 15: nBytes 24: address: 4352
	chunkRow = 3: chunkColumn =0: index = 16: nBytes 24: address: 4376
	chunkRow = 3: chunkColumn =1: index = 17: nBytes 24: address: 4400
	chunkRow = 3: chunkColumn =2: index = 18: nBytes 24: address: 4424
	chunkRow = 3: chunkColumn =3: index = 19: nBytes 24: address: 4448
	chunkRow = 3: chunkColumn =4: index = 20: nBytes 24: address: 4472

Reading direct
Direct time = 1093097


Data as read using direct read:
 [   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14]
 [  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29]
 [  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44]
 [  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59]
 [  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74]
 [  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89]
 [  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104]
 [ 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119]

Reading direct with threads

Data as read threaded direct read:
 [   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14]
 [  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29]
 [  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44]
 [  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59]
 [  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74]
 [  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89]
 [  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104]
 [ 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119]

Thread time is 1620504

HDF/Direct is 2.100525
HDF/Thread is 1.416891
