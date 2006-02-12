//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "OGMDecodeInputPin.h"

OGMDecodeInputPin::OGMDecodeInputPin(OGMDecodeFilter* inParentFilter, HRESULT* outHR)
	:	CTransformInputPin(NAME("OGMDecodeInputPin"), inParentFilter, outHR, L"OGM In")
	,	mVideoFormatBlock(NULL)
	,	mAudioFormatBlock(NULL)
	,	mSetupState(VSS_SEEN_NOTHING)
	,	mOGMMediaType(OGM_UNKNOWN_TYPE)
	,	mGranuleRateNumerator(1)
	,	mGranuleRateDenominator(0)
{

}

OGMDecodeInputPin::~OGMDecodeInputPin(void)
{

}

STDMETHODIMP OGMDecodeInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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

HRESULT OGMDecodeInputPin::SetMediaType(const CMediaType* inMediaType) 
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

HRESULT OGMDecodeInputPin::CheckMediaType(const CMediaType* inMediaType)
{
	//VIDSPEC::: Use the audio and text ident fields
	if (		(inMediaType->majortype == MEDIATYPE_OggPacketStream)
			&&	(inMediaType->subtype == MEDIASUBTYPE_None)
			&&	(inMediaType->formattype == FORMAT_OggIdentHeader)) {
				//TODO::: The header size is not fixed. This is just a minimum size.
				//			need to look in the size field to determine the actual
				//			size of the header as some codecs can include extra data
				//			after the ogm headers
				//PARTIAL RESOLUTION::: Now a minimum size but should still check for
				//			validity by checking the size field
		if (inMediaType->cbFormat >= OGM_IDENT_HEADER_SIZE) {
			if (strncmp((char*)inMediaType->pbFormat, "\001video\000\000\000", 9) == 0) {
				//TODO::: Possibly verify version
				return S_OK;
			} else if (strncmp((char*)inMediaType->pbFormat, "\001audio\000\000\000", 9) == 0) {
				return S_OK;
			} else if (strncmp((char*)inMediaType->pbFormat, "\001text\000\000\000\000", 9) == 0) {
				return S_OK;
			}
		}
	}
	return S_FALSE;
	
}
HRESULT OGMDecodeInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *outRequestedProps)
{
	outRequestedProps->cbBuffer = OGM_BUFFER_SIZE;
	outRequestedProps->cBuffers = OGM_NUM_BUFFERS;
	outRequestedProps->cbAlign = 1;
	outRequestedProps->cbPrefix = 0;

	return S_OK;
}
LOOG_INT64 OGMDecodeInputPin::convertGranuleToTime(LOOG_INT64 inGranule)
{
	switch (mOGMMediaType) {
		case OGM_VIDEO_TYPE:
			return inGranule * mVideoFormatBlock->AvgTimePerFrame;
		case OGM_AUDIO_TYPE:
			return (inGranule * UNITS) / mAudioFormatBlock->nSamplesPerSec;
		case OGM_TEXT_TYPE:
			return (inGranule * UNITS * mGranuleRateDenominator) / mGranuleRateNumerator;
		default:
			return 0;
	};
	
}

LOOG_INT64 OGMDecodeInputPin::mustSeekBefore(LOOG_INT64 inGranule)
{
	//TODO::: Get adjustment from block size info... for now, it doesn't matter if no preroll
	return inGranule;
}
IOggDecoder::eAcceptHeaderResult OGMDecodeInputPin::showHeaderPacket(OggPacket* inCodecHeaderPacket)
{
	switch (mSetupState) {
		case VSS_SEEN_NOTHING:
			//VIDSPEC::: Also accept the audio and text headers
			if (strncmp((char*)inCodecHeaderPacket->packetData(), "\001video\000\000\000", 9) == 0) {
				mOGMMediaType = OGM_VIDEO_TYPE;
				handleHeaderPacket(inCodecHeaderPacket);
				mSetupState = VSS_SEEN_BOS;
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			} else if (strncmp((char*)inCodecHeaderPacket->packetData(), "\001audio\000\000\000", 9) == 0) {
				//TODO::: Verify that there are secondary comment headers for audio and text
				mOGMMediaType = OGM_AUDIO_TYPE;
				handleHeaderPacket(inCodecHeaderPacket);
				mSetupState = VSS_SEEN_BOS;
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			} else if (strncmp((char*)inCodecHeaderPacket->packetData(), "\001text\000\000\000\000", 9) == 0) {
				mOGMMediaType = OGM_TEXT_TYPE;
				handleHeaderPacket(inCodecHeaderPacket);
				mSetupState = VSS_SEEN_BOS;
				return IOggDecoder::AHR_MORE_HEADERS_TO_COME;
			} else {
				mOGMMediaType = OGM_UNKNOWN_TYPE;
			}
			mSetupState = VSS_ERROR;
			return IOggDecoder::AHR_INVALID_HEADER;
			
		case VSS_SEEN_BOS:
			if (inCodecHeaderPacket->packetData()[0] == 0x03) {
				mSetupState = VSS_ALL_HEADERS_SEEN;
				return IOggDecoder::AHR_ALL_HEADERS_RECEIVED;
			}
			return IOggDecoder::AHR_INVALID_HEADER;
		
	
		case VSS_ALL_HEADERS_SEEN:
		case VSS_ERROR:
		default:
			return IOggDecoder::AHR_UNEXPECTED;
	}
}

bool OGMDecodeInputPin::handleHeaderPacket(OggPacket* inHeaderPack)
{
	switch(mOGMMediaType) {
		case OGM_VIDEO_TYPE:
			return handleVideoHeaderPacket(inHeaderPack);
		case OGM_AUDIO_TYPE:
			return handleAudioHeaderPacket(inHeaderPack);

		case OGM_TEXT_TYPE:
			return handleTextHeaderPacket(inHeaderPack);
		default:
			return false;
		
	};
}

bool OGMDecodeInputPin::handleTextHeaderPacket(OggPacket* inHeaderPack)
{
	mGranuleRateNumerator = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 17);
	mGranuleRateDenominator = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 25);
	
	//TODO:::
	return true;
}
bool OGMDecodeInputPin::handleAudioHeaderPacket(OggPacket* inHeaderPack)
{
	delete mAudioFormatBlock;

	//TODO::: This will cause a problem with extra header information appended.
	//			need to change to byte* and cast upon use
	mAudioFormatBlock = new WAVEFORMATEX;

	//FORMAT OF AUDIO HEADER IN OGM BOS
	//-----------------------------------------

	//0		-		8		=		STRING		Stream Type (char[8] is NULL) \001audio
	//9		-		12		=		STRING		Sub Type (format code)
	//13	-		16		=		INT32		Size of structure ???
	//17	-		24		=		INT64		Time per "unit" in DSHOW UNITS
	//25	-		32		=		INT64		Samples per "unit"
	//33	-		36		=		INT32		Deefault length ????? in media type ?? WTF ?
	//37	-		40		=		INT32		Buffer Size
	//41	-		42		=		INT16		Bits per sample
	//43	-		44		=		-----		HOLE IN DATA
	//45	-		46		=		INT16		Channel count
	//47	-		48		=		INT16		Block Align
	//49	-		52		=		INT32		Avg Bytes per Second

	mAudioFormatBlock->nAvgBytesPerSec = iLE_Math::charArrToULong(inHeaderPack->packetData() + 49);
	mAudioFormatBlock->nChannels = iLE_Math::charArrToUShort(inHeaderPack->packetData() + 45);
	mAudioFormatBlock->nBlockAlign = iLE_Math::charArrToUShort(inHeaderPack->packetData() + 47);
	
	//TODO::: Check if this needs to be div/mul by the time per unit field
	mAudioFormatBlock->nSamplesPerSec = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 25);

	mAudioFormatBlock->wBitsPerSample = iLE_Math::charArrToUShort(inHeaderPack->packetData() + 41);
	
	//TODO::: Extra header size
	mAudioFormatBlock->cbSize = sizeof(WAVEFORMATEX);

	
	unsigned short locFormatCode = 0;
	unsigned char locHexDigitValue = 0;
	for (int i = 0; i < 4; i++) {
		locFormatCode <<= 4;
		locHexDigitValue = inHeaderPack->packetData()[9 + i];

		if ((locHexDigitValue >= '0') && (locHexDigitValue <= '9')) {
			locHexDigitValue -= '0';
		} else if ((locHexDigitValue >= 'a') && (locHexDigitValue <= 'f')) {
			locHexDigitValue -= 'a';
			locHexDigitValue += 10;
		} else if ((locHexDigitValue >= 'A') && (locHexDigitValue <= 'F')) {
			locHexDigitValue -= 'A';
			locHexDigitValue += 10;
		} else {
			//This is a fatal error.
			throw 0;
		}

		locFormatCode += locHexDigitValue;
	}

	mAudioFormatBlock->wFormatTag = locFormatCode;

	return true;

}
bool OGMDecodeInputPin::handleVideoHeaderPacket(OggPacket* inHeaderPack)
{
	//VIDSPEC::: This whole thing is specific to video
	delete mVideoFormatBlock;
	mVideoFormatBlock = new VIDEOINFOHEADER;

	//FORMAT OF VIDEO HEADER IN OGM BOS
	//-----------------------------------------

	//0		-		8		=		STRING		Stream Type (char[8] is NULL) \001video
	//9		-		12		=		STRING		Sub Type (FOURCC)
	//13	-		16		=		INT32		Size of structure ???
	//17	-		24		=		INT64		Time per "unit" in DSHOW UNITS
	//25	-		32		=		INT64		Samples per "unit"
	//33	-		36		=		INT32		Deefault length ????? in media type ?? WTF ?
	//37	-		40		=		INT32		Buffer Size
	//41	-		42		=		INT16		Bits per sample
	//43	-		44		=		-----		HOLE IN DATA
	//45	-		48		=		INT32		Video Width
	//49	-		52		=		INT32		Video Height



 

	//------------------------------------------

	
	__int64 locTimePerBlock = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 17);

	
	__int64 locSamplesPerBlock = iLE_Math::CharArrToInt64(inHeaderPack->packetData() + 25);

	//debugLog<<"t/block = "<<locTimePerBlock<<"        Sam/block = "<<locSamplesPerBlock<<endl;

	mVideoFormatBlock->AvgTimePerFrame = locTimePerBlock / locSamplesPerBlock;

	//debugLog<<"Time per frame = "<<mVideoFormatBlock->AvgTimePerFrame<<endl;

	__int64 locFPSec = (UNITS / locTimePerBlock) * locSamplesPerBlock;

	//debugLog<<"Rate = "<<locFPSec<<" fps"<<endl;
	unsigned short locBPSample = ((unsigned char)(inHeaderPack->packetData()[41])) + (((unsigned short)(inHeaderPack->packetData()[42])) * 256);

	
	
	mVideoFormatBlock->bmiHeader.biBitCount = locBPSample;
	mVideoFormatBlock->bmiHeader.biClrImportant = 0;   //All colours important
	mVideoFormatBlock->bmiHeader.biClrUsed = 0;        //Use max colour depth
	mVideoFormatBlock->bmiHeader.biCompression = MAKEFOURCC(inHeaderPack->packetData()[9], inHeaderPack->packetData()[10], inHeaderPack->packetData()[11], inHeaderPack->packetData()[12]);;

	unsigned long locHeight = iLE_Math::charArrToULong(inHeaderPack->packetData() + 49);
	unsigned long locWidth =  iLE_Math::charArrToULong(inHeaderPack->packetData() + 45);

	mVideoFormatBlock->dwBitRate = 0;

	mVideoFormatBlock->bmiHeader.biHeight = locHeight;
	mVideoFormatBlock->bmiHeader.biPlanes = 1;    //Must be 1
	mVideoFormatBlock->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);    //????? Size of what ?
	mVideoFormatBlock->bmiHeader.biSizeImage = (locHeight * locWidth * locBPSample) / 8;    //Size in bytes of image ??
	mVideoFormatBlock->bmiHeader.biWidth = locWidth;
	mVideoFormatBlock->bmiHeader.biXPelsPerMeter = 2000;   //Fuck knows
	mVideoFormatBlock->bmiHeader.biYPelsPerMeter = 2000;   //" " " " " 
	
	mVideoFormatBlock->rcSource.top = 0;
	mVideoFormatBlock->rcSource.bottom = locHeight;
	mVideoFormatBlock->rcSource.left = 0;
	mVideoFormatBlock->rcSource.right = locWidth;

	mVideoFormatBlock->rcTarget.top = 0;
	mVideoFormatBlock->rcTarget.bottom = locHeight;
	mVideoFormatBlock->rcTarget.left = 0;
	mVideoFormatBlock->rcTarget.right = locWidth;

	mVideoFormatBlock->dwBitErrorRate=0;
	return true;
}
string OGMDecodeInputPin::getCodecShortName()
{
	//TODO::: These should also include the information about the fourcc or format code
	return "OGM";
}
string OGMDecodeInputPin::getCodecIdentString()
{
	//TODO:::
	return "OGM";
}

