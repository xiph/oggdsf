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
#include "VorbisEncodeOutputPin.h"

VorbisEncodeOutputPin::VorbisEncodeOutputPin(VorbisEncodeFilter* inParentFilter,CCritSec* inFilterLock, vector<CMediaType*> inAcceptableMediaTypes)
	:	AbstractTransformOutputPin(inParentFilter, inFilterLock,NAME("VorbisDecodeOutputPin"), L"Vorbis Out", 65536, 5, inAcceptableMediaTypes)
{
}

VorbisEncodeOutputPin::~VorbisEncodeOutputPin(void)
{
}

HRESULT VorbisEncodeOutputPin::CreateAndFillFormatBuffer(CMediaType* outMediaType, int inPosition)
{
	if (inPosition == 0) {
		sVorbisFormatBlock* locVorbisFormat = (sVorbisFormatBlock*)outMediaType->AllocFormatBuffer(sizeof(sVorbisFormatBlock));
		//TODO::: Check for null ?

		memcpy((void*)locVorbisFormat, (const void*) &(((VorbisEncodeFilter*)mParentFilter)->mVorbisFormatBlock), sizeof(sVorbisFormatBlock));
		return S_OK;
	} else {
        return S_FALSE;
	}
}


//bool VorbisEncodeOutputPin::FillFormatBuffer(BYTE* inFormatBuffer) {
//	VorbisEncodeFilter* locParentFilter = (VorbisEncodeFilter*)mParentFilter;
//	memcpy((void*)inFormatBuffer, (const void*) &(locParentFilter->mVorbisFormatBlock), sizeof(sVorbisFormatBlock));
//	return true;
//}
//unsigned long VorbisEncodeOutputPin::FormatBufferSize() {
//	return sizeof(sVorbisFormatBlock);
//}




//Old imp
//*********************************************
//#include "StdAfx.h"
//#include "vorbisencodeoutputpin.h"
//
//VorbisEncodeOutputPin::VorbisEncodeOutputPin(VorbisEncodeFilter* inParentFilter,CCritSec* inFilterLock, CMediaType* inOutputMediaType)
//	:	AbstractAudioEncodeOutputPin(inParentFilter, inFilterLock,NAME("VorbisDecodeOutputPin"), L"Vorbis Out", inOutputMediaType)
//{
//}
//
//VorbisEncodeOutputPin::~VorbisEncodeOutputPin(void)
//{
//}
//
//bool VorbisEncodeOutputPin::FillFormatBuffer(BYTE* inFormatBuffer) {
//	VorbisEncodeFilter* locParentFilter = (VorbisEncodeFilter*)mParentFilter;
//	memcpy((void*)inFormatBuffer, (const void*) &(locParentFilter->mVorbisFormatBlock), sizeof(sVorbisFormatBlock));
//	return true;
//}
//unsigned long VorbisEncodeOutputPin::FormatBufferSize() {
//	return sizeof(sVorbisFormatBlock);
//}
