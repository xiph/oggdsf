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

#include "stdafx.h"
#include "theoraencodeoutputpin.h"
#include <atlcomcli.h>
#include "IOggDecoder.h"
#include "Common/OggTypes.h" 

TheoraEncodeOutputPin::TheoraEncodeOutputPin(TheoraEncodeFilter* inParentFilter,CCritSec* inFilterLock, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformOutputPin(inParentFilter, inFilterLock,NAME("TheoraEncodeOutputPin"), L"Theora Out", 1024*1024, 3, inAcceptableMediaTypes)
{
}

TheoraEncodeOutputPin::~TheoraEncodeOutputPin(void)
{
}

//bool TheoraEncodeOutputPin::FillFormatBuffer(BYTE* inFormatBuffer) {
//	TheoraEncodeFilter* locParentFilter = (TheoraEncodeFilter*)mParentFilter;
//	memcpy((void*)inFormatBuffer, (const void*) &(locParentFilter->mTheoraFormatBlock), sizeof(THEORAFORMAT));
//	return true;
//}
//unsigned long TheoraEncodeOutputPin::FormatBufferSize() {
//	return sizeof(THEORAFORMAT);
//}

HRESULT TheoraEncodeOutputPin::CreateAndFillFormatBuffer(CMediaType* outMediaType, int inPosition)
{
	if (inPosition == 0) {
		THEORAFORMAT* locTheoraFormat = (THEORAFORMAT*)outMediaType->AllocFormatBuffer(sizeof(THEORAFORMAT));
		//TODO::: Check for null ?

		memcpy((void*)locTheoraFormat, (const void*) &(((TheoraEncodeFilter*)mParentFilter)->mTheoraFormatBlock), sizeof(THEORAFORMAT));
		return S_OK;
	} else {
        return S_FALSE;
	}
}

HRESULT TheoraEncodeOutputPin::CompleteConnect(IPin *inReceivePin)
{
	CComPtr<IOggDecoder> decoder;
    inReceivePin->QueryInterface(IID_IOggDecoder, (void**)&decoder);
	if (decoder != NULL) 
    {
		StampedOggPacket** locHeaders;
		locHeaders = ((TheoraEncodeInputPin*)(GetFilter()->GetPin(0)))->GetCodecHeaders();

		for (int i = 0; i < 3; i++) {
			decoder->showHeaderPacket(locHeaders[i]);
			delete locHeaders[i];
		}
	}

	return AbstractTransformOutputPin::CompleteConnect(inReceivePin);
}

TheoraEncodeFilter* TheoraEncodeOutputPin::GetFilter()
{
    return static_cast<TheoraEncodeFilter*>(mParentFilter);
}
