#include "stdafx.h"			// standard imports
#include <filesystem>
#include "dllwrapper.h"
#include "pnrfhelper.h"
#include <iterator>
#include <future>

using std::async;
using std::begin;
using std::end;
using std::exception;
using std::ifstream;
using std::pair;
using std::string;
using std::vector;
using std::wstring;

using std::filesystem::path;
using std::filesystem::recursive_directory_iterator;


// Internal methods
// Do not use this, as check should ideally be performed in R.
// If there is a thread lock, inaccessible, throw error
bool fileCheck(path filePath, char** &msg) {
	ifstream file(filePath);
	if (!file)
	{
		// Can't open file
		*msg = (char *)"File not found";
		return false;
	}

	return true;
}

// internal method for string to char* conversion
// note: char array should have been already allocated
void convertStringToAPICharArray(char** &str, string val) {
	copy(val.begin(), val.end(), *str);
	*(*str + val.size()) = '\0';
}

//https://stackoverflow.com/questions/3561659/how-can-i-abstract-out-a-repeating-try-catch-pattern-in-c
/// Internal method to handle the error, handles both COM and generic error
/**
* @param 
*    msg - input from API, msg is set to error message
*	 out - is set to -1 
* @return
*    Brief description of return value
* @throw
*    Rethrows the error after setting the message and out value
*/
void handle(char**& msg, int* &out) {
	try {
		throw;
	}
	catch (const _com_error& e) {
		out[0] = -1;
		pnrfr::PnrfError::setComError(e, msg);
		//throw;
	}
	catch (const exception& e) {
		out[0] = -1;
		pnrfr::PnrfError::setError(e, msg);
		//throw;
	}
	catch(...) {
		out[0] = -1;
		pnrfr::PnrfError::setUnknownError(msg);
	}
}

// Process file
void processFile(const path filePath, const string& savePath,
	const int channel, unsigned short dataType,
	const bool sampling, const int samplingRate,
	const bool computeMovAvg, const double windowTime) {
	vector<pair<double, double>> data, movAvg;
	auto wrapper = pnrfr::PnrfHelper(filePath.string(), (pnrfr::DataType)dataType);
	auto filename = filePath.has_stem() ? filePath.stem().string() : filePath.filename().string();
	auto saveFile = path(savePath).append(filename + ".csv");
	wrapper.loadChannel(channel, data);
	
	//todo compute mov avg with bool check
	if (computeMovAvg) {
		int windowSize = wrapper.getWindowSize(channel, windowTime);
		wrapper.getMovingAvgWithSampling(data, movAvg, windowSize, samplingRate);
		auto savePFile = path(savePath).append(filename + "_rms.csv");
		wrapper.saveAllDataToFile(data, movAvg, savePFile.string());
	}
	else {
		wrapper.saveChannelDataToFile(data, saveFile.string());
	}
}



// Exported methods

/// Processes the PNRF directory
/**
* Input parameters
* @param dirPath
*    Pointer to the directory path containing the PNRF files
* @param files
*    Array pointer to the list of file names within directory to read. If not specified, 
*    then reads all the files in the given directory
* @param computeMovAvg
*    Boolean pointer indicating if moving avg should be computed, default - will not be computed
*
* Parameters set by the function
* @param msg
*    If error occurs (i.e. out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException, Exception
*/
void processDirectory(const char** dirPath, 
	const char** fileArray, unsigned* fileArraySize, 
	int* channel, unsigned short* dataType,
	bool* sampling, int* samplingRate,
	bool* computeMovAvg, double *windowTime, 
	const char** savePath,
	char** msg, int* out) {
	try {
		auto dir = string(dirPath[0]);
		vector<string> files;
		if (*fileArraySize == 0) {
			// Read all files in directory
			for (auto& p : recursive_directory_iterator(path(dir))){
				if (p.path().extension() == ".pnrf") {
					auto filePath = p.path();
					auto result = std::async(std::launch::async, processFile, filePath, string(savePath[0]),
						*channel, *dataType,
						*sampling, *samplingRate,
						*computeMovAvg, *windowTime);

					result.get();

					/*std::thread([&]() { 
						processFile(filePath, string(savePath[0]),
						*channel, *dataType,
						*sampling, *samplingRate,
						*computeMovAvg, *windowTime); }).detach();*/
					
					//files.emplace_back(p.path().string());
				}
			}
		}
		else {
			for (unsigned i = 0; i < *fileArraySize; i++) {
				//files.emplace_back(string(dirPath[0]) + string(fileArray[i]));
				auto filename = string(fileArray[i]);
				auto filePath = path(dir).append(begin(filename), end(filename));
				if (fileCheck(filePath, msg)) {
					auto result = std::async(std::launch::async, processFile, filePath, string(savePath[0]), 
						*channel, *dataType,
						*sampling, *samplingRate,
						*computeMovAvg, *windowTime);
					result.get();
					/*readDirectory(filePath, string(savePath[0]), *channel, *dataType,
						*sampling, *samplingRate,
						*computeMovAvg, *windowTime);*/
				}
				
			}
		}
		
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Reads the PNRF file meta data
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param title
*    Pointer containing the title of the file, the method populates this value
* @param recordCount
*   Pointer to the number of records within the file, the method populates this value
*
* Parameters set by the function
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException, Exception
*/
void readFileMeta(const char** filePath, char**title, int* recordCount,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0]);
		pnrfr::Meta meta = wrapper.getFileMetaInfo();
		convertStringToAPICharArray(title, meta.getTitle());
		*recordCount = meta.getRecordCount();
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Get the no of channels in the PNRF file
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param recordArray
*    Pointer to the array of record numbers within the specified file
* @param recordCount
*   Pointer to the length of records, as by default cannot calculate size of array
*
* Parameters set by the function
* @param channelCountArray
*   Pointer to the number of channels within the specified record array, the method populates this value
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void readFileRecordMeta(const char** filePath, int* recordArray, unsigned short* recordArraySize, int* channelCountArray,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0]);
		pnrfr::Meta meta = wrapper.getFileMetaInfo();
		//auto recordCount = meta.getRecordCount();
		auto i = 0;
		while (i < *recordArraySize) {
			auto rec = meta.getRecord(recordArray[i]);
			channelCountArray[i] = rec->getChannelCount();
			i++;
		}

		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Retrieve the PNRF file's channel details
/**
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param record
*    Pointer to the record number within the specified file
* @param channel
*   Pointer to the channel number within the specified record
* @param name
*   Pointer to channel name, the method populates this
* @param type
*   Pointer to channel type, the method populates this
* @param startTime
*   Pointer to channel startTime, the method populates this
* @param endTime
*   Pointer to channel endTime, the method populates this
* @param noOfSamples
*   Pointer to channel noOfSamples, the method populates this
* @param sampleInterval
*   Pointer to channel sampleInterval, the method populates this
* @param segCount
*   Pointer to segCount name, the method populates this
* @param msg
*    If error occurs (out ==1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void readFileChannelMeta(const char** filePath, int* record, int * channel, unsigned short* dataType,
	char** name, char** type, char** yunit, double* startTime, double* endTime,
	int* noOfSamples, double* sampleInterval, int* segCount,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		//auto wrapper = make_unique<pnrfr::PnrfHelper>(filePath[0], (pnrfr::DataType)*dataType);
		auto meta = wrapper.getFileMetaInfo();
		pnrfr::Record* record_details = nullptr;
		if (*record == 0) {
			int ch = 0;
			// If record not specified, 
			//traverse through each record to determine which record holds the specified channel 
			for (int i = 1; i <= meta.getRecordCount(); i++) {
				*channel -= ch;
				record_details = meta.getRecord(i);
				ch = record_details->getChannelCount();
				if (ch >= *channel) {
					break;
				}
			}
		}
		else {
			record_details = meta.getRecord(*record);
		}

		record_details = meta.getRecord(1);
		auto channel_details = record_details->getChannel(*channel);

		convertStringToAPICharArray(name, channel_details->getName());
		convertStringToAPICharArray(type, channel_details->getType());
		convertStringToAPICharArray(yunit, channel_details->getYUnit());
		*startTime = channel_details->getStartTime();
		*endTime = channel_details->getEndTime();
		*noOfSamples = channel_details->getNoOfSamples();
		*sampleInterval = channel_details->getSampleInterval();
		*segCount = channel_details->getSegmentCount();
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Gets the channel data size (including all segments)
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*    Pointer containing the channel number
*
* Parameters set by the function
* @param size
*   Pointer to the channel data size, the method populates this value
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void getChannelDataSize(const char** filePath, int* channel, unsigned short* dataType, unsigned* size, 
	bool* sampling, int* samplingRate,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		if (*sampling & (*samplingRate > 1)) {
			*size = wrapper.getChannelSize(*channel, *samplingRate);
		}
		else {
			*size = wrapper.getChannelSize(*channel);
		}
		
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Reads the PNRF channel data
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*    Pointer containing the channel number
*
* Parameters set by the channel
* @param dataArray
*   Pointer to the data array within the channel, the method populates this value
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void readChannelData(const char** filePath, int* channel, unsigned short* dataType, double* timeArray, double* dataArray,
	bool* sampling, int* samplingRate,
	char** msg, int* out) {
	try {
		vector<pair<double,double>> val;
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		if (*sampling & (*samplingRate > 1)) {
			wrapper.loadChannelWithSampling(*channel, *samplingRate, val);
		}
		else {
			wrapper.loadChannel(*channel, val);
		}
		

		int i = 0;
		for (auto& elem : val) {
			timeArray[i] = elem.first;
			dataArray[i] = elem.second;
			i++;
		}
		
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Gets the PNRF segment data size
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*    Pointer containing the channel number
* @param segment
*    Pointer containing the segment number
*
* Parameters set by the function
* @param size
*   Pointer to the segment data size, the method populates this value
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void getSegmentDataSize(const char** filePath, int* channel, int* segment, unsigned short* dataType, unsigned* size,
	bool* sampling, int* samplingRate,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		if (*sampling & (*samplingRate > 1)) {
			*size = wrapper.getSegmentSize(*channel, *segment, *samplingRate);
		}
		else {
			*size = wrapper.getSegmentSize(*channel, *segment);
		}
		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}

/// Reads the PNRF segment data
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*    Pointer containing the channel number
* @param segnment
*   Pointer to the segment data to retrieve
* 
* Parameters set by the function
* @param dataArray
*   Pointer to the data array within the specified channel and segment, the method populates this value
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void readSegmentData(const char** filePath, int* channel, int* segment, unsigned short* dataType,
	double* timeArray, double* dataArray,
	bool* sampling, int* samplingRate,
	char** msg, int* out) {
	try {
		vector<pair<double, double>> val;
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		if (*sampling & (*samplingRate > 1)) {
			wrapper.loadChannelSegWithSampling(*channel, *segment, *samplingRate, val);
		}
		else {
			wrapper.loadChannelSeg(*channel, *segment, val);
		}

		int i = 0;
		for (auto& elem : val) {
			timeArray[i] = elem.first;
			dataArray[i] = elem.second;
			i++;
		}

		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}



/// Reads the PNRF specified channel, and saves it as a csv file
/**
* Input parameters
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*    Pointer containing the channel number
* @param savePath
*   Pointer containing the save to file location
*
* Parameters set by the function
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void saveChannelData(const char** filePath, int* channel, unsigned short* dataType, const char** savePath, 
	bool* sampling, int* samplingRate,
	char** msg, int* out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		if (*sampling & (*samplingRate > 1)) {
			wrapper.saveChannelWithSamplingToFile(*channel, *samplingRate, savePath[0]);
		}
		else {
			wrapper.saveChannelToFile(*channel, savePath[0]);
		}

		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}



/// Compute moving average of the PNRF channel data
/// Computes RMS value first and then down samples the data
/**
* Input parameter
* @param filePath
*    Pointer to the PNRF file path, filepath = "C:\\Temp\\VT609.pnrf"
* @param channel
*   Pointer to the channel number 
* @param windowTime
*   Pointer to sampling window time interval
* 
* Parameters set by the function
* @param dataArray
*   Pointer to the dataArray of moving average, the method populates this
* @param msg
*    If error occurs (out == 1), then the msg pointer is set with the corresponding error message
* @param out
*    This pointer indicates the success status code of the method. 0 - success, 1 - error
* @return
*    None
* @throw
*   COMException
*/
void computeMovingAverage(const char** filePath, int* channel, unsigned short* dataType, 
	double* timeArray, double* dataArray, double* windowTime,
	bool* sampling, int* samplingRate,
	char** msg, int *out) {
	try {
		auto wrapper = pnrfr::PnrfHelper(filePath[0], (pnrfr::DataType)*dataType);
		vector<pair<double, double>> val, movAvg;
		wrapper.loadChannel(*channel, val);

		int windowSize = wrapper.getWindowSize(*channel, *windowTime);
		if ((val.size() > 0) & (windowSize > 0)) {
			wrapper.getMovingAvg(val, movAvg, windowSize);
			int i = 0, idx = 0;
			if ((*sampling == true) & (*samplingRate > 1)) {
				while (idx < movAvg.size()) {
					const int sampleSize = (idx + *samplingRate) > movAvg.size() ? (movAvg.size() - idx) : *samplingRate;
					double avg = 0;
					for (int j = 0; j < sampleSize; j++)
					{
						avg += movAvg[j + idx].second;
					}

					timeArray[i] = movAvg[idx].first;
					dataArray[i] = (avg / sampleSize);
					idx = idx + sampleSize;
					i++;
				}
			}
			else {
				for (; i < movAvg.size(); i++) {
					timeArray[i] = movAvg[i].first;
					dataArray[i] = movAvg[i].second;
				}
			}
			
			/*for (auto& elem : movAvg) {
				timeArray[i++] = elem.first;
				dataArray[i++] = elem.second;
			}*/
		}

		out[0] = 0;
	}
	catch (...) {
		handle(msg, out);
	}
}
