#pragma once
//
//#ifndef LOOG_INT64
//# ifdef WIN32
//#  define LOOG_INT64 signed __int64
//# else  /* assume POSIX */
//#  define LOOG_INT64 int64_t
//# endif
//#endif
//

#include <libOOOgg/libOOOgg.h>
#include <libOOOggSeek/IOggDecoderSeek.h>
#include <string>
using namespace std;
class IOggDecoder 
	:	public IOggDecoderSeek
{
public:
	enum eAcceptHeaderResult {
		AHR_ALL_HEADERS_RECEIVED,
		AHR_MORE_HEADERS_TO_COME,
		AHR_INVALID_HEADER,
		AHR_UNEXPECTED,
		AHR_NULL_POINTER,

	};
	virtual LOOG_INT64 convertGranuleToTime(LOOG_INT64 inGranule) = 0;
	virtual LOOG_INT64 mustSeekBefore(LOOG_INT64 inGranule) = 0;
	virtual eAcceptHeaderResult showHeaderPacket(OggPacket* inCodecHeaderPacket) = 0;
	virtual string getCodecShortName() = 0;
	virtual string getCodecIdentString() = 0;
	
};