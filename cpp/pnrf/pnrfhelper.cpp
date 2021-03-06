#include "stdafx.h"			// standard imports
#include "pnrfhelper.h"
#include <math.h>

using std::exception;
using std::ofstream;
using std::pair;
using std::string;
using std::vector;
using std::ceil;
using namespace pnrf_com_interface;

namespace pnrfr {

	/* Start of private variables and methods */

	void PnrfHelper::getChannelData(const int channel, CComVariant &data) {
		double dStart, dEnd;

		auto chCount = recorderPtr->Channels->Count;
		if (channel < 1 || channel > chCount) {
			throw exception("No channels found");
			return;
		}

		// connect to data source channel 1
		IDataSrcPtr dataSource = recorderPtr->Channels->Item[channel]->DataSource[this->dtype];

		// fetch start and stop time
		dataSource->Sweeps->get_StartTime(&dStart);
		dataSource->Sweeps->get_EndTime(&dEnd);

		// Get data between start and stop time
		dataSource->Data(dStart, dEnd, &data);
	}

	void PnrfHelper::getChannelDataVector(const CComVariant &data, vector<pair<double, double>>& val, 
		const int segment = -1,
		const int samplingRate = -1) {
		if (data.vt != VT_EMPTY)
		{
			// create segments pointer
			IDataSegmentsPtr itfSegments = data.punkVal;
			int iSegIndex = 1;					// segment index
			int iCount = itfSegments->Count;	// segemnt count
			if (iCount < 1)
			{
				throw exception("No Segments found");
				return;
			}

			if (segment > 0) {
				iSegIndex = segment;
				iCount = segment;
			}

			auto totalSize = [=]() mutable {
				int size = 0;
				while (iSegIndex <= iCount)
				{
					// pointer inside segment data
					IDataSegmentPtr itfSegment = NULL;
					itfSegments->get_Item(iSegIndex, &itfSegment);

					size += itfSegment->NumberOfSamples;
					iSegIndex++;
				}
				return size;
			};

			val.clear();
			val.reserve(totalSize());

			// loop through all available segments
			while (iSegIndex <= iCount)
			{
				// pointer inside segment data
				IDataSegmentPtr itfSegment = NULL;
				itfSegments->get_Item(iSegIndex, &itfSegment);

				int lCnt = itfSegment->NumberOfSamples;

				// variant data array for segment data
				CComVariant varData;
				// fetch data
				itfSegment->Waveform(DataSourceResultType_Double64, 1, lCnt, 1, &varData);

				//If there is no data, process next segment
				if (varData.vt == VT_EMPTY)
					continue;

				//If it isn't an array, something is wrong here
				if (!(varData.vt & VT_ARRAY))
					continue;

				//Get data out through the use of the safe array and store locally
				SAFEARRAY* satmp = NULL;
				satmp = varData.parray;

				if (satmp->cDims > 1)
				{
					// It's a multi dimensional array
					continue;
				}


				double *pData;
				SafeArrayAccessData(satmp, (void**)&pData);
				double X0 = itfSegment->StartTime;
				double DeltaX = itfSegment->SampleInterval;
				int size = (int)satmp->rgsabound[0].cElements;

				if (samplingRate <= 0) {
					for (int i = 0; i < size; i++)
					{
						val.emplace_back((X0 + i * DeltaX), pData[i]);
					}
				}
				else {
					int i = 0;
					while (i < size) {
						const double sampleSize = (i + samplingRate) > size ? (size - i) : samplingRate;
						double avg = 0;
						for (int j = 0; j < sampleSize; j++)
						{
							avg += pData[j + i];
						}

						val.emplace_back((X0 + i * DeltaX), (avg/ sampleSize));
						i = i + sampleSize;
					}
				}
				

				SafeArrayUnaccessData(satmp);

				iSegIndex++;
			}
		}
	}


	void PnrfHelper::saveChannelDataVector(const CComVariant &data,
		const string filePath,
		const int segment,
		const int samplingRate) {
		if (data.vt != VT_EMPTY)
		{
			// create segments pointer
			IDataSegmentsPtr itfSegments = data.punkVal;
			int iSegIndex = 1;					// segment index
			int iCount = itfSegments->Count;	// segemnt count
			if (iCount < 1)
			{
				throw exception("No Segments found");
				return;
			}

			if (segment > 0) {
				iSegIndex = segment;
				iCount = segment;
			}

			ofstream pnrfFormattedfile;
			pnrfFormattedfile.open(filePath, ofstream::out | ofstream::app);
			pnrfFormattedfile << "X, Y\n";

			// loop through all available segments
			while (iSegIndex <= iCount)
			{
				// pointer inside segment data
				IDataSegmentPtr itfSegment = NULL;
				itfSegments->get_Item(iSegIndex, &itfSegment);

				int lCnt = itfSegment->NumberOfSamples;

				// variant data array for segment data
				CComVariant varData;
				// fetch data
				itfSegment->Waveform(DataSourceResultType_Double64, 1, lCnt, 1, &varData);

				//If there is no data, process next segment
				if (varData.vt == VT_EMPTY)
					continue;

				//If it isn't an array, something is wrong here
				if (!(varData.vt & VT_ARRAY))
					continue;

				//Get data out through the use of the safe array and store locally
				SAFEARRAY* satmp = NULL;
				satmp = varData.parray;

				if (satmp->cDims > 1)
				{
					// It's a multi dimensional array
					continue;
				}
				double *pData;
				SafeArrayAccessData(satmp, (void**)&pData);

				double X0 = itfSegment->StartTime;
				double DeltaX = itfSegment->SampleInterval;
				int size = (int)satmp->rgsabound[0].cElements;

				if (samplingRate <= 0) {
					for (int i = 0; i < size; i++)
					{
						pnrfFormattedfile << (X0 + i * DeltaX) << "," << (pData[i]) << "\n";
					}
				}
				else {
					int i = 0;
					while (i < size) {
						const double sampleSize = (i + samplingRate) > size ? (size - i) : samplingRate;
						double avg = 0;
						for (int j = 0; j < sampleSize; j++)
						{
							avg += pData[j + i];
						}
						
						avg = (avg / samplingRate);
						pnrfFormattedfile << (X0 + i * DeltaX) << "," << avg << "\n";
						i = i + sampleSize;
					}
				}
				SafeArrayUnaccessData(satmp);

				iSegIndex++;
			}

			pnrfFormattedfile.close();
		}
	}

	/* End of private variables and methods */



	/* Start of public variables and methods */

	/* Constructor and destructor */
	PnrfHelper::PnrfHelper(const string filePath, DataType load_type) {
		HR(loaderPtr.CreateInstance(__uuidof(PNRFLoader)));
		auto canLoad = loaderPtr->CanLoadRecording(filePath.c_str());
		if (canLoad == 0) {
			throw exception("Cannot load given PNRF file");
			return;
		}
		else if (canLoad < 100) {
			// handle partial loading, warning
		}

		recorderPtr = loaderPtr->LoadRecording(filePath.c_str());
		if ((load_type < DataType::Continuous) & (load_type > DataType::All)) {
			load_type = DataType::All;
		}
		
		dtype = static_cast<DataSourceSelect>(load_type);
	}

	/* Move */

	PnrfHelper::PnrfHelper(PnrfHelper&& helper) {
		if (this != &helper) {
			this->loaderPtr = std::move(helper.loaderPtr);
			this->recorderPtr = std::move(helper.recorderPtr);
			this->dtype = std::move(helper.dtype);
			//this->runtime = std::move(helper.runtime);

			helper.loaderPtr = nullptr;
			helper.recorderPtr = nullptr;
		}
	}

	PnrfHelper& PnrfHelper::operator=(PnrfHelper&& helper) {
		this->loaderPtr = std::move(helper.loaderPtr);
		this->recorderPtr = std::move(helper.recorderPtr);
		this->dtype = std::move(helper.dtype);
		//this->runtime = std::move(helper.runtime);

		helper.loaderPtr = nullptr;
		helper.recorderPtr = nullptr;
		return *this;
	}

	/* Implementation of IFileProcessor interface*/
	void PnrfHelper::loadChannel(const int channel, std::vector<std::pair<double, double>>& val) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->getChannelDataVector(data, val);
	}

	void PnrfHelper::loadChannelSeg(const int channel, const int segment, std::vector<std::pair<double, double>>& val) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->getChannelDataVector(data, val, segment);
	}

	void PnrfHelper::loadChannelWithSampling(const int channel, const int samplingRate, std::vector<std::pair<double, double>>& val) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->getChannelDataVector(data, val, -1, samplingRate);
	}

	void PnrfHelper::loadChannelSegWithSampling(const int channel, const int segment, const int samplingRate, std::vector<std::pair<double, double>>& val) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->getChannelDataVector(data, val, segment, samplingRate);
	}

	/* Implementation of Ifilemeta interface */
	Meta PnrfHelper::getFileMetaInfo() {
		Meta meta(string(recorderPtr->GetTitle()));
		string channelType[] = { "None", "DataChannelType_Analog", "DataChannelType_Event" };
		IDataRecordersPtr recorderDataPtr = recorderPtr->GetRecorders();
		if (recorderDataPtr->GetCount() > 0) {
			auto recordCount = recorderDataPtr->GetCount();
			for (int i = 1; i <= recordCount; i++) {
				Record record;

				IDataRecorderPtr rPtr = recorderDataPtr->GetItem(i);
				auto channelCount = rPtr->GetChannels()->GetCount();
				for (int j = 1; j <= channelCount; j++) {
					//info
					//auto info = [recorder i, channel j]
					//name
					Channel ch;
					IDataChannelPtr cPtr = rPtr->GetChannels()->GetItem(j);
					auto name = cPtr->GetName();
					DataChannelType type = cPtr->GetChannelType();
					IDataSrcPtr dPtr = cPtr->GetDataSource(DataSourceSelect_Mixed);
					auto xUnit = dPtr->GetXUnit();
					auto yUnit = dPtr->GetYUnit();

					auto startTime = dPtr->GetSweeps()->GetStartTime();
					auto endTime = dPtr->GetSweeps()->GetEndTime();

					ch.assignChannelDetails(string(name), channelType[static_cast<int>(type)], string(yUnit), startTime, endTime);
					CComVariant data;
					dPtr->Data(startTime, endTime, &data);
					if (data.vt != VT_EMPTY)
					{
						// create segments pointer
						IDataSegmentsPtr sPtr = data.punkVal;
						auto segmentCount = sPtr->GetCount();
						auto noOfSamples = sPtr->GetItem(1)->GetNumberOfSamples();
						auto sampleInterval = sPtr->GetItem(1)->GetSampleInterval();
						ch.assignSegmentDetails(segmentCount, noOfSamples, sampleInterval);
					}

					record.addChannel(ch);
				}

				meta.addRecord(record);
			}
		}

		return meta;
	}

	int PnrfHelper::getWindowSize(const int channel, const double windowTime)
	{
		CComVariant data;
		this->getChannelData(channel, data);

		if (data.vt != VT_EMPTY)
		{
			// create segments pointer
			IDataSegmentsPtr sPtr = data.punkVal;
			auto sampleInterval = sPtr->GetItem(1)->GetSampleInterval();
			return ((int)round(windowTime / sampleInterval));
		}
		return 0;
	}

	int PnrfHelper::getChannelSize(const int channel, const int samplingRate) {
		CComVariant data;
		this->getChannelData(channel, data);
		unsigned int out = 0;
		unsigned int rate = (samplingRate <= 0) ? 1 : samplingRate;

		if (data.vt != VT_EMPTY)
		{
			// create segments pointer
			IDataSegmentsPtr itfSegments = data.punkVal;
			int iCount = itfSegments->Count;	// segemnt count
			if (iCount < 1)
			{
				throw exception("No Segments found");
			}

			// loop through all available segments
			for (int iSegIndex = 1; iSegIndex <= iCount; iSegIndex++)
			{
				// pointer inside segment data
				IDataSegmentPtr itfSegment = NULL;
				itfSegments->get_Item(iSegIndex, &itfSegment);

				int lCnt = itfSegment->NumberOfSamples;

				// variant data array for segment data
				CComVariant varData;
				// fetch data
				itfSegment->Waveform(DataSourceResultType_Double64, 1, lCnt, 1, &varData);

				//If there is no data, process next segment
				if (varData.vt == VT_EMPTY)
					continue;

				//If it isn't an array, something is wrong here
				if (!(varData.vt & VT_ARRAY))
					continue;

				lCnt = ceil(((double)lCnt) / rate);
				out = out + lCnt;
			}

			//out = ceil(((double)out) / rate);
		}

		return out;
	}

	int PnrfHelper::getSegmentSize(const int channel, const int segment, const int samplingRate) {
		CComVariant data;
		this->getChannelData(channel, data);
		unsigned int out = 0;
		unsigned int rate = (samplingRate <= 0) ? 1 : samplingRate;
		if (data.vt != VT_EMPTY)
		{
			// create segments pointer
			IDataSegmentsPtr itfSegments = data.punkVal;
			int iCount = itfSegments->Count;	// segemnt count
			if (segment < 1 || segment > iCount)
			{
				throw exception("No Segments found");
			}

			// retrieve segment size
			IDataSegmentPtr itfSegment = NULL;
			itfSegments->get_Item(segment, &itfSegment);

			out = (int)itfSegment->NumberOfSamples;
			out = ceil(((double)out) / rate);
		}

		return out;
	}

	/* Implementation of IOffline interface */
	void PnrfHelper::saveChannelDataToFile(vector<pair<double, double>> &val, const string filePath) {
		ofstream pnrfFormattedfile;
		pnrfFormattedfile.open(filePath);
		pnrfFormattedfile << "X, Y\n";

		for_each(begin(val), end(val), [&](const pair<double, double>& elem) {
			pnrfFormattedfile << elem.first << "," << elem.second << "\n";
		});

		pnrfFormattedfile.close();
	}

	// todo: how to save to a file
	void PnrfHelper::saveAllDataToFile(vector<pair<double, double>> &val, vector<pair<double, double>> &movAvg, const string filePath) {
		ofstream pnrfFormattedfile;
		pnrfFormattedfile.open(filePath);
		pnrfFormattedfile << "X, RMS\n";

		for (int i = 0; i < movAvg.size(); i++) {
			pnrfFormattedfile << movAvg[i].first << "," << movAvg[i].second << "\n";
		}

		pnrfFormattedfile.close();
	}
	
	void PnrfHelper::saveChannelToFile(const int channel, const string filePath) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->saveChannelDataVector(data, filePath);
	}

	void PnrfHelper::saveChannelWithSamplingToFile(const int channel, const int samplingRate, const string filePath) {
		CComVariant data;
		this->getChannelData(channel, data);
		this->saveChannelDataVector(data, filePath, -1, samplingRate);
	}
	

	void PnrfHelper::getMovingAvg(vector<pair<double, double>>& data, vector<pair<double, double>>& movAvg, const int windowSize)
	{
		if (data.size() > 0) {
			double runningTotal = 0.0;
			int avg = 1;
			movAvg.clear();
			movAvg.reserve(data.size());

			for (int i = 0; i < data.size(); i++)
			{
				data[i].second = pow(data[i].second, 2);
				if (i > 0) {
					data[i].second = data[i].second + data[i - 1].second;
				}

				runningTotal = i <= windowSize ? data[i].second : (data[i].second - data[i - windowSize - 1].second);
				avg = i < windowSize ? (i + 1) : (windowSize + 1);
				movAvg.emplace_back(data[i].first, sqrt(runningTotal / avg));
			}
		}
	}

	void PnrfHelper::getMovingAvgWithSampling(vector<pair<double, double>>& data,
		vector<pair<double, double>>& movAvg, const int windowSize,
		const int samplingRate)
	{
		int idx = 0;
		if (data.size() > 0) {
			double runningTotal = 0.0;
			int avg = 1;
			movAvg.clear();
			movAvg.reserve(data.size());
			int idx = 0;

			for (int i = 0; i < data.size(); i++)
			{
				data[i].second = pow(data[i].second, 2);
				if (i > 0) {
					data[i].second = data[i].second + data[i - 1].second;
				}

				runningTotal = i <= windowSize ? data[i].second : (data[i].second - data[i - windowSize - 1].second);
				avg = i < windowSize ? (i + 1) : (windowSize + 1);
				movAvg.emplace_back(data[i].first, sqrt(runningTotal / avg));
			}
			
			if (samplingRate > 1) {
				int i = 0, idx = 0;
				double avg = 0;
				while (idx < movAvg.size()) {
					const double sampleSize = (idx + samplingRate) > data.size() ? (data.size() - idx) : samplingRate;
					avg = 0;
					for (int j = 0; j < sampleSize; j++)
					{
						avg += movAvg[j + idx].second;
					}

					movAvg[i].first = movAvg[idx].first;
					movAvg[i].second = (avg / sampleSize);
					idx = idx + sampleSize;
					i++;
				}

				movAvg.resize(--i);
			}
		}
	}


	/* End of public variables and method */
}