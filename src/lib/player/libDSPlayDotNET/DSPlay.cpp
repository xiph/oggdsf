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
{
	CoInitialize(NULL);
}

DSPlay::~DSPlay(void) {
	releaseInterfaces();
	CoUninitialize();
}

void DSPlay::releaseInterfaces() {
	if (mGraphBuilder != NULL) {
		mGraphBuilder->Release();
		mGraphBuilder = NULL;
	}

	if (mMediaControl != NULL) {
		mMediaControl->Release();
		mMediaControl = NULL;

	}
}

bool DSPlay::loadFile(String* inFileName) {

	
	releaseInterfaces();
	HRESULT locHR = S_OK;

	char* locFileName = Wrappers::netStrToCStr(inFileName);
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
	
		IBaseFilter* locVMR9 = NULL;

		HRESULT locHR = S_OK;
		locHR = mGraphBuilder->FindFilterByName(L"Video Mixing Renderer 9", &locVMR9);
		if (locVMR9 == NULL) {
			locHR= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR9);
			if (locHR == S_OK) {
				locHR = mGraphBuilder->AddFilter(locVMR9, L"Video Mixing Renderer 9");
				if (locHR != S_OK) {
					locVMR9->Release();
				}
			}
		}

	}
	locHR = mGraphBuilder->RenderFile(locWFileName.c_str(), NULL);

	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	}

	IMediaControl* locMediaControl = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);
	mMediaControl = locMediaControl;

	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	} else {
		mIsLoaded = true;
	}

	IMediaSeeking* locMediaSeeking = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&locMediaSeeking);
	mMediaSeeking = locMediaSeeking;

	return true;


}

bool DSPlay::isLoaded() {
	return mIsLoaded;
}
bool DSPlay::play() {
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Run();
		if (locHR != S_OK) {
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
			return true;
		}
	} else {
		return false;
	}
}

Int64 DSPlay::seek(Int64 inTime) {
	/*if (mIsLoaded && (mMediaSeeking != NULL) {
		HRESULT locHR = mMediaSeeking->SetPositions(
	}*/
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

} //end namespace libDSPlayDotNET
} //end namespace illiminable
