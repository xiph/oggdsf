//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================
#include "stdafx.h"
#include ".\anxmuxinputpin.h"
#include "AnxMuxFilter.h"
AnxMuxInputPin::AnxMuxInputPin(AnxMuxFilter* inOwningFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream, unsigned long inAnxVersionMajor, unsigned long inAnxVersionMinor)
	:	OggMuxInputPin(inOwningFilter, inFilterLock, inHR, inMuxStream)
	,	mAnxDataPacket(NULL)
	,	mFishBonePacket(NULL)
	//,	mExtraPacket(NULL)
	,	mAnxVersionMajor(inAnxVersionMajor)
	,	mAnxVersionMinor(inAnxVersionMinor)
{
#ifdef OGGCODECS_LOGGING
	debugLog.open("g:\\logs\\anxmuxinputpin.log", ios_base::out);
#endif
	mExtraPackets.clear();
}

AnxMuxInputPin::~AnxMuxInputPin(void)
{
}


HRESULT AnxMuxInputPin::CompleteConnect(IPin* inReceivePin) {
	
	//Set our delegate to the pin that is connecting to us... we'll send them our seek messages.
	IMediaSeeking* locSeeker = NULL;
	inReceivePin->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	
	mMuxStream->setIsActive(true);

	HRESULT locHR = mParentFilter->addAnotherPin();
	if ((locHR == S_OK)) {// && (mAnxDataPacket != NULL)) {
		//ANX3::: Only do this for anx2... in anx 3 we need to get the fishbone some other way.
		if ((mAnxVersionMajor == 2) && (mAnxVersionMinor == 0) && (mAnxDataPacket != NULL)) {
			mPaginator.acceptStampedOggPacket(mAnxDataPacket);
			return S_OK;
		} else if ((mAnxVersionMajor == 3) && (mAnxVersionMinor == 0) && (mFishBonePacket != NULL)) {
			//Force in a CMML Packet BOS
			if (mExtraPackets.size() != 0) {
				for (size_t i = 0; i < mExtraPackets.size(); i++) {
					mPaginator.acceptStampedOggPacket(mExtraPackets[i]);
				}
			}
			return S_OK;
		} else {
			return S_FALSE;
		}
		
	} else {
		return S_FALSE;
	}
}


HRESULT AnxMuxInputPin::SetMediaType(const CMediaType* inMediaType) 
{

	bool locWasOK = false;
	unsigned __int64 locGranRateNum = 0;
	unsigned __int64 locGranRateDenom = 0;
	unsigned long locNumHeaders = 0;
	unsigned long locPreroll = 0;
	unsigned long locGranuleShift = 0;
	StreamHeaders::eCodecType locCodecID = StreamHeaders::NONE;

	
	if ((inMediaType->majortype == MEDIATYPE_Video) && (inMediaType->subtype == MEDIASUBTYPE_Theora)) {
		//Theora
		
		sTheoraFormatBlock* locTheora = (sTheoraFormatBlock*)inMediaType->pbFormat;
		//debugLog<<"Theo sample rate = "<<locTheora->frameRateNumerator<<" / "<<locTheora->frameRateDenominator<<endl;
		//debugLog<<"Theo KFI = "<<locTheora->maxKeyframeInterval<<endl;
		mMuxStream->setConversionParams(locTheora->frameRateNumerator, locTheora->frameRateDenominator, 10000000, locTheora->maxKeyframeInterval);
		mMuxStream->setNumHeaders(3);
		mPaginator.setNumHeaders(3);

		locGranuleShift = locTheora->maxKeyframeInterval;
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
			mMuxStream->setNumHeaders(3);
			mPaginator.setNumHeaders(3);

			locWasOK = true;
			locGranRateNum = locVorbis->samplesPerSec;
			locGranRateDenom = 1;
			locNumHeaders = 3;
			locPreroll = 2;

			locCodecID = StreamHeaders::VORBIS;

			
		} else if (inMediaType->subtype == MEDIASUBTYPE_Speex) {
			//Speex
			sSpeexFormatBlock* locSpeex = (sSpeexFormatBlock*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locSpeex->samplesPerSec, 1, 10000000);
			mMuxStream->setNumHeaders(2);
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

		
	} else if(inMediaType->majortype == MEDIATYPE_Text) {
		if (inMediaType->subtype == MEDIASUBTYPE_CMML) {
			//CMML
			sCMMLFormatBlock* locCMML = (sCMMLFormatBlock*)inMediaType->pbFormat;
			


			//ANX3::: 1 for anx 2, 2 for anx 3.
			if ((mAnxVersionMajor == 3) && (mAnxVersionMinor == 0)) {
				mMuxStream->setConversionParams(locCMML->granuleNumerator, locCMML->granuleDenominator, 10000000, 32);
				//ZZZZZ:::: sould be 3
				locNumHeaders = 3;
			} else {
				mMuxStream->setConversionParams(locCMML->granuleNumerator, locCMML->granuleDenominator, 10000000);
				locNumHeaders = 1;
			}

			mPaginator.setNumHeaders(locNumHeaders);
			mPaginator.parameters()->mMaxPacksPerPage = 1;
			mMuxStream->setNumHeaders(locNumHeaders);


			locWasOK = true;
			locGranRateNum = locCMML->granuleNumerator;
			locGranRateDenom = locCMML->granuleDenominator;


			locCodecID = StreamHeaders::CMML;			
		}
	}
	if (locWasOK) {
		//ANX3::: Need to make our fishbone here.
		
		
		if ((mAnxVersionMajor == 2) && (mAnxVersionMinor == 0)) {
			//Save the packet, we'll push it into the stream when the connection is established
			mAnxDataPacket = AnxPacketMaker::makeAnxData_2_0(2, 0, locGranRateNum, locGranRateDenom, locNumHeaders, AnxPacketMaker::makeMessageHeaders(locCodecID));
		} else if ((mAnxVersionMajor == 3) && (mAnxVersionMinor == 0)) {
			mFishBonePacket = FishSkeleton::makeFishBone_3_0(locGranRateNum, locGranRateDenom, 0, locNumHeaders, mPaginator.parameters()->mSerialNo, locGranuleShift, locPreroll, AnxPacketMaker::makeMessageHeaders(locCodecID));
			if (locCodecID == StreamHeaders::CMML) {
				mExtraPackets.push_back(FishSkeleton::makeCMMLBOS());
				mExtraPackets.push_back(FishSkeleton::makeCMML_XML_Thing());
			} else {
				mExtraPackets.clear();
				//mExtraPacket = NULL;
			}
		}
        return S_OK;
	} else {
		return S_FALSE;
	}
}