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

#include "StdAfx.h"
#include ".\dsplayer.h"




DSPlayer::DSPlayer(void) 
	:	mGraphBuilder(NULL)
	,	mMediaControl(NULL)
	,	mMediaSeeking(NULL)
	,	mMediaEvent(NULL)
	,	mEventHandle(INVALID_HANDLE_VALUE)
	,	mCMMLCallback(NULL)
	,	mMediaEventNotify(NULL)
	//,	mDNCMMLCallbacks(NULL)
	//,	mDNMediaEvent(NULL)
	,	mCMMLAppControl(NULL)
{
	CoInitialize(NULL);
	//mCMMLProxy = new CMMLCallbackProxy;			//Need to delete this !
	
	debugLog.open("G:\\logs\\DSPlayer.log", ios_base::out);

	//Something about the way the activeX control links won't let us do the normal
	// DEFINE_GUID macro... so this is manually assigning it to a memeber variable so it links.
	Y_IID_ICMMLAppControl.Data1 = 0x6188ad0c;
	Y_IID_ICMMLAppControl.Data2 = 0x62cb;
	Y_IID_ICMMLAppControl.Data3 = 0x4658;
	Y_IID_ICMMLAppControl.Data4[0] = 0xa1;
	Y_IID_ICMMLAppControl.Data4[1] = 0x4e;
	Y_IID_ICMMLAppControl.Data4[2] = 0xcd;
	Y_IID_ICMMLAppControl.Data4[3] = 0x23;
	Y_IID_ICMMLAppControl.Data4[4] = 0xcf;
	Y_IID_ICMMLAppControl.Data4[5] = 0x84;
	Y_IID_ICMMLAppControl.Data4[6] = 0xec;
	Y_IID_ICMMLAppControl.Data4[7] = 0x31;

}
//		// {6188AD0C-62CB-4658-A14E-CD23CF84EC31}
//DEFINE_GUID(Y_IID_ICMMLAppControl, 
//0x6188ad0c, 0x62cb, 0x4658, 0xa1, 0x4e, 0xcd, 0x23, 0xcf, 0x84, 0xec, 0x31);
//}

bool DSPlayer::checkEvents() {
	const DWORD TIMEOUT_WAIT = 0;  //Wait this many ms for handle
	long locEventCode = 0;
	long locParam1 = 0;
	long locParam2 = 0;
	HRESULT locHR = S_OK;

	if (WAIT_OBJECT_0 == WaitForSingleObject(mEventHandle, TIMEOUT_WAIT))   { 
			while (locHR = mMediaEvent->GetEvent(&locEventCode, &locParam1, &locParam2, 0), SUCCEEDED(locHR)) 
			{
	            
				debugLog<<"Event : "<<locEventCode<<" Params : "<<locParam1<<", "<<locParam2<<endl;
				
				
				if (mMediaEventNotify != NULL) {
					mMediaEventNotify->eventNotification(locEventCode, locParam1, locParam2);
				}

				mMediaEvent->FreeEventParams(locEventCode, locParam1, locParam2);
			}
	}
	return true;
}


DSPlayer::~DSPlayer(void) {
	debugLog<<"Killing DSPlayer"<<endl;
	debugLog.close();
	
	releaseInterfaces();
	CoUninitialize();
}

void DSPlayer::releaseInterfaces() {
	debugLog<<"Releasing interfaces"<<endl;
	ULONG numRef = 0;
	if (mMediaControl != NULL) {
		numRef = 
			mMediaControl->Release();
		debugLog<<"Media Control count = "<<numRef<<endl;
		mMediaControl = NULL;
	}

	if (mMediaSeeking != NULL) {
		numRef = 
			mMediaSeeking->Release();

		debugLog<<"Media Seeking count = "<<numRef<<endl;
		mMediaSeeking = NULL;
	}

	if (mMediaEvent != NULL) {
		numRef = 
			mMediaEvent->Release();

		debugLog<<"Media Event count = "<<numRef<<endl;
		mMediaEvent = NULL;
	}

	if (mCMMLAppControl != NULL) {
		numRef = 
			mCMMLAppControl->Release();

		debugLog<<"CMML App control count = "<<numRef<<endl;
		mCMMLAppControl = NULL;
	}

	debugLog<<"Before Graph release..."<<endl;
	if (mGraphBuilder != NULL) {
		numRef =
            mGraphBuilder->Release();

		debugLog<<"Graph Builder count = "<<numRef<<endl;
		mGraphBuilder = NULL;
	}


	debugLog<<"After graph release>.."<<endl;
	//TODO::: Release everything !
}
wstring DSPlayer::toWStr(string inString) {
	wstring retVal;

	//LPCWSTR retPtr = new wchar_t[retVal.length() + 1];
	for (std::string::const_iterator i = inString.begin(); i != inString.end(); i++) {
		retVal.append(1, *i);
	}
	

	return retVal;
}

bool DSPlayer::loadFile(string inFileName, HWND inWindow, int inLeft, int inTop, int inWidth, int inHeight) {
	//Debugging only
	ULONG numRef = 0;
	//


	releaseInterfaces();
	HRESULT locHR = S_OK;

	
	debugLog<<"File = "<<inFileName<<endl;
	wstring locWFileName = toWStr(inFileName);
	
	
	

	//Have to use a local pointer or taking the adress of a member function makes the second level
	// of indirection a __gc pointer.
	IGraphBuilder* locGraphBuilder = NULL;
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);
	mGraphBuilder = locGraphBuilder;
	
	if (locHR != S_OK) {
		mIsLoaded = false;
		debugLog<<"Faild to make graph"<<endl;
		return false;
	}
	
	
	//If it's an annodex file, then put the VMR 9 in the graph.
	if (isFileAnnodex(inFileName)) {
		debugLog<<"Is annodex"<<endl;
		IBaseFilter* locVMR9 = NULL;

		HRESULT locHR2 = S_OK;
		locHR2 = mGraphBuilder->FindFilterByName(L"Video Mixing Renderer 9", &locVMR9);
		if (locVMR9 == NULL) {
			debugLog<<"Not in graph... making it !"<<endl;
			locHR2= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR9);
			if (locHR2 == S_OK) {
				locHR2 = mGraphBuilder->AddFilter(locVMR9, L"Video Mixing Renderer 9");
				numRef =
					locVMR9->Release();
				debugLog<<"VMR9 ref count = "<<numRef<<endl;
				
			}
		} else {
			numRef =
				locVMR9->Release();

			debugLog<<"VMR9 ref count = "<<numRef<<endl;
		}


		

	}

	debugLog<<"About to call render on "<<endl;
	//Build the graph
	locHR = mGraphBuilder->RenderFile(locWFileName.c_str(), NULL);
	debugLog<<"After render call..."<<endl;
	if (locHR != S_OK) {
		debugLog<<"Render File FAILED !!"<<endl;
		mIsLoaded = false;
		return false;
	}


	//CHANGES HERE FOR EMBEDDED WINDOW
	debugLog<<"Looking for IVideo Window"<<endl;
	IVideoWindow* locVW = NULL;
	locHR = locGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&locVW);

	if (locHR == S_OK) {
		debugLog<<"We got the IVideoWindow"<<endl;

		locVW->put_MessageDrain((OAHWND)inWindow);


		locVW->put_Owner((OAHWND)inWindow);
		debugLog<<"Setting stuff..."<<endl;
		locVW->SetWindowPosition(inLeft, inTop, inWidth, inHeight);
		debugLog<<"Releasing windows"<<endl;
		locVW->Release();

		debugLog<<"Post release"<<endl;
	}

	//

	debugLog<<"Render must have been ok"<<endl;
	if (isFileAnnodex(inFileName)) {
		debugLog<<"Is annodex"<<endl;
		//Get the app control interface for CMML.
		IBaseFilter* locCMMLFilter = NULL;
		locHR = mGraphBuilder->FindFilterByName(L"CMML Decode Filter", &locCMMLFilter);
		

		if (locCMMLFilter != NULL) {
			ICMMLAppControl* locCMMLAppControl = NULL;
			
			locHR = locCMMLFilter->QueryInterface(Y_IID_ICMMLAppControl, (void**)&locCMMLAppControl);
			if (locCMMLAppControl != NULL) {
				mCMMLAppControl = locCMMLAppControl;
				
				mCMMLAppControl->setCallbacks(mCMMLCallback);
			}
			numRef = 
                locCMMLFilter->Release();

			debugLog<<"CMML Filter ref Count = "<<numRef<<endl;
		}

	}
	debugLog<<"After CMML Code..."<<endl;

	//Get the media control interface
	IMediaControl* locMediaControl = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);
	mMediaControl = locMediaControl;

	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	} else {
		mIsLoaded = true;
	}

	//get the media seeking interface if its available.
	IMediaSeeking* locMediaSeeking = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&locMediaSeeking);
	mMediaSeeking = locMediaSeeking;

	//Get the media event interface
	IMediaEvent* locMediaEvent = NULL;
	locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);

	if (locHR == S_OK) {
		mMediaEvent = locMediaEvent;
		HANDLE locEventHandle = INVALID_HANDLE_VALUE;
		locHR = locMediaEvent->GetEventHandle((OAEVENT*)&locEventHandle);
		mEventHandle = locEventHandle;
	}

//	if (FAILED(hr))

	return true;
}
bool DSPlayer::loadFile(string inFileName) {

	//Debugging only
	ULONG numRef = 0;
	//


	releaseInterfaces();
	HRESULT locHR = S_OK;

	
	debugLog<<"File = "<<inFileName<<endl;
	wstring locWFileName = toWStr(inFileName);
	
	
	

	//Have to use a local pointer or taking the adress of a member function makes the second level
	// of indirection a __gc pointer.
	IGraphBuilder* locGraphBuilder = NULL;
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);
	mGraphBuilder = locGraphBuilder;
	
	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	}
	
	
	//If it's an annodex file, then put the VMR 9 in the graph.
	if (isFileAnnodex(inFileName)) {
		debugLog<<"Is annodex"<<endl;
		IBaseFilter* locVMR9 = NULL;

		HRESULT locHR2 = S_OK;
		locHR2 = mGraphBuilder->FindFilterByName(L"Video Mixing Renderer 9", &locVMR9);
		if (locVMR9 == NULL) {
			debugLog<<"Not in graph... making it !"<<endl;
			locHR2= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR9);
			if (locHR2 == S_OK) {
				locHR2 = mGraphBuilder->AddFilter(locVMR9, L"Video Mixing Renderer 9");
				numRef =
					locVMR9->Release();
				debugLog<<"VMR9 ref count = "<<numRef<<endl;
				
			}
		} else {
			numRef =
				locVMR9->Release();

			debugLog<<"VMR9 ref count = "<<numRef<<endl;
		}


		

	}

	debugLog<<"About to call render on "<<endl;
	//Build the graph
	locHR = mGraphBuilder->RenderFile(locWFileName.c_str(), NULL);

	if (locHR != S_OK) {
		debugLog<<"Render File FAILED !!"<<endl;
		mIsLoaded = false;
		return false;
	}

	debugLog<<"Render must have been ok"<<endl;
	if (isFileAnnodex(inFileName)) {
		debugLog<<"Is annodex"<<endl;
		//Get the app control interface for CMML.
		IBaseFilter* locCMMLFilter = NULL;
		locHR = mGraphBuilder->FindFilterByName(L"CMML Decode Filter", &locCMMLFilter);
		

		if (locCMMLFilter != NULL) {
			ICMMLAppControl* locCMMLAppControl = NULL;
			
			locHR = locCMMLFilter->QueryInterface(Y_IID_ICMMLAppControl, (void**)&locCMMLAppControl);
			if (locCMMLAppControl != NULL) {
				mCMMLAppControl = locCMMLAppControl;
				
				mCMMLAppControl->setCallbacks(mCMMLCallback);
			}
			numRef = 
                locCMMLFilter->Release();

			debugLog<<"CMML Filter ref Count = "<<numRef<<endl;
		}

	}
	debugLog<<"After CMML Code..."<<endl;

	//Get the media control interface
	IMediaControl* locMediaControl = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);
	mMediaControl = locMediaControl;

	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	} else {
		mIsLoaded = true;
	}

	//get the media seeking interface if its available.
	IMediaSeeking* locMediaSeeking = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&locMediaSeeking);
	mMediaSeeking = locMediaSeeking;

	//Get the media event interface
	IMediaEvent* locMediaEvent = NULL;
	locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);

	if (locHR == S_OK) {
		mMediaEvent = locMediaEvent;
		HANDLE locEventHandle = INVALID_HANDLE_VALUE;
		locHR = locMediaEvent->GetEventHandle((OAEVENT*)&locEventHandle);
		mEventHandle = locEventHandle;
	}

//	if (FAILED(hr))

	return true;


}

//bool DSPlayer::setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks) {
//	return mCMMLProxy->setManagedDelegate(inCMMLCallbacks);
//}

bool DSPlayer::isLoaded() {
	return mIsLoaded;
}
bool DSPlayer::play() {
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Run();
		if (SUCCEEDED(locHR)) {
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

bool DSPlayer::pause() {
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Pause();
		if (locHR != S_OK) {
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

bool DSPlayer::stop() {
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Stop();
		if (locHR != S_OK) {
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

__int64 DSPlayer::seek(__int64 inTime) {
	if (mIsLoaded && (mMediaSeeking != NULL)) {
		LONGLONG locCurrent = inTime;
		LONGLONG locStop = 0;
		HRESULT locHR = mMediaSeeking->SetPositions(&locCurrent, AM_SEEKING_AbsolutePositioning | AM_SEEKING_ReturnTime, 
													&locStop, AM_SEEKING_NoPositioning);
		
		if (SUCCEEDED(locHR)) {
			return locCurrent;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
	
}

__int64 DSPlayer::seekStart() {
	return 0;
}

__int64 DSPlayer::queryPosition() {
	return 0;
}

__int64 DSPlayer::fileSize() {
	return -1;
}
__int64 DSPlayer::fileDuration() {
	if (mIsLoaded && (mMediaSeeking != NULL)) {
		LONGLONG locDuration = 0;
		HRESULT locHR = mMediaSeeking->GetDuration(&locDuration);

		if (locHR != S_OK) {
			return -1;
		} else {
			return locDuration;
		}
	} else {
		return -1;
	}
}

bool DSPlayer::isFileAnnodex(string inFilename)
{
	//BUG::: Case sensitive
	string locExt = inFilename.substr(inFilename.size() - 4, 4);
	
	if (locExt == ".anx") {
		return true;
	} else {
		return false;
	}
}

bool DSPlayer::setMediaEventCallback(IMediaEventNotification* inMediaEventCallback) {
	mMediaEventNotify = inMediaEventCallback;
	return true;
}
IMediaEventNotification* DSPlayer::getMediaEventCallback() {
	return mMediaEventNotify;
}



