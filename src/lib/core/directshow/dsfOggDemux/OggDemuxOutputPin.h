#pragma once

class OggDemuxOutputPin
{
public:
	OggDemuxOutputPin(void);
	~OggDemuxOutputPin(void);

	static const unsigned long BUFFER_SIZE = 98304;
	static const unsigned long NUM_BUFFERS = 3;

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	
	virtual HRESULT DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest);
	virtual HRESULT CheckMediaType(const CMediaType *pmt);
 
};
