#pragma once

class ITheoraEncodeSettings
{
public:
	
	virtual unsigned long targetBitrate() = 0;
	virtual unsigned char quality() = 0;
	virtual unsigned long keyframeFreq() = 0;

	virtual bool setTargetBitrate(unsigned long inBitrate) = 0;
	virtual bool setQuality(unsigned char inQuality) = 0;
	virtual bool setKeyframeFreq(unsigned long inKeyframeFreq) = 0;
protected:
	//unsigned long mTargetBitrate;

	//unsigned char mQuality;

	//unsigned long mKeyframeFreq;
	
};
