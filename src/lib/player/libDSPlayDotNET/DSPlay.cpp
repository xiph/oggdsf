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
		return true;
	}

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

bool DSPlay::seek(Int64 inTime) {
	return true;
}

Int64 DSPlay::queryPosition() {
	return 0;
}

} //end namespace libDSPlayDotNET
} //end namespace illiminable