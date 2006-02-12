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

#include "StdAfx.h"
#include "OGMDecodeFilter.h"



//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
		L"OGM Decode Filter",					// Name
	    &CLSID_OGMDecodeFilter,				// CLSID
	    OGMDecodeFilter::CreateInstance,		// Method to create an instance of Speex Decoder
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

OGMDecodeFilter::OGMDecodeFilter()
	:	CTransformFilter(NAME("OGM Video Decoder"), NULL, CLSID_OGMDecodeFilter)
	,	mInputPin(NULL)
	,	mOutputPin(NULL)
	,	mOGMGranulesBuffered(0)
	,	mSegStart(0)
	,	mSegEnd(0)
	,	mSegRate(0)
	
{

	
}



OGMDecodeFilter::~OGMDecodeFilter(void)
{

}

CUnknown* WINAPI OGMDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	//This routine is the COM implementation to create a new Filter
	OGMDecodeFilter *pNewObject = new OGMDecodeFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
	return pNewObject;
} 

HRESULT OGMDecodeFilter::CheckInputType(const CMediaType* inMediaType)
{
	return mInputPin->CheckMediaType(inMediaType);
}
HRESULT OGMDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType)
{

	return S_OK;
}
HRESULT OGMDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest)
{
	HRESULT locHR = S_OK;

	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;
	
	if (inPropertyRequest->cbAlign <= 0) {
		locReqAlloc.cbAlign = 1;
	} else {
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}


	if (inPropertyRequest->cbBuffer == 0) {
		//VIDSPEC::: Smaller for audio and text
		locReqAlloc.cbBuffer = 65536*16;
	} else {
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}
	

	if (inPropertyRequest->cbPrefix < 0) {
			locReqAlloc.cbPrefix = 0;
	} else {
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}
	
	if (inPropertyRequest->cBuffers == 0) {
		locReqAlloc.cBuffers = 5;
	} else {
		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (locHR != S_OK) {
		//TODO::: Handle a fail state here.
		return locHR;
	} else {
		//TODO::: Need to save this pointer to decommit in destructor ???
		locHR = inAllocator->Commit();

	
		return locHR;
	}
	
}

HRESULT OGMDecodeFilter::NewSegment(REFERENCE_TIME inStartTime, REFERENCE_TIME inStopTime, double inRate)
{
	mSegStart = inStartTime;
	mSegEnd = inStopTime;
	mSegRate = inRate;
	return CTransformFilter::NewSegment(inStartTime, inStopTime, inRate);
}
HRESULT OGMDecodeFilter::GetMediaType(int inPosition, CMediaType* outMediaType)
{
	
	if (inPosition < 0) {
		return E_INVALIDARG;
	}
	
	if ((inPosition == 0) && (mInputPin != NULL) && (mInputPin->IsConnected())) {

		//VIDSPEC:::This needs cases for audio and text
		switch(mInputPin->getOGMMediaType()) {
			case OGMDecodeInputPin::OGM_VIDEO_TYPE:
			{
				VIDEOINFOHEADER* locVideoFormat = (VIDEOINFOHEADER*)outMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
				*locVideoFormat = *mInputPin->getVideoFormatBlock();
				//FillMediaType(outMediaType, locVideoFormat->bmiHeader.biSizeImage);
				outMediaType->majortype = MEDIATYPE_Video;
				outMediaType->subtype = (GUID)(FOURCCMap(locVideoFormat->bmiHeader.biCompression));;
				outMediaType->formattype = FORMAT_VideoInfo;
				//TODO:::Handle temproal compression and variable size field

			}
			break;
				
			case OGMDecodeInputPin::OGM_AUDIO_TYPE:
			{
				WAVEFORMATEX* locAudioFormat = (WAVEFORMATEX*)outMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
				*locAudioFormat = *mInputPin->getAudioFormatBlock();
				outMediaType->majortype = MEDIATYPE_Audio;
				outMediaType->subtype = MEDIASUBTYPE_PCM;
				outMediaType->subtype.Data1 = locAudioFormat->wFormatTag;
				outMediaType->formattype = FORMAT_WaveFormatEx;
			}

			break;
			case OGMDecodeInputPin::OGM_TEXT_TYPE:

				outMediaType->majortype = MEDIATYPE_Text;
				outMediaType->subtype = MEDIASUBTYPE_None;
				outMediaType->formattype = FORMAT_None;
				break;

			default:
				return E_FAIL;

		}


		return S_OK;
	} else {
		return VFW_S_NO_MORE_ITEMS;
	}


}

HRESULT OGMDecodeFilter::Receive(IMediaSample* inSample)
{
	//VIDSPEC:::
	BYTE* locInBuff = NULL;
	HRESULT locHR = inSample->GetPointer(&locInBuff);

	if (locHR == S_OK) {
		//Get the time input
		REFERENCE_TIME locStart = -1;
		REFERENCE_TIME locEnd = -1;
		inSample->GetTime(&locStart, &locEnd);

		//Do nothing if it's a header packet
		if ((locInBuff[0] & 1) != 0) {
			return S_OK;
		}

		//Get the sample length and setup some buffers
		unsigned long locLength = inSample->GetActualDataLength();
		unsigned char* locBuff = new unsigned char[locLength];
		sSimplePack locPack;
		memcpy((void*)locBuff, (const void*)locInBuff, locLength);
		locPack.mBuff = locBuff;
		locPack.mLength = locLength;

		
		//Find out how many bytes of the header are the length field
		unsigned long locNumLenBytes = locInBuff[0];
		const unsigned char LEN_MASK = 0xC2; //11000010
		locNumLenBytes &= LEN_MASK;
		locNumLenBytes = (locNumLenBytes >> 6) | ((locNumLenBytes&2) << 1);

		//Find out how many frames this will gnerate
		//locPackTime represents different things for text, audio and video
		//
		//video	-	number of frames duration (can be more than 1 frame generated from a packet)
		//audio	-	number of samples generated by this packet
		//text	-	duration of text in 1ms units (ie 1000 = 1sec)
		__int64 locPackTime = 0;
		if (locNumLenBytes != 0) {
			for (int i = 0; i <  locNumLenBytes; i++) {
				locPackTime |= ((__int64)locInBuff[1+i] << (i * 8));
			}
		} else {
			locPackTime = 1;
		}
		
		//Buffer up the packet	-	
		mOGMGranulesBuffered += locPackTime;			//VS:::
		locPack.mDuration = locPackTime;		//VS:::
		locPack.mHeaderSize = locNumLenBytes + 1;
		locPack.mIsKeyframe = ((locInBuff[0] & (1<<3)) != 0);

		mPacketBuffer.push_back(locPack);

		if (locEnd > 0) {
			REFERENCE_TIME locGlobalStart = 0;
			REFERENCE_TIME locGlobalEnd = 0;

			//__int64 locFrameDuration = mInputPin->getVideoFormatBlock()->AvgTimePerFrame;		//VS:::
			__int64 locNumBuffered = mPacketBuffer.size();

			locGlobalEnd = mInputPin->convertGranuleToTime(locEnd); //locEnd * locFrameDuration;											//VS:::
			locGlobalStart = locGlobalEnd - (mInputPin->convertGranuleToTime(mOGMGranulesBuffered));//locGlobalEnd - (mOGMGranulesBuffered * locFrameDuration);				//VS:::

			__int64 locUptoStart = locGlobalStart;
			__int64 locUptoEnd = locGlobalStart;

			__int64 locAdjustedStart = 0;
			__int64 locAdjustedEnd = 0;
			for (int i = 0; i < locNumBuffered; i++) {
				IMediaSample* locOutSample = NULL;
				
				locHR = InitializeOutputSample(inSample, &locOutSample);
				if (locHR == S_OK) {
					locUptoEnd = locUptoStart + (mInputPin->convertGranuleToTime(mPacketBuffer[i].mDuration));//locUptoStart + (mPacketBuffer[i].mDuration * locFrameDuration);		//VS:::

					locAdjustedStart = locUptoStart - mSegStart;
					locAdjustedEnd = locUptoEnd - mSegStart;
					locOutSample->SetTime(&locAdjustedStart, &locAdjustedEnd);
					locOutSample->SetMediaTime(&locAdjustedStart, &locAdjustedEnd);
					locOutSample->SetSyncPoint(mPacketBuffer[i].mIsKeyframe);
					locOutSample->SetActualDataLength(mPacketBuffer[i].mLength - mPacketBuffer[i].mHeaderSize);
					BYTE* locOutBuff = NULL;
					locOutSample->GetPointer(&locOutBuff);
					memcpy((void*)locOutBuff, (const void*)(mPacketBuffer[i].mBuff + mPacketBuffer[i].mHeaderSize), mPacketBuffer[i].mLength - mPacketBuffer[i].mHeaderSize);
					locHR = m_pOutput->Deliver(locOutSample);
					locOutSample->Release();

					if (locHR != S_OK) {
						deleteBufferedPackets();
						return S_FALSE;
					}

					
					locUptoStart = locUptoEnd;

					
				} else {
					deleteBufferedPackets();
					return S_FALSE;
				}

			}

			deleteBufferedPackets();
			return S_OK;





		} else {
			return S_OK;
		}

	} else {
		return locHR;
	}
}

void OGMDecodeFilter::deleteBufferedPackets()
{
	for (size_t i = 0; i < mPacketBuffer.size(); i++) {
		delete[] mPacketBuffer[i].mBuff;
	}
	mPacketBuffer.clear();
	mOGMGranulesBuffered = 0;
}
HRESULT OGMDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* inOutputSample)
{

	//BYTE* locInBuff = NULL;
	//HRESULT locHR = inInputSample->GetPointer(&locInBuff);

	//if (locHR == S_OK) {
	//	REFERENCE_TIME locStart = -1;
	//	REFERENCE_TIME locEnd = -1;
	//	inInputSample->GetTime(&locStart, &locEnd);
	//	unsigned long locLength = inInputSample->GetActualDataLength();
	//	unsigned char* locBuff = new unsigned char[locLength];
	//	sSimplePack locPack;
	//	memcpy((void*)locBuff, (const void*)locInBuff, locLength);
	//	locPack.mBuff = locBuff;
	//	locPack.mLength = locLength;
	//	mPacketBuffer.push_back(sSimplePack);



	//	

	//}


	return S_OK;
}

CBasePin* OGMDecodeFilter::GetPin(int inPinNo)
{

    HRESULT locHR = S_OK;

    // Create an input pin if necessary

    if (m_pInput == NULL) {

        m_pInput = new OGMDecodeInputPin(this, &locHR);		//Deleted in base destructor

        
        if (m_pInput == NULL) {
            return NULL;
        }
		//VIDSPEC::: Change the name dynamically if audio or text
		mInputPin = (OGMDecodeInputPin*)m_pInput;
        m_pOutput = new CTransformOutputPin(NAME("OGM Out"), this, &locHR, L"Video Out");	//Deleted in base destructor
			

        if (m_pOutput == NULL) {
            delete m_pInput;
            m_pInput = NULL;
        }
    }

    // Return the pin

    if (inPinNo == 0) {
        return m_pInput;
    } else if (inPinNo == 1) {
        return m_pOutput;
    } else {
        return NULL;
    }

}