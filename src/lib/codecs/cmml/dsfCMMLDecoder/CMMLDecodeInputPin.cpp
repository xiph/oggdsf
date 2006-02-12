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
#include "CMMLDecodeInputPin.h"
#include "CMMLDecodeFilter.h"

CMMLDecodeInputPin::CMMLDecodeInputPin(CMMLDecodeFilter* inParentFilter, HRESULT* outHR)
	:	CTransformInputPin(NAME("CMMLDecodeInputPin"), inParentFilter, outHR, L"CMML In")
	,	mSetupState(VSS_SEEN_NOTHING)
	,	mCMMLFormatBlock(NULL)
	,	mCMMLHeadPacket(NULL)
{

}

CMMLDecodeInputPin::~CMMLDecodeInputPin(void)
{
	delete mCMMLFormatBlock;
	delete mCMMLHeadPacket;
}

STDMETHODIMP CMMLDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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

HRESULT CMMLDecodeInputPin::SetMediaType(const CMediaType* inMediaType) 
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

HRESULT CMMLDecodeInputPin::CheckMediaType(const CMediaType *inMediaType)
{
	if (		(inMediaType->majortype == MEDIATYPE_OggPacketStream)
			&&	(inMediaType->subtype == MEDIASUBTYPE_None)
			&&	(inMediaType->formattype == FORMAT_OggIdentHeader)) {
		if (inMediaType->cbFormat == CMML_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "CMML\000\000\000\000", 8) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			}
		}
	}
	return S_FALSE;
	
}
HRESULT CMMLDecodeInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = CMML_BUFFER_SIZE;
	outRequestedProps->cBuffers = CMML_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}
LOOG_INT64 CMMLDecodeInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
	//return inGranule * mVideoFormatBlock->AvgTimePerFrame;
	LOOG_INT64 locMask = 0xffffffffffffffff >> (64 - mCMMLFormatBlock->granuleSplitBits);
	LOOG_INT64 locTime = ((inGranule & locMask) + (inGranule >> mCMMLFormatBlock->granuleSplitBits));

	return ((locTime * mCMMLFormatBlock->granuleDenominator * UNITS) / mCMMLFormatBlock->granuleNumerator);
}

LOOG_INT64 CMMLDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}
IOggDecoder::eAcceptHeaderResult CMMLDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	switch (mSetupState) {
		case VSS_SEEN_NOTHING:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "CMML\000\000\000\000", 0) == 0) {
				handleHeaderPacket(inCodecHeaderPacket);
				mSetupState = VSS_SEEN_BOS;
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			}
			mSetupState = VSS_ERROR;
			return IOggDecoder::AHR_INVALID_HEADER;
			
		case VSS_SEEN_BOS:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "<?xml", 5) == 0) {
				mSetupState = VSS_SEEN_XML_HEADER;
				
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			}
			return IOggDecoder::AHR_INVALID_HEADER;
		
		case VSS_SEEN_XML_HEADER:
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "<head", 5) == 0) {
				mSetupState = VSS_ALL_HEADERS_SEEN;
				//handleHeadPacket(inCodecHeaderPacket->clone())
				mCMMLHeadPacket = inCodecHeaderPacket->clone();
				return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
			}
			return IOggDecoder::AHR_INVALID_HEADER;
		case VSS_ALL_HEADERS_SEEN:
		case VSS_ERROR:
		default:
			return IOggDecoder::AHR_UNEXPECTED;
	}
}



bool CMMLDecodeInputPin::handleHeaderPacket(OggPacket* inHeaderPack)
{
	delete mCMMLFormatBlock;
	mCMMLFormatBlock = new sCMMLFormatBlock;
	mCMMLFormatBlock->granuleNumerator = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 12);
	mCMMLFormatBlock->granuleDenominator = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 20);
	mCMMLFormatBlock->granuleSplitBits = inHeaderPack->packetData()[28];
	return true;
	



}
string CMMLDecodeInputPin::getCodecShortName()
{
	return "CMML";
}
string CMMLDecodeInputPin::getCodecIdentString()
{
	//TODO:::
	return "CMML";
}

