//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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
#include "oggmuxinputpin.h"

OggMuxInputPin::OggMuxInputPin(OggMuxFilter* inParentFilter, CCritSec* inFilterLock, HRESULT* inHR, OggMuxStream* inMuxStream)
	:	CBaseInputPin(NAME("OggMuxInputPin"), inParentFilter, inFilterLock, inHR, L"Ogg Packet In")
	,	mParentFilter(inParentFilter)
	,	mMuxStream(inMuxStream)
{
	OggPaginatorSettings* locSettings = new OggPaginatorSettings;
	locSettings->mMinPageSize = 4096;
	locSettings->mMaxPageSize = 8192;
	
	LARGE_INTEGER locTicks;
	QueryPerformanceCounter(&locTicks);
	srand((unsigned int)locTicks.LowPart);
	locSettings->mSerialNo = ((unsigned long)(rand() + 1)) * ((unsigned long)(rand() + 1));
	//locSettings->mSerialNo = 13130;
	
	mPaginator.setParameters(locSettings);
	mPaginator.setPageCallback(mMuxStream);

	debugLog.open("C:\\temp\\oggmuxinpin.log", ios_base::out);
}

OggMuxInputPin::~OggMuxInputPin(void)
{
	debugLog.close();
}


HRESULT OggMuxInputPin::GetMediaType(int inPosition, CMediaType* outMediaType) {
	switch(inPosition) {
		case 0:
			outMediaType->majortype = MEDIATYPE_Video;
			outMediaType->subtype = MEDIASUBTYPE_Theora;
			return S_OK;
		case 1:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_Vorbis;
			return S_OK;
		case 2:
			outMediaType->majortype = MEDIATYPE_Audio;
			outMediaType->subtype = MEDIASUBTYPE_Speex;
			return S_OK;

		default:
			return VFW_S_NO_MORE_ITEMS;
	
	}
}
HRESULT OggMuxInputPin::CheckMediaType(const CMediaType* inMediaType) {
	if	(	(inMediaType->majortype == MEDIATYPE_Video 
				&& inMediaType->subtype == MEDIASUBTYPE_Theora 
				&& inMediaType->formattype == FORMAT_Theora) 
			||
			(inMediaType->majortype == MEDIATYPE_Audio 
				&& inMediaType->subtype == MEDIASUBTYPE_Vorbis 
				&& inMediaType->formattype == FORMAT_Vorbis)
			||
			(inMediaType->majortype == MEDIATYPE_Audio 
				&& inMediaType->subtype == MEDIASUBTYPE_Speex 
				&& inMediaType->formattype == FORMAT_Speex)
		) {
		return S_OK;
	} else {
		return E_FAIL;
	}
}

STDMETHODIMP OggMuxInputPin::Receive(IMediaSample* inSample) {
	LONGLONG locStart = 0;
	LONGLONG locEnd = 0;
	BYTE* locSampleBuff;
	inSample->GetPointer(&locSampleBuff);
	HRESULT locHR = inSample->GetTime(&locStart, &locEnd);

	debugLog <<"Received "<<locStart<<" - "<<locEnd<<endl;
	long locBuffSize = inSample->GetActualDataLength();
	unsigned char* locBuff = new unsigned char[locBuffSize];
	memcpy((void*)locBuff, (const void*)locSampleBuff, inSample->GetActualDataLength());
	StampedOggPacket* locPacket = new StampedOggPacket(locBuff, inSample->GetActualDataLength(), true, locStart, locEnd, StampedOggPacket::OGG_END_ONLY);
	
	mPaginator.acceptStampedOggPacket(locPacket);

	return S_OK;
}

HRESULT OggMuxInputPin::CompleteConnect(IPin* inReceivePin) {
	mMuxStream->setIsActive(true);
	return mParentFilter->addAnotherPin();
}
STDMETHODIMP OggMuxInputPin::EndOfStream(void) {
	mPaginator.finishStream();
	mMuxStream->setIsEOS(true);
	//HRESULT locHR = mParentFilter->NotifyEvent(EC_COMPLETE, S_OK, NULL);
	return S_OK;
	
}