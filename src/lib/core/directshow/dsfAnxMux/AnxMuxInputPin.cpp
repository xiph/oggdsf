#include "StdAfx.h"
#include ".\anxmuxinputpin.h"
#include "AnxMuxFilter.h"
AnxMuxInputPin::AnxMuxInputPin(AnxMuxFilter* inOwningFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream)
:	OggMuxInputPin(inOwningFilter, inFilterLock, inHR, inMuxStream)
{
	debugLog.open("g:\\logs\\anxmuxinputpin.log", ios_base::out);
}

AnxMuxInputPin::~AnxMuxInputPin(void)
{
}

HRESULT AnxMuxInputPin::SetMediaType(const CMediaType* inMediaType) 
{

	bool locWasOK = false;
	unsigned __int64 locGranRateNum = 0;
	unsigned __int64 locGranRateDenom = 0;
	unsigned long locNumHeaders = 0;
	StreamHeaders::eCodecType locCodecID = StreamHeaders::NONE;

	
	if ((inMediaType->majortype == MEDIATYPE_Video) && (inMediaType->subtype == MEDIASUBTYPE_Theora)) {
		//Theora
		
		sTheoraFormatBlock* locTheora = (sTheoraFormatBlock*)inMediaType->pbFormat;
		//debugLog<<"Theo sample rate = "<<locTheora->frameRateNumerator<<" / "<<locTheora->frameRateDenominator<<endl;
		//debugLog<<"Theo KFI = "<<locTheora->maxKeyframeInterval<<endl;
		mMuxStream->setConversionParams(locTheora->frameRateNumerator, locTheora->frameRateDenominator, 10000000, locTheora->maxKeyframeInterval);
		mPaginator.setNumHeaders(3);

		locWasOK = true;
		locGranRateNum = locTheora->frameRateNumerator;
		locGranRateDenom = locTheora->frameRateDenominator;
		locNumHeaders = 3;
		locCodecID = StreamHeaders::THEORA;

	} else if (inMediaType->majortype == MEDIATYPE_Audio) {
		if (inMediaType->subtype == MEDIASUBTYPE_Vorbis) {
			//Vorbis
			sVorbisFormatBlock* locVorbis = (sVorbisFormatBlock*)inMediaType->pbFormat;
			//debugLog<<"Vorbis sample rate = "<<locVorbis->samplesPerSec<<endl;
			mMuxStream->setConversionParams(locVorbis->samplesPerSec, 1, 10000000);
			mPaginator.setNumHeaders(3);

			locWasOK = true;
			locGranRateNum = locVorbis->samplesPerSec;
			locGranRateDenom = 1;
			locNumHeaders = 3;

			locCodecID = StreamHeaders::VORBIS;

			
		} else if (inMediaType->subtype == MEDIASUBTYPE_Speex) {
			//Speex
			sSpeexFormatBlock* locSpeex = (sSpeexFormatBlock*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locSpeex->samplesPerSec, 1, 10000000);
			mPaginator.setNumHeaders(2);


			locWasOK = true;
			locGranRateNum = locSpeex->samplesPerSec;
			locGranRateDenom = 1;
			locNumHeaders = 2;
			locCodecID = StreamHeaders::SPEEX;
		}

		//TODO::: Add a num headers field to the flac format block.

		//} else if (inMediaType->subtype == MEDIASUBTYPE_OggFLAC_1_0) {
		//	//We are connected to the encoder nd getting individual metadata packets.
		//	sFLACFormatBlock* locFLAC = (sFLACFormatBlock*)inMediaType->pbFormat;
		//	mMuxStream->setConversionParams(locFLAC->samplesPerSec, 1, 10000000);
		//	//debugLog<<"FLAC sample rate = "<<locFLAC->samplesPerSec<<endl;
		//	//mNeedsFLACHeaderTweak = true;
		//	mNeedsFLACHeaderCount = true;
		//} else if (inMediaType->subtype == MEDIASUBTYPE_FLAC) {
		//	//We are connected directly to the demux and are getting metadata in one block
		//	// Need to use the header splitter class.
		//	sFLACFormatBlock* locFLAC = (sFLACFormatBlock*)inMediaType->pbFormat;
		//	mMuxStream->setConversionParams(locFLAC->samplesPerSec, 1, 10000000);
		//	//debugLog<<"FLAC sample rate = "<<locFLAC->samplesPerSec<<endl;
		//	mNeedsFLACHeaderTweak = true;
		//} 

		
	}
	if (locWasOK) {
		mPaginator.acceptStampedOggPacket(AnxPacketMaker::makeAnxData_2_0(2,0, locGranRateNum, locGranRateDenom, locNumHeaders, AnxPacketMaker::makeMessageHeaders(locCodecID)));
        return S_OK;
	} else {
		return S_FALSE;
	}
































	////This pushes an anxdata packet into the stream.
	//HRESULT locHR = OggMuxInputPin::SetMediaType(inMediaType);

	//if (locHR == S_OK) {
	//	debugLog<<"Set media type ok in base class"<<endl;
	//	mPaginator.acceptStampedOggPacket(AnxPacketMaker::makeAnxData_2_0(mMuxStream, &mPaginator));
	//}

	//return locHR;


}