// iTranscode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
using namespace std;



bool transcodeToVorbis(string inFileName) {
	IGraphBuilder* locGraphBuilder = NULL;
	IMediaControl* locMediaControl = NULL;
	HRESULT locHR;
	CoInitialize(NULL);
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);
	locHR = locGraphBuilder->RenderFile(StringHelper::toWStr(inFileName).c_str(), NULL);

	if (locHR != S_OK) {
		//Release memory here !!
		if (locGraphBuilder != NULL) {
			locGraphBuilder->Release();
		}
		CoUninitialize();
		return false;
	}

	locHR = locGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);
	
	IMediaEvent* locMediaEvent = NULL;
	locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);

	IEnumFilters* locFilterEnum = NULL;
	locHR = locGraphBuilder->EnumFilters(&locFilterEnum);

	bool locStillMore = true;

	IBaseFilter* locFilter = NULL;
	
	locHR = S_OK;
	ULONG locHowManyFilters = 0;
	ULONG locHowManyPins = 0;
	while (locHR == S_OK) {
		//Loop through all the filter.
		locHR = locFilterEnum->Next(1, &locFilter, &locHowMany);
		if (locHR == S_OK) {
			//When we find one... loop through it's pins.
			HRESULT locPinHR = S_OK;
			IEnumPins* locPinEnum = NULL;
			IPin* locPin = NULL;
			PIN_DIRECTION locPinDirn;
			bool locHasOutputs = false;
			locPinHR = locFilter->EnumPins(&locPinEnum);
			if (locPinHR == S_OK) {
				while (locPinHR == S_OK) {
					//Loop through the pins.
					locPinHR = locPinEnum->Next(1, &locPin, &locHowManyPins);
					if (locPinHR == S_OK) {
						locPin->QueryDirection(&locPinDirn);
						if (locPinDirn == PINDIR_OUTPUT) {
							//Has an output pin... can't be a renderer, break out of the pin loop.
							locHasOutputs = true;
							break;
						}

					}
				}

				if (!locHasOutputs) {
					//Renderer filter here. Remove it from the graph.
					locHR = locGraphBuilder->RemoveFilter(locFilter);
					//Break out of the filter loop.
					break;
				}

			}

		}
	}


	IBaseFilter* locVorbisEncoder = NULL;
	locHR = CoCreateInstance(CLSID_VorbisEncodeFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&locVorbisEncoder);

	if (locHR == S_OK) {
		//Created a vorbis filter...
		locGraphBuilder->AddFilter(locVorbisEncoder, L"Vorbis Encode Filter");
		
		////UPTO HERE::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	}


	locHR = locMediaControl->Run();

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
				//cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
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

	return true;
}
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3) {
		cout<<"Usage       : iTranscode <source path/file mask> <target directory>"<<endl;
		cout<<"              target directory must end with a backslash"<<endl;
		cout<<"Description : This program will search the directory for any files you specify and"<<endl;
		cout<<"              attempt to transcode them to ogg vorbis"<<endl;
	} else {
		unsigned long locNumFound = 0;	


		WIN32_FIND_DATA locFindData;
		HANDLE locFindHandle;

		
		locFindHandle = FindFirstFile(argv[1], &locFindData); //, FindExSearchNameMatch, NULL, 0 );

		if (locFindHandle == INVALID_HANDLE_VALUE) {
			cout<<"Invalid file mask, or no matches."<<endl;
			
		} else {
			
			if ((locFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				locNumFound++;
				cout<<"Found "<<locNumFound << " : "<< locFindData.cFileName<<endl;
				string locFileName = locFindData.cFileName;
				bool locTransOK = transcodeToVorbis(locFileName);
			}
		
			BOOL locFindOK = TRUE;
			while (locFindOK == TRUE) {
				locFindOK = FindNextFile(locFindHandle, &locFindData);
				if (locFindOK == TRUE) {
					if ((locFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
						locNumFound++;
						cout<<"Found "<<locNumFound << " : "<< locFindData.cFileName<<endl;;
					}
				}
	
			}
			FindClose(locFindHandle);
		}
	}


	return 0;
}

