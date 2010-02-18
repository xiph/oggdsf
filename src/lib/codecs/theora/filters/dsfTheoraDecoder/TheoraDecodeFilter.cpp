//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
//
//Copyright (C) 2003 Commonwealth Scientific and Industrial Research
//Organisation (CSIRO) Australia
//
//Copyright (C) 2008-2009 Cristian Adam
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
#include "TheoraDecodeFilter.h"
#include "common/util.h"

//COM Factory Template
CFactoryTemplate g_Templates[] = 
{
    { 
        TheoraDecodeFilter::NAME,				// Name
	    &CLSID_TheoraDecodeFilter,				// CLSID
	    TheoraDecodeFilter::CreateInstance,		// Method to create an instance of Theora Decoder
        NULL,									// Initialization function
        &TheoraDecodeFilter::m_filterReg        // Set-up information (for filters)
    }
};

const wchar_t* TheoraDecodeFilter::NAME = L"Xiph.Org Theora Decoder";

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 

const AMOVIESETUP_MEDIATYPE TheoraDecodeFilter::m_outputMediaTypes[] = 
{
    {
        &MEDIATYPE_Video,
        &MEDIASUBTYPE_YV12
    },
    {
        &MEDIATYPE_Video,
        &MEDIASUBTYPE_YUY2
    //},
    //{
    //	&MEDIATYPE_Video,
    //	&MEDIASUBTYPE_RGB565
    //},
    //{
    //	&MEDIATYPE_Video,
    //	&MEDIASUBTYPE_RGB24
    }
};

const AMOVIESETUP_MEDIATYPE TheoraDecodeFilter::m_inputMediaTypes = 
{
    &MEDIATYPE_OggPacketStream,
    &MEDIASUBTYPE_None
};

const AMOVIESETUP_PIN TheoraDecodeFilter::m_pinReg[] = 
{
    {
        L"Theora Input",					//Name (obsoleted)
        FALSE,								//Renders from this pin ?? Not sure about this.
        FALSE,								//Not an output pin
        FALSE,								//Cannot have zero instances of this pin
        FALSE,								//Cannot have more than one instance of this pin
        &GUID_NULL,							//Connects to filter (obsoleted)
        NULL,								//Connects to pin (obsoleted)
        1,									//Supports two media type
        &m_inputMediaTypes				    //Pointer to media type (Video/Theora)
    } ,

    {
        L"Theora Output",					//Name (obsoleted)
        FALSE,								//Renders from this pin ?? Not sure about this.
        TRUE,								//Is an output pin
        FALSE,								//Cannot have zero instances of this pin
        FALSE,								//Cannot have more than one instance of this pin
        &GUID_NULL,							//Connects to filter (obsoleted)
        NULL,								//Connects to pin (obsoleted)
        2,									//Supports two types
        m_outputMediaTypes					//Pointer to media type (YV12)
    }
};

const AMOVIESETUP_FILTER TheoraDecodeFilter::m_filterReg = 
{
    &CLSID_TheoraDecodeFilter,
    NAME,
    MERIT_NORMAL,
    2,
    m_pinReg
};


namespace 
{
	inline unsigned char CLAMP(short v)    
	{
		if (v > 255)
		{
			return 255;
		}
		else if (v < 0)
		{
			return 0;
		}

		return static_cast<unsigned char>(v);
	}
}

TheoraDecodeFilter::TheoraDecodeFilter() 
	:	CTransformFilter(const_cast<wchar_t*>(NAME), NULL, CLSID_TheoraDecodeFilter)
	,	m_pictureWidth(0)
	,	m_pictureHeight(0)
	,	m_bmiFrameSize(0)
	,	m_frameCount(0)
	,	m_yOffset(0)
	,	m_xOffset(0)
	,	m_frameDuration(0)
	,	m_begun(false)
	,	m_seekTimeBase(0)
	,	m_lastSeenStartGranPos(0)

	,	m_segStart(0)
	,	m_segEnd(0)
	,	m_playbackRate(0.0)
	,	m_theoraFormatInfo(NULL)
{
	LOG(logDEBUG) << "Created Theora Decoder Filter";

	m_currentOutputSubType = MEDIASUBTYPE_None;
	sOutputVideoParams videoParams;

	CMediaType* acceptMediaType = NULL;

	//YUY2 Media Type VideoInfo2
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_YUY2;
	acceptMediaType->formattype = FORMAT_VideoInfo2;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 16;
	videoParams.fourCC = MAKEFOURCC('Y','U','Y','2');
	m_outputVideoParams.push_back(videoParams);

	//YUY2 Media Type VideoInfo
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_YUY2;
	acceptMediaType->formattype = FORMAT_VideoInfo;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 16;
	videoParams.fourCC = MAKEFOURCC('Y','U','Y','2');
	m_outputVideoParams.push_back(videoParams);

	//YV12 media type VideoInfo2
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_YV12;
	acceptMediaType->formattype = FORMAT_VideoInfo2;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 12;
	videoParams.fourCC = MAKEFOURCC('Y','V','1','2');
	m_outputVideoParams.push_back(videoParams);

	//YV12 media type VideoInfo
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_YV12;
	acceptMediaType->formattype = FORMAT_VideoInfo;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 12;
	videoParams.fourCC = MAKEFOURCC('Y','V','1','2');
	m_outputVideoParams.push_back(videoParams);
 
	//RGB32 Media Type
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_RGB32;
	acceptMediaType->formattype = FORMAT_VideoInfo;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 32;
	videoParams.fourCC = BI_RGB;
	m_outputVideoParams.push_back(videoParams);

	//RGB565 Media Type
	acceptMediaType = new CMediaType(&MEDIATYPE_Video);		//Deleted in pin destructor
	acceptMediaType->subtype = MEDIASUBTYPE_RGB565;
	acceptMediaType->formattype = FORMAT_VideoInfo;
	m_outputMediaTypesList.push_back(acceptMediaType);

	videoParams.bitsPerPixel = 16;
	videoParams.fourCC = BI_BITFIELDS;
	m_outputVideoParams.push_back(videoParams);

	m_theoraDecoder = new TheoraDecoder;
	m_theoraDecoder->initCodec();
}

TheoraDecodeFilter::~TheoraDecodeFilter() 
{
	for (size_t i = 0; i < m_outputMediaTypesList.size(); i++) 
    {
		delete m_outputMediaTypesList[i];
	}

	delete m_theoraDecoder;
	m_theoraDecoder = NULL;

	delete m_theoraFormatInfo;
	m_theoraFormatInfo = NULL;
}


#ifdef WINCE
LPAMOVIESETUP_FILTER TheoraDecodeFilter::GetSetupData()
{	
	return (LPAMOVIESETUP_FILTER)&m_filterReg;	
}
#endif

CUnknown* WINAPI TheoraDecodeFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
    util::ConfigureLogSettings();

	//This routine is the COM implementation to create a new Filter
	TheoraDecodeFilter *pNewObject = new TheoraDecodeFilter();

    return pNewObject;
}

void TheoraDecodeFilter::FillMediaType(int inPosition, CMediaType* outMediaType, unsigned long inSampleSize) 
{
	outMediaType->SetType(&m_outputMediaTypesList[inPosition]->majortype);
	outMediaType->SetSubtype(&m_outputMediaTypesList[inPosition]->subtype);
	outMediaType->SetFormatType(&m_outputMediaTypesList[inPosition]->formattype);
	outMediaType->SetTemporalCompression(FALSE);
	outMediaType->SetSampleSize(inSampleSize);
}

bool TheoraDecodeFilter::FillVideoInfoHeader(int inPosition, VIDEOINFOHEADER* inFormatBuffer) 
{
	//MTS::: Needs changes for alternate media types. FOURCC and bitCOunt
	TheoraDecodeFilter* locFilter = this;

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->m_theoraFormatInfo->frameRateDenominator) / locFilter->m_theoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->m_theoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = m_outputVideoParams[inPosition].bitsPerPixel;  

	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth

	inFormatBuffer->bmiHeader.biCompression = m_outputVideoParams[inPosition].fourCC;
    inFormatBuffer->bmiHeader.biWidth = locFilter->m_theoraFormatInfo->pictureWidth;
    inFormatBuffer->bmiHeader.biHeight = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   
	inFormatBuffer->bmiHeader.biSizeImage = GetBitmapSize(&inFormatBuffer->bmiHeader);
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 0;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 0;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->m_theoraFormatInfo->pictureWidth;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->m_theoraFormatInfo->pictureWidth;

	inFormatBuffer->dwBitErrorRate=0;
	return true;
}

bool TheoraDecodeFilter::FillVideoInfoHeader2(int inPosition, VIDEOINFOHEADER2* inFormatBuffer) 
{
	//MTS::: Needs changes for alternate media types. FOURCC and bitCOunt
	TheoraDecodeFilter* locFilter = this;

	inFormatBuffer->AvgTimePerFrame = (UNITS * locFilter->m_theoraFormatInfo->frameRateDenominator) / locFilter->m_theoraFormatInfo->frameRateNumerator;
	inFormatBuffer->dwBitRate = locFilter->m_theoraFormatInfo->targetBitrate;
	
	inFormatBuffer->bmiHeader.biBitCount = m_outputVideoParams[inPosition].bitsPerPixel;  

	inFormatBuffer->bmiHeader.biClrImportant = 0;   //All colours important
	inFormatBuffer->bmiHeader.biClrUsed = 0;        //Use max colour depth

	inFormatBuffer->bmiHeader.biCompression = m_outputVideoParams[inPosition].fourCC;
    inFormatBuffer->bmiHeader.biWidth = locFilter->m_theoraFormatInfo->pictureWidth;
    inFormatBuffer->bmiHeader.biHeight = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->bmiHeader.biPlanes = 1;    //Must be 1
	inFormatBuffer->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	inFormatBuffer->bmiHeader.biSizeImage = GetBitmapSize(&inFormatBuffer->bmiHeader);
	inFormatBuffer->bmiHeader.biXPelsPerMeter = 0;   //Fuck knows
	inFormatBuffer->bmiHeader.biYPelsPerMeter = 0;   //" " " " " 
	
	inFormatBuffer->rcSource.top = 0;
	inFormatBuffer->rcSource.bottom = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->rcSource.left = 0;
	inFormatBuffer->rcSource.right = locFilter->m_theoraFormatInfo->pictureWidth;

	inFormatBuffer->rcTarget.top = 0;
	inFormatBuffer->rcTarget.bottom = locFilter->m_theoraFormatInfo->pictureHeight;
	inFormatBuffer->rcTarget.left = 0;
	inFormatBuffer->rcTarget.right = locFilter->m_theoraFormatInfo->pictureWidth;

	inFormatBuffer->dwBitErrorRate=0;

    //Info 2 extensions
    inFormatBuffer->dwInterlaceFlags = 0;
    inFormatBuffer->dwCopyProtectFlags = 0;

    if (m_theoraFormatInfo->aspectNumerator == 0 || m_theoraFormatInfo->aspectDenominator == 0) 
	{
        //Maybe setting to 0?
        inFormatBuffer->dwPictAspectRatioX = m_theoraFormatInfo->pictureWidth;
        inFormatBuffer->dwPictAspectRatioY = m_theoraFormatInfo->pictureHeight;
    } 
	else 
	{
        inFormatBuffer->dwPictAspectRatioX = m_theoraFormatInfo->pictureWidth * m_theoraFormatInfo->aspectNumerator;
        inFormatBuffer->dwPictAspectRatioY = m_theoraFormatInfo->pictureHeight * m_theoraFormatInfo->aspectDenominator;
    }
#ifndef WINCE
    inFormatBuffer->dwControlFlags = 0;
#endif
    inFormatBuffer->dwReserved2 = 0;
	return true;
}

HRESULT TheoraDecodeFilter::CheckInputType(const CMediaType* inMediaType) 
{
	if	( inMediaType->majortype == MEDIATYPE_OggPacketStream &&
		  inMediaType->subtype == MEDIASUBTYPE_None && 
          inMediaType->formattype == FORMAT_OggIdentHeader)
	{
		if (inMediaType->cbFormat == THEORA_IDENT_HEADER_SIZE) 
        {
			if (strncmp((char*)inMediaType->pbFormat, "\200theora", 7) == 0) 
            {
				//TODO::: Possibly verify version
				LOG(logDEBUG) << "Input type ok";
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

HRESULT TheoraDecodeFilter::CheckOutputType(const CMediaType* inMediaType)
{
	for (size_t i = 0; i < m_outputMediaTypesList.size(); i++) 
    {
		if	(inMediaType->majortype == m_outputMediaTypesList[i]->majortype &&
			inMediaType->subtype == m_outputMediaTypesList[i]->subtype &&
			inMediaType->formattype == m_outputMediaTypesList[i]->formattype)
		{
			LOG(logDEBUG) << "Output type ok";
			return S_OK;
		} 
	}
	LOG(logDEBUG) << "Output type no good";

	if (inMediaType->majortype == MEDIATYPE_Video) 
    {
		LOG(logDEBUG) << "Querying for video - FAIL";
		LOG(logDEBUG) << "Sub type = " << inMediaType->subtype;
		LOG(logDEBUG) << "format type = " << inMediaType->formattype;
	} 
    else 
    {
		LOG(logDEBUG) << "Querying for non-video type";
	}

	//If it matched none... return false.
	return S_FALSE;
}

HRESULT TheoraDecodeFilter::CheckTransform(const CMediaType* inInputMediaType, const CMediaType* inOutputMediaType) 
{
	//MTS::: Needs multiple media types
	if (CheckInputType(inInputMediaType) == S_OK && 
        CheckOutputType(inOutputMediaType) == S_OK) 
	{
		if (inOutputMediaType->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* locVideoHeader = (VIDEOINFOHEADER2*)inOutputMediaType->Format();

			m_bmiHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
			m_bmiWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);


			m_bmiFrameSize = (m_bmiHeight * m_bmiWidth * locVideoHeader->bmiHeader.biBitCount) / 8;
		}
		else if (inOutputMediaType->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER* locVideoHeader = (VIDEOINFOHEADER*)inOutputMediaType->Format();

			m_bmiHeight = (unsigned long)abs(locVideoHeader->bmiHeader.biHeight);
			m_bmiWidth = (unsigned long)abs(locVideoHeader->bmiHeader.biWidth);

			m_bmiFrameSize = (m_bmiHeight * m_bmiWidth * locVideoHeader->bmiHeader.biBitCount) / 8;
		}

        LOG(logDEBUG) << "Check transform:";
        LOG(logDEBUG) << "\tbmiWidth: " << m_bmiWidth;
        LOG(logDEBUG) << "\tbmiHeight: " << m_bmiHeight;
        LOG(logDEBUG) << "\tbmiFrameSize: " << m_bmiFrameSize;

		return S_OK;
	} 
    else 
    {
		LOG(logDEBUG) << "Check transform FAILED";
		return S_FALSE;
	}
}

HRESULT TheoraDecodeFilter::DecideBufferSize(IMemAllocator* inAllocator, ALLOCATOR_PROPERTIES* inPropertyRequest) 
{
	HRESULT locHR = S_OK;

	//Create the structures for setproperties to use
	ALLOCATOR_PROPERTIES locReqAlloc;
	ALLOCATOR_PROPERTIES locActualAlloc;

	//MTS::: Maybe this needs to be reconsidered for other output types... ie rgb32 will be much bigger

	const unsigned long MIN_NUM_BUFFERS = 1;
	const unsigned long DEFAULT_NUM_BUFFERS = 1;
	
	//Validate and change what we have been requested to do.
	//Allignment of data
	if (inPropertyRequest->cbAlign <= 0) 
	{
		locReqAlloc.cbAlign = 1;
	} 
    else 
	{
		locReqAlloc.cbAlign = inPropertyRequest->cbAlign;
	}

	//Size of each buffer
	if (m_pOutput->CurrentMediaType().formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2* pvih = (VIDEOINFOHEADER2*)m_pOutput->CurrentMediaType().Format();
		locReqAlloc.cbBuffer = m_theoraFormatInfo->outerFrameHeight * m_theoraFormatInfo->outerFrameWidth *
            pvih->bmiHeader.biBitCount / 8;
	}
	else if (m_pOutput->CurrentMediaType().formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* pvih = (VIDEOINFOHEADER*)m_pOutput->CurrentMediaType().Format();
        locReqAlloc.cbBuffer = m_theoraFormatInfo->outerFrameHeight * m_theoraFormatInfo->outerFrameWidth *
            pvih->bmiHeader.biBitCount / 8;
	}
	else 
	{
		locReqAlloc.cbBuffer = inPropertyRequest->cbBuffer;
	}

	//How many prefeixed bytes
	if (inPropertyRequest->cbPrefix < 0) 
	{
			locReqAlloc.cbPrefix = 0;
	} 
	else 
	{
		locReqAlloc.cbPrefix = inPropertyRequest->cbPrefix;
	}

	//Number of buffers in the allcoator
	if (inPropertyRequest->cBuffers < MIN_NUM_BUFFERS) 
	{
		locReqAlloc.cBuffers = DEFAULT_NUM_BUFFERS;
	} 
	else 
	{
		locReqAlloc.cBuffers = inPropertyRequest->cBuffers;
	}

	//Set the properties in the allocator
	locHR = inAllocator->SetProperties(&locReqAlloc, &locActualAlloc);

	if (FAILED(locHR))
	{
		return locHR;
	}
	
	locHR = inAllocator->Commit();

	if (FAILED(locHR))
	{
		return locHR;
	}

    LOG(logINFO) << __FUNCTIONW__ << " Buffer size: " << locActualAlloc.cbBuffer << ", buffers: " << locActualAlloc.cBuffers;

	return S_OK;
}

HRESULT TheoraDecodeFilter::GetMediaType(int inPosition, CMediaType* outOutputMediaType) 
{
	if (inPosition < 0) 
	{
		return E_INVALIDARG;
	} 
	
    if (inPosition < (int)m_outputMediaTypesList.size()) 
	{	
		if (m_outputMediaTypesList[inPosition]->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* locVideoFormat = (VIDEOINFOHEADER2*)outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER2));
			FillVideoInfoHeader2(inPosition, locVideoFormat);
			FillMediaType(inPosition, outOutputMediaType, locVideoFormat->bmiHeader.biSizeImage);
		}
		else if (m_outputMediaTypesList[inPosition]->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER* locVideoFormat = 0;
			if (m_outputMediaTypesList[inPosition]->subtype == MEDIASUBTYPE_RGB565)
			{
				VIDEOINFO *pvi = (VIDEOINFO *) outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
				if(NULL == pvi)
					return E_OUTOFMEMORY;

				ZeroMemory(pvi, sizeof(VIDEOINFO));
				memcpy(pvi->dwBitMasks, bits565, sizeof(DWORD) * 3);

				locVideoFormat = (VIDEOINFOHEADER*)pvi;
			}
			else
			{
				locVideoFormat = (VIDEOINFOHEADER*)outOutputMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
			}

			FillVideoInfoHeader(inPosition, locVideoFormat);
			FillMediaType(inPosition, outOutputMediaType, locVideoFormat->bmiHeader.biSizeImage);
		}

		LOG(logDEBUG) << "Get Media Type";
		return S_OK;
	} 
	else 
	{
		return VFW_S_NO_MORE_ITEMS;
	}
}

void TheoraDecodeFilter::ResetFrameCount() 
{
	//XTODO::: Maybe not needed
	m_frameCount = 0;
}

HRESULT TheoraDecodeFilter::NewSegment(REFERENCE_TIME inStart, REFERENCE_TIME inEnd, double inRate) 
{
	LOG(logDEBUG) << "Resetting frame count";
	
    ResetFrameCount();
	m_segStart = inStart;
	m_segEnd = inEnd;
	m_playbackRate = inRate;

	return CTransformFilter::NewSegment(inStart, inEnd, inRate);
}

HRESULT TheoraDecodeFilter::Receive(IMediaSample* inInputSample)
{
	BYTE* locBuff = NULL;
	//Get a source poitner into the input buffer
	HRESULT locHR = inInputSample->GetPointer(&locBuff);

	if (locHR != S_OK) 
    {
		//LOG(logDEBUG) << "Receive : Get pointer failed..." << locHR;	
		return S_FALSE;
	}

	if (inInputSample->GetActualDataLength() > 0 && (locBuff[0] & 128) != 0) 
    {
		//inInputSample->Release();

		//This is a header, so ignore it
		return S_OK;
	}
	
    //Make a copy of the packet buffer
	BYTE* locNewBuff = new unsigned char[inInputSample->GetActualDataLength()];		//This gets put into a packet.
	memcpy((void*)locNewBuff, (const void*)locBuff, inInputSample->GetActualDataLength());

	REFERENCE_TIME locStart = 0;
	REFERENCE_TIME locEnd = 0;
	inInputSample->GetTime(&locStart, &locEnd);

	LOG(logDEBUG) << "Theora::Receive - Sample: Size = " << inInputSample->GetActualDataLength() << " Time: " << locStart << " - " << locEnd;

	//This packet is given to the decoder or buffered for later
	StampedOggPacket* locPacket = new StampedOggPacket(locNewBuff, inInputSample->GetActualDataLength(), false, false, locStart, locEnd, StampedOggPacket::OGG_END_ONLY);

	//Buffer all packets, even if we are about to send them anyway
	m_bufferedPackets.push_back(locPacket);

	if (locEnd < 0) 
    {
    	//The packet was ok, but we just aren't going to deliver it yet
		return S_OK;
	} 

    //Now we have one with a stamp, we can send all the previous ones.
	TheoraDecodeInputPin* locInputPin = (TheoraDecodeInputPin*)m_pInput;
	REFERENCE_TIME locGlobalEnd = locInputPin->convertGranuleToTime(locEnd);
	unsigned long locNumBufferedFrames = m_bufferedPackets.size();
	REFERENCE_TIME locGlobalStart = locGlobalEnd - (locNumBufferedFrames * m_frameDuration);

	locStart = locGlobalStart;

	//Offsetting
	REFERENCE_TIME locGlobalOffset = 0;
	//Handle stream offsetting
	if (!locInputPin->GetSentStreamOffset() && locInputPin->GetOutputPinInterface() != NULL) 
    {
		locInputPin->GetOutputPinInterface()->notifyStreamBaseTime(locStart);
		locInputPin->SetSentStreamOffset(true);	
	}

	if (locInputPin->GetOutputPinInterface() != NULL) 
    {
		locGlobalOffset = locInputPin->GetOutputPinInterface()->getGlobalBaseTime();
	}
	
	LOG(logDEBUG) << "Theora::Receive - " << locNumBufferedFrames << " frames buffered";

	for (unsigned long i = 0; i < locNumBufferedFrames; i++) 
    {
		LOG(logDEBUG) << "Theora::Receive - Processing buffered frame " << i;

		bool locIsKeyFrame = m_theoraDecoder->isKeyFrame(m_bufferedPackets[i]);

		LOG(logDEBUG) << "Pre theora decode";
		
        yuv_buffer* locYUV = m_theoraDecoder->decodeTheora(m_bufferedPackets[i]);		//This accept the packet and deletes it
		
        LOG(logDEBUG) << "Post theora decode";
		
        locEnd = locStart + m_frameDuration;
		REFERENCE_TIME locAdjustedStart = locStart - m_segStart - locGlobalOffset;
		REFERENCE_TIME locAdjustedEnd = locEnd - m_segStart - locGlobalOffset;

		if (locAdjustedStart < 0) 
        {
			locAdjustedStart = 0;
		}

		if (locAdjustedEnd >= 0) 
        { 
			if (locYUV != NULL) 
            {
				IMediaSample* locOutSample = NULL;

				LOG(logDEBUG) << "Theora::Receive - Pre output sample initialisation";
				
                locHR = InitializeOutputSample(inInputSample, &locOutSample);
				if (locHR != S_OK) 
                {
					//XTODO::: We need to trash our buffered packets
					LOG(logDEBUG) << "Theora::Receive - Output sample initialisation failed";
					
					DeleteBufferedPacketsAfter(i);
					
					return S_FALSE;
				}
				
                LOG(logDEBUG) << "Theora::Receive - Output sample initialisation suceeded";

				//REFERENCE_TIME locAdjustedStart = (locStart * RATE_DENOMINATOR) / mRateNumerator;
				//REFERENCE_TIME locAdjustedEnd = (locEnd * RATE_DENOMINATOR) / mRateNumerator;

				//Fill the sample info
				if (TheoraDecoded(locYUV, locOutSample, locIsKeyFrame, locAdjustedStart, locAdjustedEnd) != S_OK) 
                {							
					//XTODO::: We need to trash our buffered packets
					locOutSample->Release();
					DeleteBufferedPacketsAfter(i);
					return S_FALSE;
				} 
                else 
                {
					//Deliver the sample
					LOG(logDEBUG) << "Theora::Receive - Delivering: " << locAdjustedStart << " to " << locAdjustedEnd << (locIsKeyFrame ? "KEYFRAME": " ");
					
					locHR = m_pOutput->Deliver(locOutSample);
					ULONG locTempRefCount = locOutSample->Release();

					LOG(logDEBUG) << "Theora::Receive - After deliver refcount = " << locTempRefCount;
					LOG(logDEBUG) << "Theora::Receive - Post delivery";
					
                    if (locHR != S_OK) 
                    {
						//XTODO::: We need to trash our buffered packets
						LOG(logDEBUG) << "Theora::Receive - Delivery failed";
						LOG(logDEBUG) << "Theora::Receive - locHR = " << locHR;

						//locOutSample->Release();
						DeleteBufferedPacketsAfter(i);
						return S_FALSE;
					}
					LOG(logDEBUG) << "Theora::Receive - Delivery Suceeded";
				}
			} 
            else 
            {
				//XTODO::: We need to trash our buffered packets
				LOG(logDEBUG) << "locYUV == NULL";

				DeleteBufferedPacketsAfter(i);
				return S_FALSE;
			}
		}
		locStart = locEnd;
	}

	m_bufferedPackets.clear();

	LOG(logDEBUG) << "Leaving receive method with S_OK";

	return S_OK;
}

void TheoraDecodeFilter::DeleteBufferedPacketsAfter(unsigned long inPacketIndex)
{
	for (size_t i = inPacketIndex + 1; i < m_bufferedPackets.size(); i++) 
    {
		delete m_bufferedPackets[i];
	}

	m_bufferedPackets.clear();
}
HRESULT TheoraDecodeFilter::Transform(IMediaSample* inInputSample, IMediaSample* outOutputSample) 
{
	//LOG(logDEBUG) << "Theora::Transform NOT IMPLEMENTED";
	return E_NOTIMPL;
}

HRESULT TheoraDecodeFilter::DecodeToRGB565(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd)
{
    LOG(logDEBUG) << __FUNCTIONW__;

    BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

    unsigned char * ptry = inYUVBuffer->y + m_yOffset * inYUVBuffer->y_stride;
    unsigned char * ptru = inYUVBuffer->u + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptrv = inYUVBuffer->v + (m_yOffset / 2) * inYUVBuffer->uv_stride;
	unsigned char * ptro = locBuffer;

	for (unsigned long i = 0; i < m_pictureHeight; ++i) 
	{
		unsigned char* ptro2 = ptro;
		for (unsigned long j = m_xOffset; j < m_pictureWidth + m_xOffset; j += 2) 
		{
			short pr, pg, pb, y;
			short r, g, b;

			pr = (-56992 + ptrv[j / 2] * 409) >> 8;
			pg = (34784 - ptru[j / 2] * 100 - ptrv[j / 2] * 208) >> 8;
			pb = (short)((-70688 + ptru[j / 2] * 516) >> 8);

			y = 298*ptry[j] >> 8;
			r = y + pr;
			g = y + pg;
			b = y + pb;
			
			*(unsigned short*)(ptro2) = (unsigned short)	
				(CLAMP(r) >> 3) << 11 |
				(CLAMP(g) >> 2) << 5 |
				 CLAMP(b) >> 3;

			ptro2 += 2;

			y = 298*ptry[j + 1] >> 8;
			r = y + pr;
			g = y + pg;
			b = y + pb;

			*(unsigned short*)(ptro2) = (unsigned short)	
				(CLAMP(r) >> 3) << 11 |
				(CLAMP(g) >> 2) << 5 |
				 CLAMP(b) >> 3;

			ptro2 += 2;
		}
		ptry += inYUVBuffer->y_stride;
		if (i & 1) 
		{
			ptru += inYUVBuffer->uv_stride;
			ptrv += inYUVBuffer->uv_stride;
		}
		ptro += m_bmiWidth * 2;
	}

    SetSampleParams(outSample, m_bmiFrameSize, &inStart, &inEnd, inIsKeyFrame ? TRUE : FALSE);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToRGB32(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd)
{
    LOG(logDEBUG) << __FUNCTIONW__;

	unsigned char* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

    unsigned char * ptry = inYUVBuffer->y + m_yOffset * inYUVBuffer->y_stride;
    unsigned char * ptru = inYUVBuffer->u + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptrv = inYUVBuffer->v + (m_yOffset / 2) * inYUVBuffer->uv_stride;
	unsigned char * ptro = locBuffer;

	for (unsigned long i = 0; i < m_pictureHeight; i++) 
	{
		unsigned char* ptro2 = ptro;
		for (unsigned long j = m_xOffset; j < m_pictureWidth + m_xOffset; j += 2) 
		{
			short pr, pg, pb, y;
			short r, g, b;

			pr = (-56992 + ptrv[j / 2] * 409) >> 8;
			pg = (34784 - ptru[j / 2] * 100 - ptrv[j / 2] * 208) >> 8;
			pb = (short)((-70688 + ptru[j / 2] * 516) >> 8);

			y = 298*ptry[j] >> 8;
			r = y + pr;
			g = y + pg;
			b = y + pb;

			*ptro2++ = CLAMP(b);
			*ptro2++ = CLAMP(g);
			*ptro2++ = CLAMP(r);
			*ptro2++ = 255;

			y = 298*ptry[j + 1] >> 8;
			r = y + pr;
			g = y + pg;
			b = y + pb;

			*ptro2++ = CLAMP(b);
			*ptro2++ = CLAMP(g);
			*ptro2++ = CLAMP(r);
			*ptro2++ = 255;
		}
		ptry += inYUVBuffer->y_stride;
		if (i & 1) 
		{
			ptru += inYUVBuffer->uv_stride;
			ptrv += inYUVBuffer->uv_stride;
		}
		ptro += m_bmiWidth * 4;
	}

    SetSampleParams(outSample, m_bmiFrameSize, &inStart, &inEnd, inIsKeyFrame ? TRUE : FALSE);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToYUY2(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{
    LOG(logDEBUG) << __FUNCTIONW__;

    unsigned char* locBuffer = NULL;
    outSample->GetPointer(&locBuffer);

    unsigned char * ptry = inYUVBuffer->y + m_yOffset * inYUVBuffer->y_stride;
    unsigned char * ptru = inYUVBuffer->u + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptrv = inYUVBuffer->v + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptro = locBuffer;

    for (unsigned long i = 0; i < m_pictureHeight; ++i) 
    {
        unsigned char* ptro2 = ptro;
        for (unsigned long j = m_xOffset; j < m_pictureWidth + m_xOffset; j += 2) 
        {
            *ptro2++ = ptry[j];
            *ptro2++ = ptru[j / 2];
            *ptro2++ = ptry[j + 1];
            *ptro2++ = ptrv[j / 2];
        }

        ptry += inYUVBuffer->y_stride;
        if (i & 1) 
        {
            ptru += inYUVBuffer->uv_stride;
            ptrv += inYUVBuffer->uv_stride;
        }
        ptro += m_bmiWidth * 2;
    }

    SetSampleParams(outSample, m_bmiFrameSize, &inStart, &inEnd, inIsKeyFrame ? TRUE : FALSE);

	return S_OK;
}

HRESULT TheoraDecodeFilter::DecodeToYV12(yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{
    LOG(logDEBUG) << __FUNCTIONW__;

	BYTE* locBuffer = NULL;
	outSample->GetPointer(&locBuffer);

    unsigned char * ptry = inYUVBuffer->y + m_yOffset * inYUVBuffer->y_stride;
    unsigned char * ptru = inYUVBuffer->u + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptrv = inYUVBuffer->v + (m_yOffset / 2) * inYUVBuffer->uv_stride;
    unsigned char * ptro = locBuffer;

	for (unsigned long line = 0; line < m_pictureHeight; ++line) 
    {
		memcpy(ptro, ptry + m_xOffset, m_pictureWidth);

		ptry += inYUVBuffer->y_stride;
		ptro += m_bmiWidth;
	}
	
	for (unsigned long line = 0; line < m_pictureHeight / 2; ++line) 
    {
		memcpy(ptro, ptrv + m_xOffset / 2, m_pictureWidth / 2);
		ptrv += inYUVBuffer->uv_stride;
		ptro += m_bmiWidth / 2;
	}

    for (unsigned long line = 0; line < m_pictureHeight / 2; ++line) 
    {
        memcpy(ptro, ptru + m_xOffset / 2, m_pictureWidth / 2);
        ptru += inYUVBuffer->uv_stride;
        ptro += m_bmiWidth / 2;
    }

    SetSampleParams(outSample, m_bmiFrameSize, &inStart, &inEnd, inIsKeyFrame ? TRUE : FALSE);
	
	return S_OK;
}

HRESULT TheoraDecodeFilter::TheoraDecoded (yuv_buffer* inYUVBuffer, IMediaSample* outSample, bool inIsKeyFrame, REFERENCE_TIME inStart, REFERENCE_TIME inEnd) 
{	
	AM_MEDIA_TYPE* sampleMediaType;
	outSample->GetMediaType(&sampleMediaType);

	static GUID sampleMediaSubType = m_currentOutputSubType;
		
	if (sampleMediaType != NULL)
	{
		sampleMediaSubType = sampleMediaType->subtype;
	}

	DeleteMediaType(sampleMediaType);

	if (sampleMediaSubType == MEDIASUBTYPE_YV12) 
	{
		return DecodeToYV12(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} 
	else if (sampleMediaSubType == MEDIASUBTYPE_YUY2) 
	{
		return DecodeToYUY2(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} 
	else if (sampleMediaSubType == MEDIASUBTYPE_RGB565) 
	{
		return DecodeToRGB565(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} 
	else if (sampleMediaSubType == MEDIASUBTYPE_RGB32) 
	{
		return DecodeToRGB32(inYUVBuffer, outSample, inIsKeyFrame, inStart, inEnd);
	} 
	else 
	{
		LOG(logERROR) << "Decoding to unknown type - failure";
		return E_FAIL;
	}
}

HRESULT TheoraDecodeFilter::SetMediaType(PIN_DIRECTION inDirection, const CMediaType* inMediaType) 
{
	if (inDirection == PINDIR_INPUT) 
    {
		if (CheckInputType(inMediaType) == S_OK) 
        {
			//LOG(logDEBUG) << "Setting format block";
			SetTheoraFormat(inMediaType->pbFormat);
			
            // TODO: after using th_decode_headerin to get the theora informations
            // remove the vertical flip of the yoffset
			m_xOffset = m_theoraFormatInfo->xOffset;
			m_yOffset = m_theoraFormatInfo->outerFrameHeight - m_theoraFormatInfo->pictureHeight - m_theoraFormatInfo->yOffset;

			m_pictureWidth = m_theoraFormatInfo->pictureWidth;
			m_pictureHeight = m_theoraFormatInfo->pictureHeight;

			//How many UNITS does one frame take.
			m_frameDuration = (UNITS * m_theoraFormatInfo->frameRateDenominator) / (m_theoraFormatInfo->frameRateNumerator);

			m_frameCount = 0;
		} 
        else 
        {
			//Failed... should never be here !
			throw 0;
		}
		LOG(logDEBUG) << "SETTING input type";

		return CTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);//CVideoTransformFilter::SetMediaType(PINDIR_INPUT, inMediaType);
	} 
    else 
    {
		m_currentOutputSubType = inMediaType->subtype;
		
        LOG(logDEBUG) << "SETTING output type";
		
        return CTransformFilter::SetMediaType(PINDIR_OUTPUT, inMediaType);//CVideoTransformFilter::SetMediaType(PINDIR_OUTPUT, inMediaType);
	}
}


bool TheoraDecodeFilter::SetSampleParams(IMediaSample* outMediaSample, unsigned long inDataSize, REFERENCE_TIME* inStartTime, REFERENCE_TIME* inEndTime, BOOL inIsSync) 
{
	outMediaSample->SetTime(inStartTime, inEndTime);
	outMediaSample->SetMediaTime(NULL, NULL);
	outMediaSample->SetActualDataLength(inDataSize);
	outMediaSample->SetPreroll(FALSE);
	outMediaSample->SetDiscontinuity(FALSE);
	outMediaSample->SetSyncPoint(inIsSync);

	return true;
}

//BOOL TheoraDecodeFilter::ShouldSkipFrame(IMediaSample* inSample) 
//{
//	//m_bSkipping = FALSE;
//	LOG(logDEBUG) << "Don't skip";
//	return FALSE;
//}

sTheoraFormatBlock* TheoraDecodeFilter::GetTheoraFormatBlock() 
{
	return m_theoraFormatInfo;
}

void TheoraDecodeFilter::SetTheoraFormat(BYTE* inFormatBlock) 
{
	delete m_theoraFormatInfo;
	m_theoraFormatInfo = new sTheoraFormatBlock;			//Deelted in destructor.

    // TODO: replace code blow with th_decode_headerin

	//0		-	55			theora ident						0	-	6
	//56	-	63			ver major							7	-	7
	//64	-	71			ver minor							8	-	8
	//72	-	79			ver subversion						9	=	9
	//80	-	95			width/16							10	-	11
	//96	-	111			height/16							12	-	13
	//112	-	135			framewidth							14	-	16
	//136	-	159			frameheight							17	-	19
	//160	-	167			xoffset								20	-	20
	//168	-	175			yoffset								21	-	21
	//176	-	207			framerateNum						22	-	25
	//208	-	239			frameratedenom						26	-	29
	//240	-	263			aspectNum							30	-	32
	//264	-	287			aspectdenom							33	-	35
	//288	-	295			colourspace							36	-	36
	//296	-	319			targetbitrate						37	-	39
	//320	-	325			targetqual							40	-	40.75
	//326	-	330			keyframintlog						40.75-  41.375

	unsigned char* locIdentHeader = inFormatBlock;
	m_theoraFormatInfo->theoraVersion = (iBE_Math::charArrToULong(locIdentHeader + 7)) >>8;
	m_theoraFormatInfo->outerFrameWidth = (iBE_Math::charArrToUShort(locIdentHeader + 10)) * 16;
	m_theoraFormatInfo->outerFrameHeight = (iBE_Math::charArrToUShort(locIdentHeader + 12)) * 16;
	m_theoraFormatInfo->pictureWidth = (iBE_Math::charArrToULong(locIdentHeader + 14)) >>8;
	m_theoraFormatInfo->pictureHeight = (iBE_Math::charArrToULong(locIdentHeader + 17)) >>8;
	m_theoraFormatInfo->xOffset = locIdentHeader[20];
	m_theoraFormatInfo->yOffset = locIdentHeader[21];
	m_theoraFormatInfo->frameRateNumerator = iBE_Math::charArrToULong(locIdentHeader + 22);
	m_theoraFormatInfo->frameRateDenominator = iBE_Math::charArrToULong(locIdentHeader + 26);
	m_theoraFormatInfo->aspectNumerator = (iBE_Math::charArrToULong(locIdentHeader + 30)) >>8;
	m_theoraFormatInfo->aspectDenominator = (iBE_Math::charArrToULong(locIdentHeader + 33)) >>8;
	m_theoraFormatInfo->colourSpace = locIdentHeader[36];
	m_theoraFormatInfo->targetBitrate = (iBE_Math::charArrToULong(locIdentHeader + 37)) >>8;
	m_theoraFormatInfo->targetQuality = (locIdentHeader[40]) >> 2;

	m_theoraFormatInfo->maxKeyframeInterval= (((locIdentHeader[40]) % 4) << 3) + (locIdentHeader[41] >> 5);

    LOG(logINFO) << "Theora Format: ";
    LOG(logINFO) << "\ttheoraVersion: " << m_theoraFormatInfo->theoraVersion;
    LOG(logINFO) << "\touterFrameWidth: " << m_theoraFormatInfo->outerFrameWidth;
    LOG(logINFO) << "\touterFrameHeight: " << m_theoraFormatInfo->outerFrameHeight;
    LOG(logINFO) << "\tpictureWidth: " << m_theoraFormatInfo->pictureWidth;
    LOG(logINFO) << "\tpictureHeight: " << m_theoraFormatInfo->pictureHeight;
    LOG(logINFO) << "\txOffset: " << m_theoraFormatInfo->xOffset;
    LOG(logINFO) << "\tyOffset: " << m_theoraFormatInfo->yOffset;
    LOG(logINFO) << "\tframeRateNumerator: " << m_theoraFormatInfo->frameRateNumerator;
    LOG(logINFO) << "\tframeRateDenominator: " << m_theoraFormatInfo->frameRateDenominator;
    LOG(logINFO) << "\taspectNumerator: " << m_theoraFormatInfo->aspectNumerator;
    LOG(logINFO) << "\taspectDenominator: " << m_theoraFormatInfo->aspectDenominator;
    LOG(logINFO) << "\tcolourSpace: " << m_theoraFormatInfo->colourSpace;
    LOG(logINFO) << "\ttargetBitrate: " << m_theoraFormatInfo->targetBitrate;
    LOG(logINFO) << "\ttargetQuality: " << m_theoraFormatInfo->targetQuality;
    LOG(logINFO) << "\tmaxKeyframeInterval: " << m_theoraFormatInfo->maxKeyframeInterval;
}

CBasePin* TheoraDecodeFilter::GetPin(int inPinNo)
{
    HRESULT locHR = S_OK;

    // Create an input pin if necessary
    if (m_pInput == NULL) 
    {
        m_pInput = new TheoraDecodeInputPin(this, &locHR);		//Deleted in base destructor    
        m_pOutput = new TheoraDecodeOutputPin(this, &locHR);	//Deleted in base destructor
    }

    // Return the pin
    if (inPinNo == 0) 
    {
        return m_pInput;
    } 
    else if (inPinNo == 1) 
    {
        return m_pOutput;
    } 
    else 
    {
        return NULL;
    }
}
