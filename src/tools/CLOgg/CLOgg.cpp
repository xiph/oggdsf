//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
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

//
// CLOgg.cpp : Command line minimalist audio player.
//

#include "stdafx.h"
#include <dshow.h>
#include <windows.h>
#include <iostream>
using namespace std;

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	//
	IGraphBuilder* locGraphBuilder = NULL;
	IMediaControl* locMediaControl = NULL;
	HRESULT locHR = S_FALSE;;
	CoInitialize(NULL);
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);
	locHR = locGraphBuilder->RenderFile(L"g:\\a.ogg", NULL);

	locHR = locGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);


	locHR = locMediaControl->Run();

	IMediaEvent* locMediaEvent = NULL;
	locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);
	
	HANDLE  hEvent; 
	long    evCode, param1, param2;
	BOOLEAN bDone = FALSE;
	HRESULT hr = S_OK;
	hr = locMediaEvent->GetEventHandle((OAEVENT*)&hEvent);
	if (FAILED(hr))
	{
	    /* Insert failure-handling code here. */
	}
	while(true) //!bDone) 
	{
	    if (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, 100))
	    { 
			while (hr = locMediaEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr)) 
			{
	            //printf("Event code: %#04x\n Params: %d, %d\n", evCode, param1, param2);
				cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
				locMediaEvent->FreeEventParams(evCode, param1, param2);
				bDone = (EC_COMPLETE == evCode);
			}
		}
	} 

	cout<<"Finished..."<<endl;
	int x;
	cin>>x;
	locMediaControl->Release();
	locGraphBuilder->Release();
	CoUninitialize();

	return 0;
}

