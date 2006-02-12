//===========================================================================
//Copyright (C) 2003, 2004, 2005 Zentaro Kavanagh
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
#include "OggRawAudioExtractorInputPin.h"

OggRawAudioExtractorInputPin::OggRawAudioExtractorInputPin(OggRawAudioExtractorFilter* inParentFilter, HRESULT* outHR)
	:	CTransformInputPin(NAME("OggAudioExtractorInputPin"), inParentFilter, outHR, L"Ogg Raw Audio In")

	,	mSetupState(VSS_SEEN_NOTHING)
{

}

OggRawAudioExtractorInputPin::~OggRawAudioExtractorInputPin(void)
{

}

STDMETHODIMP OggRawAudioExtractorInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSeeking) {
		*ppv = (IMediaSeeking*)this;
		((IUnknown*)*ppv)->AddRef();
		return NOERROR;
	} else if (riid == IID_IOggDecoder) {
		*ppv = (IOggDecoder*)this;
		//((IUnknown*)*ppv)->AddRef();
		return NOERROR;

	}

	return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv); 
}

HRESULT OggRawAudioExtractorInputPin::SetMediaType(const CMediaType* inMediaType) 
{
	//FIX:::Error checking
	//RESOLVED::: Bit better.
	if (CheckMediaType(inMediaType) == S_OK) {
		//((OGMDecodeFilter*)m_pFilter)->setOGMFormat(inMediaType->pbFormat);
		
	} else {
		throw 0;
	}
	return CBaseInputPin::SetMediaType(inMediaType);

}

HRESULT OggRawAudioExtractorInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (		(inMediaType->majortype == MEDIATYPE_OggPacketStream)
			&&	(inMediaType->subtype == MEDIASUBTYPE_None)
			&&	(inMediaType->formattype == FORMAT_OggIdentHeader)) {
		if (inMediaType->cbFormat == OGG_RAW_AUDIO_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "PCM     ", 8) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			}
		}
	}
	return S_FALSE;
	
}
HRESULT OggRawAudioExtractorInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = 65536;
	outRequestedProps->cBuffers = OGG_RAW_AUDIO_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}
LOOG_INT64 OggRawAudioExtractorInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
	return (inGranule * UNITS) / mFormatBlock.samplesPerSec;
}

LOOG_INT64 OggRawAudioExtractorInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}
IOggDecoder::eAcceptHeaderResult OggRawAudioExtractorInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	switch (mSetupState) {
		case VSS_SEEN_NOTHING:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "PCM     ", 8) == 0) {
				handleHeaderPacket(inCodecHeaderPacket);
				mSetupState = VSS_SEEN_BOS;
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			}
			mSetupState = VSS_ERROR;
			return IOggDecoder::AHR_INVALID_HEADER;
			
		case VSS_SEEN_BOS:
			//TODO::: Handle extra headers
			mSetupState = VSS_ALL_HEADERS_SEEN;
			return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
			
			//return IOggDecoder::AHR_INVALID_HEADER;
		
	
		case VSS_ALL_HEADERS_SEEN:
		case VSS_ERROR:
		default:
			return IOggDecoder::AHR_UNEXPECTED;
	}
}



bool OggRawAudioExtractorInputPin::handleHeaderPacket(OggPacket* inHeaderPack)
{
	
	//mVideoFormatBlock = new VIDEOINFOHEADER;

	mFormatBlock.samplesPerSec = iBE_Math::charArrToULong(inHeaderPack->packetData() + 16);
	mFormatBlock.maxFramesPerPacket = iBE_Math::charArrToULong(inHeaderPack->packetData() + 22) >> 16;
	mFormatBlock.numChannels = inHeaderPack->packetData()[21];
	mFormatBlock.numHeaders = iBE_Math::charArrToULong(inHeaderPack->packetData() + 24);
	
	switch(iBE_Math::charArrToULong(inHeaderPack->packetData() + 12)) {
		case FMT_S8:
		case FMT_U8:
			mFormatBlock.bitsPerSample = 8;
			break;
		case FMT_S16_LE:
		case FMT_S16_BE:
			mFormatBlock.bitsPerSample = 16;
			break;

		default:
			throw 0;

		//case FMT_S24_LE,
		//case FMT_S24_BE,
		//case FMT_S32_LE,
		//case FMT_S32_BE,

		//case FMT_ULAW		=	0x10,
		//case FMT_ALAW,

		//case FMT_FLT32_LE	=	0x20,
		//case FMT_FLT32_BE,
		//case FMT_FLT64_LE,
		//case FMT_FLT64_BE,
		
	}
	
	

	return true;
}
string OggRawAudioExtractorInputPin::getCodecShortName()
{
	return "Ogg Raw Audio";
}
string OggRawAudioExtractorInputPin::getCodecIdentString()
{
	//TODO:::
	return "Ogg Raw Audio";
}

