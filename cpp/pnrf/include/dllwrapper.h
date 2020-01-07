#ifndef PNRF_dllwrapper_h
#define PNRF_dllwrapper_h

// Exported R functions
ComRuntime runtime;

// Directory processing
void processDirectory(const char** dirPath,
	const char** fileArray, unsigned* fileArraySize,
	int* channel, unsigned short* dataType,
	bool* sampling, int* samplingRate,
	bool* computeMovAvg, double *windowTime,
	const char** savePath,
	char** msg, int* out);

// File meta
void readFileMeta(const char** filePath, char**title, int* recordCount,
	char** msg, int* out);
void readFileRecordMeta(const char** filePath, int* recordArray, unsigned short* recordArraySize, int* channelCountArray,
	char** msg, int* out);
void readFileChannelMeta(const char** filePath, int* record, int* channel, unsigned short* dataType,
	char** name, char** type, char** yunit, double* startTime, double* endTime,
	int* noOfSamples, double* sampleInterval, int* segCount,
	char** msg, int* out);

// Channel related
void getChannelDataSize(const char** filePath, int* channel, unsigned short* dataType, unsigned* size,
	bool* sampling, int* samplingRate,
	char** msg, int* out);
void readChannelData(const char** filePath, int* channel, unsigned short* dataType, double* timeArray, double* dataArray,
	bool* sampling, int* samplingRate,
	char** msg, int* out);
void saveChannelData(const char** filePath, int* channel, unsigned short* dataType, const char** savePath,
	bool* sampling, int* samplingRate,
	char** msg, int* out);

// Segment related
void getSegmentDataSize(const char** filePath, int* channel, int* segment, unsigned short* dataType, unsigned* size,
	bool* sampling, int* samplingRate,
	char** msg, int* out);
void readSegmentData(const char** filePath, int* channel, int* segment, unsigned short* dataType,
	double* timeArray, double* dataArray,
	bool* sampling, int* samplingRate,
	char** msg, int* out);

// Computations
void computeMovingAverage(const char** filePath, int* channel, unsigned short* dataType, 
	double* timeArray, double* dataArray, double* windowTime,
	bool* sampling, int* samplingRate,
	char** msg, int* out);

#endif 
