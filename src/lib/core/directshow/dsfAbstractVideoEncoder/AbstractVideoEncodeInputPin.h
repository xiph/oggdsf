#pragma once
#include "abstractvideoencoderdllstuff.h"
//#include "AbstractVideoEncodeOutputPin.h"
#include "AbstractVideoEncodeFilter.h"
class AbstractVideoEncodeOutputPin;
class AbstractVideoEncodeFilter;
class ABS_VIDEO_ENC_API AbstractVideoEncodeInputPin
	:	public CBaseInputPin
{
public:
	AbstractVideoEncodeInputPin(AbstractVideoEncodeFilter* inParentFilter, CCritSec* inFilterLock, AbstractVideoEncodeOutputPin* inOutputPin, CHAR* inObjectName, LPCWSTR inPinDisplayName);
	virtual ~AbstractVideoEncodeInputPin(void);

	static const short SIZE_16_BITS = 2;
	static const unsigned long HUNDRED_NANOS = 10000000;
	static const signed short SINT_MAX = 32767;
	static const signed short SINT_MIN = -32768;

	
	

	//PURE VIRTUALS
	virtual long encodeData(unsigned char* inBuf, long inNumBytes) = 0;
	virtual bool ConstructCodec() = 0;
	virtual void DestroyCodec() = 0;
	

	STDMETHODIMP Receive(IMediaSample *pSample);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);
	virtual HRESULT GetMediaType(int inPosition, CMediaType *outMediaType);
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);

	virtual void ResetFrameCount();

	virtual STDMETHODIMP EndOfStream(void);
	virtual STDMETHODIMP BeginFlush();
	virtual STDMETHODIMP EndFlush();
	virtual STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

protected:

	virtual bool SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime);


	HRESULT mHR;
	bool mBegun;
	AbstractVideoEncodeOutputPin* mOutputPin;
	__int64 mUptoFrame;
	AbstractVideoEncodeFilter* mParentFilter;
	CMediaType* mAcceptableMediaType;

	//fstream debugLog;
	//unsigned long mFrameSize;
	//unsigned long mNumChannels;
	//unsigned long mSampleRate;

	VIDEOINFOHEADER* mVideoFormat;
};
