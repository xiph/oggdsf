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
#pragma once
#include <streams.h>
#include <pullpin.h>
#include <initguid.h>

#ifdef DSFCMMLRAWSOURCE_EXPORTS
#define DSFCMMLRAWSOURCE_API __declspec(dllexport)
#else
#define DSFCMMLRAWSOURCE_API __declspec(dllimport)
#endif

// {B78032F1-DCC8-4af4-91AD-FB98E806130D}
DEFINE_GUID(CLSID_CMMLRawSourceFilter, 
0xb78032f1, 0xdcc8, 0x4af4, 0x91, 0xad, 0xfb, 0x98, 0xe8, 0x6, 0x13, 0xd);


// {53696C76-6961-40b2-B136-436F6E726164}
DEFINE_GUID(FORMAT_CMML, 
0x53696c76, 0x6961, 0x40b2, 0xb1, 0x36, 0x43, 0x6f, 0x6e, 0x72, 0x61, 0x64);


// {5A656E74-6172-6F26-B79C-D6416E647282}
DEFINE_GUID(MEDIASUBTYPE_CMML, 
0x5a656e74, 0x6172, 0x6f26, 0xb7, 0x9c, 0xd6, 0x41, 0x6e, 0x64, 0x72, 0x82);

//Structure defining the registration details of the filter
const REGFILTER2 CMMLRawSourceFilterReg = {
		1,
		MERIT_NORMAL,
		0,
        NULL
		
};


struct sCMMLFormatBlock {
	__int64 granuleNumerator;
	__int64 granuleDenominator;
};
