//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
//
//Copyright (C) 2004 Commonwealth Scientific and Industrial Research
// Orgainisation (CSIRO) Australia
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
#include "dsplay.h"

#pragma managed
namespace illiminable {
namespace libDSPlayDotNET {
	//class __gc DSPlayer {
	//	public:
	//		bool loadFile(String inFileName);
	//		bool play();
	//		bool pause();
	//		bool stop();
	//		bool seek(Int64 inTime);
	//		Int64 queryPosition();
	//};



DSPlay::DSPlay(void) 
	:	mGraphBuilder(NULL)
	,	mMediaControl(NULL)
	,	mMediaSeeking(NULL)
	,	mMediaEvent(NULL)
	,	mEventHandle(INVALID_HANDLE_VALUE)
	//,	mDNCMMLCallbacks(NULL)
	,	mDNMediaEvent(NULL)
	,	mCMMLAppControl(NULL)
	,	mWindowHandle(NULL)
	,	mVideoWindow(NULL)
	,	mLeft(0)
	,	mTop(0)
	,	mWidth(0)
	,	mHeight(0)

{
	CoInitialize(NULL);
	mCMMLProxy = new CMMLCallbackProxy;			//Need to delete this !
	debugLog = new fstream;
	debugLog->open("G:\\logs\\dsplay.log", ios_base::out);
}

DSPlay::DSPlay(IntPtr inWindowHandle, Int32 inLeft, Int32 inTop, Int32 inWidth, Int32 inHeight) 
	:	mGraphBuilder(NULL)
	,	mMediaControl(NULL)
	,	mMediaSeeking(NULL)
	,	mMediaEvent(NULL)
	,	mEventHandle(INVALID_HANDLE_VALUE)
	//,	mDNCMMLCallbacks(NULL)
	,	mDNMediaEvent(NULL)
	,	mCMMLAppControl(NULL)
	,	mWindowHandle(inWindowHandle)
	,	mVideoWindow(NULL)
	,	mLeft(inLeft)
	,	mTop(inTop)
	,	mWidth(inWidth)
	,	mHeight(inHeight)
{
	CoInitialize(NULL);
	mCMMLProxy = new CMMLCallbackProxy;			//Need to delete this !
	debugLog = new fstream;
	debugLog->open("G:\\logs\\dsplay.log", ios_base::out);
}

bool DSPlay::checkEvents() {
	const DWORD TIMEOUT_WAIT = 0;  //Wait this many ms for handle
	long locEventCode = 0;
	long locParam1 = 0;
	long locParam2 = 0;
	HRESULT locHR = S_OK;

	if (WAIT_OBJECT_0 == WaitForSingleObject(mEventHandle, TIMEOUT_WAIT))   { 
			while (locHR = mMediaEvent->GetEvent(&locEventCode, &locParam1, &locParam2, 0), SUCCEEDED(locHR)) 
			{
	            
				//cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
				
				//This is dodgy ! param1 and 2 are actually pointers !!
				if (mDNMediaEvent != NULL) {
					mDNMediaEvent->eventNotification(locEventCode, locParam1, locParam2);
				}

				mMediaEvent->FreeEventParams(locEventCode, locParam1, locParam2);
			}
	}
	return true;
}


DSPlay::~DSPlay(void) {
	*debugLog<<"Killing DSPlay"<<endl;
	debugLog->close();
	delete debugLog;
	releaseInterfaces();
	CoUninitialize();
}

void DSPlay::releaseInterfaces() {
	*debugLog<<"Releasing interfaces"<<endl;
	ULONG numRef = 0;
	if (mMediaControl != NULL) {
		numRef = 
			mMediaControl->Release();
		*debugLog<<"Media Control count = "<<numRef<<endl;
		mMediaControl = NULL;
	}

	if (mMediaSeeking != NULL) {
		numRef = 
			mMediaSeeking->Release();

		*debugLog<<"Media Seeking count = "<<numRef<<endl;
		mMediaSeeking = NULL;
	}

	if (mMediaEvent != NULL) {
		numRef = 
			mMediaEvent->Release();

		*debugLog<<"Media Event count = "<<numRef<<endl;
		mMediaEvent = NULL;
	}

	if (mCMMLAppControl != NULL) {
		numRef = 
			mCMMLAppControl->Release();

		*debugLog<<"CMML App control count = "<<numRef<<endl;
		mCMMLAppControl = NULL;
	}

	*debugLog<<"Before Graph release..."<<endl;
	if (mGraphBuilder != NULL) {
		numRef =
            mGraphBuilder->Release();

		*debugLog<<"Graph Builder count = "<<numRef<<endl;
		mGraphBuilder = NULL;
	}

	if (mVideoWindow != NULL) {
		numRef =
            mVideoWindow->Release();

		*debugLog<<"Video Window count = "<<numRef<<endl;
		mVideoWindow = NULL;
	}


	*debugLog<<"After graph release>.."<<endl;
	//TODO::: Release everything !
}

bool DSPlay::loadFile(String* inFileName) {

	//Debugging only
	ULONG numRef = 0;
	//


	releaseInterfaces();
	HRESULT locHR = S_OK;

	char* locFileName = Wrappers::netStrToCStr(inFileName);
	*debugLog<<"File = "<<locFileName<<endl;
	wstring locWFileName = illiminable::libDSPlayDotNET::toWStr(locFileName);
	
	Wrappers::releaseCStr(locFileName);
	locFileName = NULL;

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
		*debugLog<<"Is annodex"<<endl;
		IBaseFilter* locVMR9 = NULL;

		HRESULT locHR2 = S_OK;
		locHR2 = mGraphBuilder->FindFilterByName(L"Video Mixing Renderer 9", &locVMR9);
		if (locVMR9 == NULL) {
			*debugLog<<"Not in graph... making it !"<<endl;
			locHR2= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR9);
			if (locHR2 == S_OK) {
				locHR2 = mGraphBuilder->AddFilter(locVMR9, L"Video Mixing Renderer 9");
				numRef =
					locVMR9->Release();
				*debugLog<<"VMR9 ref count = "<<numRef<<endl;
				
			}
		} else {
			numRef =
				locVMR9->Release();

			*debugLog<<"VMR9 ref count = "<<numRef<<endl;
		}


		

	}

	*debugLog<<"About to call render on "<<endl;
	//Build the graph
	locHR = mGraphBuilder->RenderFile(locWFileName.c_str(), NULL);

	if (locHR != S_OK) {
		*debugLog<<"Render File FAILED !!"<<endl;
		mIsLoaded = false;
		return false;
	}

	*debugLog<<"Render must have been ok"<<endl;
	if (isFileAnnodex(inFileName)) {
		*debugLog<<"Is annodex"<<endl;
		//Get the app control interface for CMML.
		IBaseFilter* locCMMLFilter = NULL;
		locHR = mGraphBuilder->FindFilterByName(L"CMML Decode Filter", &locCMMLFilter);
		

		if (locCMMLFilter != NULL) {
			ICMMLAppControl* locCMMLAppControl = NULL;
			
			locHR = locCMMLFilter->QueryInterface(X_IID_ICMMLAppControl, (void**)&locCMMLAppControl);
			if (locCMMLAppControl != NULL) {
				mCMMLAppControl = locCMMLAppControl;
				mCMMLAppControl->setCallbacks(mCMMLProxy);
			}
			numRef = 
                locCMMLFilter->Release();

			*debugLog<<"CMML Filter ref Count = "<<numRef<<endl;
		}

	}
	*debugLog<<"After CMML Code..."<<endl;

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

	if (mWindowHandle != NULL) {
		//Get the IVideoWindow interface.
		IVideoWindow* locVideoWindow = NULL;
		locHR = mGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&locVideoWindow);

		if (locHR == S_OK) {
			mVideoWindow = locVideoWindow;
			mVideoWindow->put_Owner((int)mWindowHandle);
			mVideoWindow->SetWindowPosition(mLeft, mTop, mWidth, mHeight);
			mVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
		}
	}


//	if (FAILED(hr))

	return true;


}

bool DSPlay::setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks) {
	return mCMMLProxy->setManagedDelegate(inCMMLCallbacks);
}

bool DSPlay::isLoaded() {
	return mIsLoaded;
}
bool DSPlay::play() {
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

bool DSPlay::pause() {
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

bool DSPlay::stop() {
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Stop();
		if (locHR != S_OK) {
			return false;
		} else {
			seek(0);
			return true;
		}
	} else {
		return false;
	}
}

Int64 DSPlay::seek(Int64 inTime) {
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

Int64 DSPlay::seekStart() {
	return 0;
}

Int64 DSPlay::queryPosition() {
	return 0;
}

Int64 DSPlay::fileSize() {
	return -1;
}
Int64 DSPlay::fileDuration() {
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

bool DSPlay::isFileAnnodex(String* inFilename)
{
	String* locExt = (inFilename->Substring(inFilename->Length - 4, 4))->ToUpper();
	if (locExt->Equals(".ANX")) {
		return true;
	} else {
		return false;
	}
}

bool DSPlay::setMediaEventCallback(IDNMediaEvent* inMediaEventCallback) {
	mDNMediaEvent = inMediaEventCallback;
	return true;
}
IDNMediaEvent* DSPlay::getMediaEventCallback() {
	return mDNMediaEvent;
}


} //end namespace libDSPlayDotNET
} //end namespace illiminable
