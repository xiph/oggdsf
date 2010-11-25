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
#include "VorbisEncodeOutputPin.h"
#include <atlcomcli.h>
#include "IOggDecoder.h"
#include "Common/OggTypes.h" 

VorbisEncodeOutputPin::VorbisEncodeOutputPin(       VorbisEncodeFilter* inParentFilter
                                                ,   CCritSec* inFilterLock
                                                ,   vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformOutputPin(     inParentFilter
                                    ,   inFilterLock
                                    ,   NAME("VorbisDecodeOutputPin")
                                    ,   L"Vorbis Out"
                                    ,   65536
                                    ,   5
                                    ,   inAcceptableMediaTypes)
{

}

VorbisEncodeOutputPin::~VorbisEncodeOutputPin(void)
{

}

HRESULT VorbisEncodeOutputPin::CreateAndFillFormatBuffer(CMediaType* outMediaType, int inPosition)
{
	if (inPosition == 0) {
		VORBISFORMAT* locVorbisFormat = (VORBISFORMAT*)outMediaType->AllocFormatBuffer(sizeof(VORBISFORMAT));
		//TODO::: Check for null ?

		memcpy((void*)locVorbisFormat, (const void*) &(((VorbisEncodeFilter*)mParentFilter)->mVorbisFormatBlock), sizeof(VORBISFORMAT));
		return S_OK;
	} else {
        return S_FALSE;
	}
}

HRESULT VorbisEncodeOutputPin::CompleteConnect(IPin *inReceivePin)
{
    CComPtr<IOggDecoder> decoder;
    inReceivePin->QueryInterface(IID_IOggDecoder, (void**)&decoder);
    if (decoder != NULL) 
    {
        std::vector<StampedOggPacket*> locHeaders;
        locHeaders = ((VorbisEncodeInputPin*)(GetFilter()->GetPin(0)))->GetCodecHeaders();

        for (int i = 0; i < 3; i++) {
            decoder->showHeaderPacket(locHeaders[i]);
            delete locHeaders[i];
        }
    }

    return AbstractTransformOutputPin::CompleteConnect(inReceivePin);
}

VorbisEncodeFilter* VorbisEncodeOutputPin::GetFilter()
{
    return static_cast<VorbisEncodeFilter*>(mParentFilter);
}
