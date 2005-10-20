//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
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
#include "AnxDemuxSourceFilter.h"

#include <libOOOggSeek/AutoAnxSeekTable.h>


//-------------------
// This template lets the Object factory create us properly and work with COM infrastructure.
CFactoryTemplate g_Templates[] = 
{
    { 
		L"AnxDemuxFilter",						// Name
	    &CLSID_AnxDemuxSourceFilter,            // CLSID
	    AnxDemuxSourceFilter::CreateInstance,	// Method to create an instance of MyComponent
        NULL,									// Initialization function
        NULL									// Set-up information (for filters)
    }

};

// Generic way of determining the number of items in the template
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]); 



CUnknown* WINAPI AnxDemuxSourceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr) 
{
	AnxDemuxSourceFilter *pNewObject = new AnxDemuxSourceFilter();
    if (pNewObject == NULL) {
        *pHr = E_OUTOFMEMORY;
    }
    return pNewObject;
} 
AnxDemuxSourceFilter::AnxDemuxSourceFilter(void)
	:	OggDemuxSourceFilter(CLSID_AnxDemuxSourceFilter)
{
	mStreamMapper = new AnxStreamMapper(this);
	//anxDebug.open("G:\\logs\\anxdemux.log", ios_base::out);
}

AnxDemuxSourceFilter::~AnxDemuxSourceFilter(void)
{
	//anxDebug<<"Deconstructing anx filter"<<endl;
}


//ANX::: Seek table will need modifying to handle this.
STDMETHODIMP AnxDemuxSourceFilter::Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) {
	//Initialise the file here and setup all the streams
	CAutoLock locLock(m_pLock);
	

	mFileName = inFileName;

	//WARNING::: This nasty hack is to account for the fact that directshow doesn't know how to
	// properly parse url's given to it... because it does a dumb string match for extension at the end of the file,
	// urls with fragments or queries, won't match the extension... so this hack is for the ff plug-in so
	// that it appends another .anx after the fragment/query, which is stripped off here, before sending to the
	// server.

	//NOTE::: This magic string also appears in the hacked version of dsplayer in the IE plugin.
	//The number 18 is the length of the magic string
	if (mFileName.find(L"XsZZfQ__WiiPFD.anx") == mFileName.size() - 18){
		mFileName = mFileName.substr(0, mFileName.size() - 18);
		
	}
	


	//ANX::: Needs to override ??? Or just modify the seeker.
	mSeekTable = new AutoAnxSeekTable(StringHelper::toNarrowStr(mFileName));
	
	//anxDebug<<"After seek table instantiation"<<endl;
	mSeekTable->buildTable();
	//anxDebug<<"After build table..."<<endl;
	//debugLog<<"After build table"<<endl;
	return SetUpPins();
}