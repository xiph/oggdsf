#pragma once


#include "IRecomposer.h"

#include <libOOOgg/libOOOgg.h>
#include <libOOOggChef/libOOOggChef.h>

#include <string>
#include <vector>

using namespace std;

class LIBOOOGGCHEF_API AnnodexRecomposer : public IRecomposer, public IOggCallback
{
public:
	typedef bool (*BufferWriter) (unsigned char *buffer, unsigned long bufferSize, void *userData);

	AnnodexRecomposer(void);
	AnnodexRecomposer(string inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData);
	~AnnodexRecomposer(void);

	void recomposeStreamFrom(double inStartingTimeOffset, const vector<const string>* inWantedMIMETypes);
	bool acceptOggPage(OggPage* inOggPage);

    AnnodexRecomposer(const AnnodexRecomposer&);  // Don't copy me
    AnnodexRecomposer &operator=(const AnnodexRecomposer&);  // Don't assign men

protected:

	typedef pair<unsigned long, unsigned long> tSerial_HeadCountPair;

	enum eDemuxState {
		SEEN_NOTHING,
		SEEN_ANNODEX_BOS,
		SEEN_ANNODEX_EOS,
		SEEN_ALL_CODEC_HEADERS,
		INVALID = 100,
	};

	enum eDemuxParserState {
		LOOK_FOR_HEADERS,
		LOOK_FOR_BODY,
	};

	void sendPage(OggPage* inOggPage);

	BufferWriter mBufferWriter;
	void* mBufferWriterUserData;

	fstream mDebugFile;

	string mFilename;

	unsigned long mAnnodexSerialNumber;

	eDemuxState mDemuxState;
	eDemuxParserState mDemuxParserState;

	vector<tSerial_HeadCountPair> mWantedStreamSerialNumbers;
	const vector<const string>* mWantedMIMETypes;
};
