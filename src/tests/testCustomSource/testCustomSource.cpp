// testCustomSource.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <dshow.h>
#include <windows.h>
#include <iostream>

#include "ICustomSource.h"

#include "CustomSourceClass.h"

// {23EF732D-DAAF-41ee-85FB-BB97B1D01FF3}
//DEFINE_GUID(IID_ICustomSource, 
//0x23ef732d, 0xdaaf, 0x41ee, 0x85, 0xfb, 0xbb, 0x97, 0xb1, 0xd0, 0x1f, 0xf3);

static const GUID IID_ICustomSource = 
{ 0x23ef732d, 0xdaaf, 0x41ee, { 0x85, 0xfb, 0xbb, 0x97, 0xb1, 0xd0, 0x1f, 0xf3} };

// {C9361F5A-3282-4944-9899-6D99CDC5370B}
//DEFINE_GUID(CLSID_OggDemuxPacketSourceFilter, 
//0xc9361f5a, 0x3282, 0x4944, 0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb);

static const GUID CLSID_OggDemuxPacketSourceFilter = 
{ 0xc9361f5a, 0x3282, 0x4944, { 0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb } };

using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{

	int y;
	cin>>y;
	//
	IGraphBuilder* locGraphBuilder = NULL;
	IMediaControl* locMediaControl = NULL;
	IBaseFilter* locDemuxer = NULL;
	ICustomSource* locCustomSourceSetter = NULL;
	HRESULT locHR = S_FALSE;;
	CoInitialize(NULL);
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);

	locHR = CoCreateInstance(CLSID_OggDemuxPacketSourceFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locDemuxer);

	locGraphBuilder->AddFilter(locDemuxer, L"Custom Ogg Source");
	locDemuxer->QueryInterface(IID_ICustomSource, (void**)&locCustomSourceSetter);


	CustomSourceClass* locCustomFileSourceInterface = new CustomSourceClass;
	locCustomFileSourceInterface->open("D:\\testfile.ogg");
	
	locCustomSourceSetter->setCustomSourceAndLoad(locCustomFileSourceInterface);

	//Do not release, it's not really a COM interface
	//locCustomSourceSetter->Release();

	IEnumPins* locPinEnum = NULL;

	locDemuxer->EnumPins(&locPinEnum);

	

	IPin* locPin = NULL;
	ULONG locHowMany = 0;
	while (locPinEnum->Next(1, &locPin, &locHowMany) == S_OK) {
		locHR = locGraphBuilder->Render(locPin);
		locPin->Release();
		locPin = NULL;
	}






	//locHR = locGraphBuilder->RenderFile(L"g:\\a.ogg", NULL);

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
	while(!bDone) 
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

