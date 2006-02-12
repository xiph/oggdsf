
#pragma once

class IOggDecoderSeek {
public:
	IOggDecoderSeek(void)		{}
	virtual ~IOggDecoderSeek(void)		{}


	virtual LOOG_INT64 convertGranuleToTime(LOOG_INT64 inGranule) = 0;
	virtual LOOG_INT64 mustSeekBefore(LOOG_INT64 inGranule) = 0;


};
