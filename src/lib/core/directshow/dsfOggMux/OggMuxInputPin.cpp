//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//          (C) 2013 Cristian Adam
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

#include "Precompiled.h"
#include "oggmuxinputpin.h"

OggMuxInputPin::OggMuxInputPin(OggMuxFilter* inParentFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream)
	:	CBaseInputPin(NAME("OggMuxInputPin"), inParentFilter, inFilterLock, inHR, L"Ogg Packet In")
	,	mParentFilter(inParentFilter)
	,	mMuxStream(inMuxStream)
	,	mNeedsFLACHeaderTweak(false)
	,	mNeedsFLACHeaderCount(false)
{
	OggPaginatorSettings* locSettings = new OggPaginatorSettings;
	locSettings->mMinPageSize = 4096;
	locSettings->mMaxPageSize = 8192;
	
	LARGE_INTEGER locTicks;
	QueryPerformanceCounter(&locTicks);
	srand((unsigned int)locTicks.LowPart);
	locSettings->mSerialNo = ((unsigned long)(rand() + 1)) * ((unsigned long)(rand() + 1));

	mPaginator.setParameters(locSettings);
	mPaginator.setPageCallback(mMuxStream);
}

OggMuxInputPin::~OggMuxInputPin()
{
}

STDMETHODIMP OggMuxInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) 
    {
        return GetInterface((IMediaSeeking*)this, ppv);
	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}

//ANX::: Override and insert an anxdata into the stream.
HRESULT OggMuxInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//LOG(logDEBUG)<<"Set media type..."<<endl;
	if (inMediaType->majortype == MEDIATYPE_Video) 
    {
		if (inMediaType->subtype == MEDIASUBTYPE_Theora) 
        {
			//Theora	
			THEORAFORMAT* locTheora = (THEORAFORMAT*)inMediaType->pbFormat;
			//LOG(logDEBUG)<<"Theo sample rate = "<<locTheora->frameRateNumerator<<" / "<<locTheora->frameRateDenominator<<endl;
			//LOG(logDEBUG)<<"Theo KFI = "<<locTheora->maxKeyframeInterval<<endl;
			mMuxStream->setConversionParams(locTheora->frameRateNumerator, locTheora->frameRateDenominator, 10000000, locTheora->maxKeyframeInterval);
			mMuxStream->setNumHeaders(3);
			mPaginator.setNumHeaders(3);
		}
		else if (inMediaType->subtype == MEDIASUBTYPE_Schroedinger) 
        {
			mMuxStream->setConversionParams(*((unsigned *)inMediaType->pbFormat), *(((unsigned *)inMediaType->pbFormat) + 1), 10000000, 32);
			mMuxStream->setNumHeaders(1);
			mPaginator.setNumHeaders(1);
		} 
	} else if (inMediaType->majortype == MEDIATYPE_Audio) 
    {
		if (inMediaType->subtype == MEDIASUBTYPE_Vorbis) 
        {
			//Vorbis
			VORBISFORMAT* locVorbis = (VORBISFORMAT*)inMediaType->pbFormat;
			//LOG(logDEBUG)<<"Vorbis sample rate = "<<locVorbis->samplesPerSec<<endl;
			mMuxStream->setConversionParams(locVorbis->samplesPerSec, 1, 10000000);
			mMuxStream->setNumHeaders(3);
			mPaginator.setNumHeaders(3);
			
		} 
        else if (inMediaType->subtype == MEDIASUBTYPE_Speex) 
        {
			//Speex
			SPEEXFORMAT* locSpeex = (SPEEXFORMAT*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locSpeex->samplesPerSec, 1, 10000000);
			mMuxStream->setNumHeaders(2);
			mPaginator.setNumHeaders(2);
		} 
        else if (inMediaType->subtype == MEDIASUBTYPE_OggFLAC_1_0) 
        {
			//We are connected to the encoder nd getting individual metadata packets.
			FLACFORMAT* locFLAC = (FLACFORMAT*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locFLAC->samplesPerSec, 1, 10000000);
			//LOG(logDEBUG)<<"FLAC sample rate = "<<locFLAC->samplesPerSec<<endl;
			//mNeedsFLACHeaderTweak = true;
			mNeedsFLACHeaderCount = true;
		} 
        else if (inMediaType->subtype == MEDIASUBTYPE_FLAC) 
        {
			//We are connected directly to the demux and are getting metadata in one block
			// Need to use the header splitter class.
			FLACFORMAT* locFLAC = (FLACFORMAT*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locFLAC->samplesPerSec, 1, 10000000);
			//LOG(logDEBUG)<<"FLAC sample rate = "<<locFLAC->samplesPerSec<<endl;
			mNeedsFLACHeaderTweak = true;
		} 
        else if (inMediaType->subtype == MEDIASUBTYPE_RawOggAudio) 
        {
			OGGRAWAUDIOFORMAT* locRawAudio = (OGGRAWAUDIOFORMAT*)inMediaType->pbFormat;
			mMuxStream->setConversionParams(locRawAudio->samplesPerSec, 1, 10000000);
			mMuxStream->setNumHeaders(locRawAudio->numHeaders);
			mPaginator.setNumHeaders(locRawAudio->numHeaders);
		}		
	} 

	return S_OK;
}

HRESULT OggMuxInputPin::GetMediaType(int inPosition, CMediaType* outMediaType) 
{
	switch(inPosition) 
    {
		case 0:
			outMediaType->majortype = MEDIATYPE_Video;
			outMediaType->subtype = MEDIASUBTYPE_Theora;
			return S_OK;
		case 1:
			outMediaType->majortype = MEDIATYPE_Video;
			outMediaType->subtype = MEDIASUBTYPE_Schroedinger;
			return S_OK; 
		case 2:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_Vorbis;
			return S_OK;
		case 3:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_Speex;
			return S_OK;
		case 4:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_OggFLAC_1_0;
			return S_OK;
		case 5:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_FLAC;
			return S_OK;

		case 6:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_RawOggAudio;
			return S_OK;

		default:
			return VFW_S_NO_MORE_ITEMS;	
	}
}

HRESULT OggMuxInputPin::CheckMediaType(const CMediaType* inMediaType) 
{
	if	(	(inMediaType->majortype == MEDIATYPE_Video 
				&& inMediaType->subtype == MEDIASUBTYPE_Theora 
				&& inMediaType->formattype == FORMAT_Theora) 
 			||
			(inMediaType->majortype == MEDIATYPE_Video
				&& inMediaType->subtype == MEDIASUBTYPE_Schroedinger
				&& inMediaType->formattype == FORMAT_Schroedinger)
			||
			(inMediaType->majortype == MEDIATYPE_Audio 
				&& inMediaType->subtype == MEDIASUBTYPE_Vorbis 
				&& inMediaType->formattype == FORMAT_Vorbis)
			||
			(inMediaType->majortype == MEDIATYPE_Audio 
				&& inMediaType->subtype == MEDIASUBTYPE_Speex 
				&& inMediaType->formattype == FORMAT_Speex)
			||
			(inMediaType->majortype == MEDIATYPE_Audio
				&&	inMediaType->subtype == MEDIASUBTYPE_OggFLAC_1_0
				&&	inMediaType->formattype == FORMAT_FLAC)
			||
			(inMediaType->majortype == MEDIATYPE_Audio
				&&	inMediaType->subtype == MEDIASUBTYPE_FLAC
				&&	inMediaType->formattype == FORMAT_FLAC)
			||
			(inMediaType->majortype == MEDIATYPE_Audio
				&&	inMediaType->subtype == MEDIASUBTYPE_RawOggAudio
				&&	inMediaType->formattype == FORMAT_RawOggAudio)
    )
    {
		return S_OK;
	} 
    else 
    {
		return E_FAIL;
	}
}

STDMETHODIMP OggMuxInputPin::Receive(IMediaSample* inSample) 
{
	CAutoLock locLock(mParentFilter->mStreamLock);
	LONGLONG locStart = 0;
	LONGLONG locEnd = 0;
	BYTE* locSampleBuff;
	inSample->GetPointer(&locSampleBuff);
	HRESULT locHR = inSample->GetTime(&locStart, &locEnd);
    UNREFERENCED_PARAMETER(locHR);

	//LOG(logDEBUG) <<"Received "<<locStart<<" - "<<locEnd<<endl;
	long locBuffSize = inSample->GetActualDataLength();
	unsigned char* locBuff = new unsigned char[locBuffSize];
	memcpy((void*)locBuff, (const void*)locSampleBuff, inSample->GetActualDataLength());
	StampedOggPacket* locPacket = new StampedOggPacket(locBuff, inSample->GetActualDataLength(), 
                                        false, false, locStart, locEnd, StampedOggPacket::OGG_END_ONLY);
	
	if (mNeedsFLACHeaderCount) 
    {
		mNeedsFLACHeaderCount = false;
		//This is to set the number of headers on the paginator for OggFLAC_1_0
		mPaginator.setNumHeaders( (locPacket->packetData()[8]) + 1 );
	}

    if ((mNeedsFLACHeaderTweak)) 
    {
		//The first packet in FLAC has all the metadata in one block...
		// It needs to be broken up for correct muxing....

		//A note about the header formats used for flac in directshow.
		//
		//MEDIASUBTYPE_FLAC
		//	The first packet is all the meta data in one block.
		//	The only filter to output this is the demux.
		//	The demux never outputs type MEDIASUBTYPE_OGG_FLAC_1_0
		//	Even if the input is a new FLAC stream, it is translated before leaving the filter.
		//
		//MEDIASUBTYPE_OggFLAC_1_0
		//	The metadata packets are all seperated.
		//	This is the only format outputted by the encoder
		//
		//

		//If we are in this section of code... it means that the demux has
		// been connected directly to the mux.
		//This could be to mux multi stream flac.
		//Alternatively this configuration could be used to convert the old format to the new.

		//LOG(logDEBUG)<<"In the header tweak section..."<<endl;
		FLACMetadataSplitter* locFLACSplitter = new FLACMetadataSplitter;

		//LOG(logDEBUG)<<"Feeding metadata..."<<endl;
		locFLACSplitter->loadMetadata(locPacket->clone());
		
		//delete locPacket;		//Don't delete the splitter will delete when it's done.

		for (unsigned long i = 0; i < locFLACSplitter->numHeaders(); i++) 
        {
			//LOG(logDEBUG)<<"Giving pager, packet "<<i<<endl;
			//LOG(logDEBUG)<<locFLACSplitter->getHeader(i)->toPackDumpString()<<endl;		//This is a leak !!
			if (i == 0) 
            {
				//Set the number of headers in the paginator for FLAC classic.
				StampedOggPacket* locHeadPack = locFLACSplitter->getHeader(i);
				mPaginator.setNumHeaders((locHeadPack->packetData()[8]) + 1);
				delete locHeadPack;
			}
			mPaginator.acceptStampedOggPacket(locFLACSplitter->getHeader(i));		//This get function returns our copy which we give away.
			//LOG(logDEBUG)<<"After paginator feed..."<<endl;
		}
		mNeedsFLACHeaderTweak = false;
		//LOG(logDEBUG)<<"Pre delete of splitter..."<<endl;
		delete locFLACSplitter;
		//LOG(logDEBUG)<<"Post delete of splitter"<<endl;

	} 
    else 
    {
		//Not truncated or contuned... its a full packet.
		
		//LOG(logDEBUG)<<"Normal add packet..."<<endl;
		mPaginator.acceptStampedOggPacket(locPacket);
	}

	return S_OK;
}

HRESULT OggMuxInputPin::CompleteConnect(IPin* inReceivePin) 
{	
	//Set our delegate to the pin that is connecting to us... we'll send them our seek messages.
	IMediaSeeking* locSeeker = NULL;
	inReceivePin->QueryInterface(IID_IMediaSeeking, (void**)&locSeeker);
	SetDelegate(locSeeker);
	
	mMuxStream->setIsActive(true);
	return mParentFilter->addAnotherPin();
}

HRESULT OggMuxInputPin::BreakConnect() 
{
	CAutoLock locLock(m_pLock);
	//Release the seeking delegate
	ReleaseDelegate();
	return CBaseInputPin::BreakConnect();
}

STDMETHODIMP OggMuxInputPin::EndOfStream(void) 
{	
    CAutoLock locLock(mParentFilter->mStreamLock);
	mPaginator.finishStream();
	mMuxStream->setIsEOS(true);
	
	//HRESULT locHR = mParentFilter->NotifyEvent(EC_COMPLETE, S_OK, NULL);
	return S_OK;
}

unsigned long OggMuxInputPin::PaginatorMaximumPacketsPerPage()
{
	return mPaginator.parameters()->mMaxPacksPerPage;
}

void OggMuxInputPin::SetPaginatorMaximumPacketsPerPage(unsigned long inMaxPacketsPerPage)
{
	mPaginator.parameters()->mMaxPacksPerPage = inMaxPacketsPerPage;
}
