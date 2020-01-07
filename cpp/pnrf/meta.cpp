#include "stdafx.h"			// standard stuff
#include "pnrfhelper.h"

using std::exception;
using std::string;

namespace pnrfr {
	// Implementation of File meta class, Record, and Channel
	string Channel::getName() const { return this->name; };
	string Channel::getType() const { return this->type; };
	string Channel::getYUnit() const { return this->yunit; };
	double Channel::getStartTime() const { return this->startTime; };
	double Channel::getEndTime() const { return this->endTime; };
	int Channel::getSegmentCount() const { return this->segmentCount; };
	int Channel::getNoOfSamples() const { return this->noOfSamples; };
	double Channel::getSampleInterval() const { return this->sampleInterval; };

	void Channel::assignChannelDetails(string cname, string ctype, std::string cyunit, double cstarttime, double cendtime) {
		this->name = cname;
		this->type = ctype;
		this->yunit = cyunit;
		this->startTime = cstarttime;
		this->endTime = cendtime;
	};

	void Channel::assignSegmentDetails(int segmentcount, int noofsamples, double sampleinterval) {
		this->segmentCount = segmentcount;
		this->noOfSamples = noofsamples;
		this->sampleInterval = sampleinterval;
	};

	void Record::addChannel(Channel channel) {
		channels.push_back(channel);
	}

	 Channel* Record::getChannel(int no) {
		no = no - 1;
		if ((no >= channels.size()) | (no < 0)) {
			throw exception("Index out of range, no channel found");
		}
		return &channels[no];
	}

	int Record::getChannelCount() const {
		return static_cast<int>(channels.size());
	}

	void Meta::addRecord(Record record) {
		records.push_back(record);
	}

	 Record* Meta::getRecord(int no) {
		no = no - 1;
		if ((no >= records.size()) | (no < 0)) {
			throw exception("Index out of range, no record found");
		}
		return &records[no];
	}

	int Meta::getRecordCount() const {
		return static_cast<int>(records.size());
	}

	std::string Meta::getTitle() const { return this->title; };

}