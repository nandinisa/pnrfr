#ifndef PNRF_fileProcessor_h
#define PNRF_fileProcessor_h

#include <vector>

namespace pnrfr {
	class IFileProcessor
	{
	public:
		virtual void loadChannel(const int channel, std::vector<std::pair<double, double>>& val) = 0;
		virtual void loadChannelSeg(const int channel, const int segment, std::vector<std::pair<double, double>>& val) = 0;
		
		virtual void loadChannelWithSampling(const int channel, const int samplingRate, std::vector<std::pair<double, double>>& val) = 0;
		virtual void loadChannelSegWithSampling(const int channel, const int samplingRate, const int segment, std::vector<std::pair<double, double>>& val) = 0;
		
		virtual void getMovingAvg(std::vector<std::pair<double, double>>& data, std::vector<std::pair<double, double>>& movAvg, const int windowsize) = 0;
		virtual void getMovingAvgWithSampling(std::vector<std::pair<double, double>>& data, std::vector<std::pair<double, double>>& movAvg, const int windowsize, const int samplingRate) = 0;
	};
}

#endif