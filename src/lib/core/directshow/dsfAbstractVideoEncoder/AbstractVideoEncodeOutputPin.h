#pragma once
#include "abstractvideoencoderdllstuff.h"
#include "AbstractVideoEncodeFilter.h"
class AbstractVideoEncodeFilter;
class ABS_VIDEO_ENC_API AbstractVideoEncodeOutputPin
	:	public CBaseOutputPin
{
public:
	AbstractVideoEncodeOutputPin(AbstractVideoEncodeFilter* inParentFilter, CCritSec* inFilterLock, CHAR* inObjectName, LPCWSTR inPinDisplayName, CMediaType* inOutputMediaType);
	virtual ~AbstractVideoEncodeOutputPin(void);

	virtual HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES *inReqAllocProps);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);

	//PURE VIRTUALS
	virtual bool FillFormatBuffer(BYTE* inFormatBuffer) = 0;
	virtual unsigned long FormatBufferSize() = 0;
	
	virtual HRESULT GetMediaType(int inPosition, CMediaType *outMediaType);


	//Virtuals for data queue
	virtual HRESULT CompleteConnect (IPin *inReceivePin);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverEndFlush(void);
	virtual HRESULT DeliverBeginFlush(void);

	COutputQueue* mDataQueue;
protected:
	HRESULT mHR;
	AbstractVideoEncodeFilter* mParentFilter;
	void FillMediaType(CMediaType* inMediaType);

	CMediaType* mOutputMediaType;

	static const int BUFF_SIZE = 65536;	static const int BUFF_COUNT = 5;
};
