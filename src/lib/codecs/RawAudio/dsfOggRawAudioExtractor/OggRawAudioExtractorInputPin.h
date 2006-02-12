#pragma once

#include "oggrawaudioextractordllstuff.h"
#include "IOggDecoder.h"

class OggRawAudioExtractorFilter;

class OggRawAudioExtractorInputPin
	:	public CTransformInputPin
	,	public IOggDecoder
{
public:
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	OggRawAudioExtractorInputPin(OggRawAudioExtractorFilter* inParent, HRESULT* outHR);
	virtual ~OggRawAudioExtractorInputPin(void);

	virtual STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps);
	virtual HRESULT SetMediaType(const CMediaType* inMediaType);
	virtual HRESULT CheckMediaType(const CMediaType *inMediaType);

	

	//IOggDecoder Interface
	virtual LOOG_INT64 convertGranuleToTime(LOOG_INT64 inGranule);
	virtual LOOG_INT64 mustSeekBefore(LOOG_INT64 inGranule);
	virtual IOggDecoder::eAcceptHeaderResult showHeaderPacket(OggPacket* inCodecHeaderPacket);
	virtual string getCodecShortName();
	virtual string getCodecIdentString();


	sOggRawAudioFormatBlock getFormatBlock()		{return mFormatBlock;	}
protected:
	enum eOggRawAudioSetupState {
		VSS_SEEN_NOTHING,
		VSS_SEEN_BOS,
		VSS_SEEN_COMMENT,
		VSS_ALL_HEADERS_SEEN,
		VSS_ERROR
	};

	eOggRawAudioSetupState mSetupState;
	bool handleHeaderPacket(OggPacket* inHeaderPack);

	sOggRawAudioFormatBlock mFormatBlock; 


	static const unsigned long OGG_RAW_AUDIO_IDENT_HEADER_SIZE = 28;
	static const unsigned long OGG_RAW_AUDIO_NUM_BUFFERS = 50;


	enum eRawAudioFormat {
		FMT_S8,
		FMT_U8,
		FMT_S16_LE,
		FMT_S16_BE,
		FMT_S24_LE,
		FMT_S24_BE,
		FMT_S32_LE,
		FMT_S32_BE,

		FMT_ULAW		=	0x10,
		FMT_ALAW,

		FMT_FLT32_LE	=	0x20,
		FMT_FLT32_BE,
		FMT_FLT64_LE,
		FMT_FLT64_BE,

		
	};


};
