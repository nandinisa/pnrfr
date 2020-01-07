#ifndef PNRF_pnrfioffline_h
#define PNRF_pnrfioffline_h

#include <vector>
#include <string>

namespace pnrfr {
	class IOffline {
	public:
		virtual void saveChannelToFile(const int channel, const std::string filePath) = 0;
		virtual void saveChannelWithSamplingToFile(const int channel, const int samplingRate, const std::string filePath) = 0;
		virtual void saveChannelDataToFile(std::vector<std::pair<double, double>> &val, const std::string filePath) = 0;
	};
}
#endif



