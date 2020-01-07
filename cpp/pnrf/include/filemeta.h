#ifndef PNRF_filemeta_h
#define PNRF_filemeta_h

#include <string>
#include <vector>

namespace pnrfr {
	class Channel {
	private:
		std::string name;
		std::string type;
		std::string yunit;
		double startTime;
		double endTime;
		int segmentCount;
		int noOfSamples;
		double sampleInterval;
	public:
		Channel(std::string name, std::string type, std::string yunit, double starttime, double endtime,
			int segmentcount, int noofsamples, double sampleinterval) :name(name), type(type), yunit(yunit),
			startTime(starttime), endTime(endtime), segmentCount(segmentcount), noOfSamples(noofsamples),
			sampleInterval(sampleinterval) {};
		Channel() {};
		~Channel() {};

		void assignChannelDetails(std::string cname, std::string ctype, std::string cyunit, double cstarttime, double cendtime);
		void assignSegmentDetails(int segmentcount, int noofsamples, double sampleinterval);

		std::string getName() const;
		std::string getType() const;
		std::string getYUnit() const;
		double getStartTime() const;
		double getEndTime() const;
		int getSegmentCount() const;
		int getNoOfSamples() const;
		double getSampleInterval() const;
	};

	class Record {
	private:
		std::vector<Channel> channels;
	public:
		Record() {};
		~Record() {};

		void addChannel(Channel channel);
		Channel* getChannel(int no);
		int getChannelCount() const;
	};

	class Meta {
	private:
		std::string title;
		std::vector<Record> records;
	public:
		Meta(std::string title) :title(title) {};
		~Meta() {};

		void addRecord(Record record);
		Record* getRecord(int no);
		int getRecordCount() const;
		std::string getTitle() const;
	};

	class IFileMeta {
	public:
		virtual Meta getFileMetaInfo() = 0;
		virtual int getChannelSize(const int channel, const int samplingRate = -1) = 0;
		virtual int getSegmentSize(const int channel, const int segment, const int samplingRate = -1) = 0;
		virtual int getWindowSize(const int channel, const double windowTime) = 0;
	};

}
#endif