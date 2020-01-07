#ifndef PNRF_pnrfhelper_h
#define PNRF_pnrfhelper_h

#include "ioffline.h"
#include "filemeta.h"
#include "fileprocessor.h"

// #import "RecordingInterface.olb" no_namespace
#import "libid:8098371E-98AD-0070-BEF3-21B9A51D6B3E" rename_namespace("pnrf_com_interface")
// #import "percPNRFLoader.dll" no_namespace
#import "libid:8098371E-98AD-0062-BEF3-21B9A51D6B3E" rename_namespace("pnrf_com_interface")

namespace pnrfr {
	enum DataType { Continuous = 1, Sweeps, All};

	class PnrfHelper :IOffline, IFileMeta, IFileProcessor {
	private:
		pnrf_com_interface::IRecordingLoaderPtr loaderPtr = nullptr;
		pnrf_com_interface::IRecordingPtr recorderPtr = nullptr;
		//ComRuntime runtime;
		pnrf_com_interface::DataSourceSelect dtype;

		void getChannelData(const int channel, CComVariant &data);
		void getChannelDataVector(const CComVariant &data, std::vector<std::pair<double, double>>& val, const int segment, const int samplingRate);
		void saveChannelDataVector(const CComVariant &data, const std::string filePath, const int segment = -1, const int samplingRate = -1);

	public:
		//PnrfHelper(const char* filePath);
		PnrfHelper(const std::string filePath, DataType load_type = All);

		/* Ioffline interface */
		void saveChannelToFile(const int channel, const std::string filePath);
		void saveChannelDataToFile(std::vector<std::pair<double, double>> &val, const std::string filePath);
		void saveChannelWithSamplingToFile(const int channel, const int samplingRate, const std::string filePath);
		void saveAllDataToFile(std::vector<std::pair<double, double>> &val, std::vector<std::pair<double, double>> &movAvg, const std::string filePath);

		/* Ifilemeta interface */
		Meta getFileMetaInfo();
		int getChannelSize(const int channel, const int samplingRate = -1);
		int getSegmentSize(const int channel, const int segment, const int samplingRate = -1);
		int getWindowSize(const int channel, const double windowTime);

		/* Ifileprocessor interface */
		void loadChannel(const int channel, std::vector<std::pair<double, double>>& val);
		void loadChannelSeg(const int channel, const int segment, std::vector<std::pair<double, double>>& val);

		void loadChannelWithSampling(const int channel, const int samplingRate, std::vector<std::pair<double, double>>& val);
		void loadChannelSegWithSampling(const int channel, const int segment, const int samplingRate, std::vector<std::pair<double, double>>& val);

		void getMovingAvg(std::vector<std::pair<double, double>>& data,
			std::vector<std::pair<double, double>>& movAvg, const int windowSize);
		void getMovingAvgWithSampling(std::vector<std::pair<double, double>>& data, 
			std::vector<std::pair<double, double>>& movAvg, const int windowSize,
			const int samplingRate);

		/* Move */
		PnrfHelper(PnrfHelper&& helper);
		PnrfHelper& operator=(PnrfHelper&& helper);
	};
}

#endif