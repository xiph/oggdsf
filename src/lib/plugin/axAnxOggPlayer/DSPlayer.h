#pragma once


#include <uuids.h>
#include <dshow.h>

//
#include <d3d9.h>
#include <vmr9.h>


#include <string>

#include "ICMMLCallbacks.h"
#include "ICMMLAppControl.h"
#include "libCMMLTags.h"
#include "IMediaEventNotification.h"





#include <fstream>
#include <string>



using namespace std;


class DSPlayer
{
public:

	DSPlayer(void);
	~DSPlayer(void);
	

		bool loadFile(string inFileName);
		bool loadFile(string inFileName, HWND inWindow, int inLeft, int inTop, int inWidth, int inHeight);
		bool play();
		bool pause();
		bool stop();
		__int64 seek(__int64 inTime);
		__int64 seekStart();
		__int64 queryPosition();

		bool isLoaded();
		__int64 fileSize();
		__int64 fileDuration();
	

		bool setMediaEventCallback(IMediaEventNotification* inMediaEventCallback);
		IMediaEventNotification* getMediaEventCallback();

		//bool setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks);
		bool checkEvents();

		void releaseInterfaces();
	protected:
		//static wstring toWStr(std::string inString);
		IGraphBuilder* mGraphBuilder;
		IMediaControl* mMediaControl;
		IMediaSeeking* mMediaSeeking;
		IMediaEvent* mMediaEvent;
		ICMMLAppControl* mCMMLAppControl;
		ICMMLCallbacks* mCMMLCallback;
		IMediaEventNotification* mMediaEventNotify;

		HANDLE mEventHandle;

		bool mIsLoaded;
		bool isFileAnnodex(string inFilename);

		wstring toWStr(string inString);

		GUID Y_IID_ICMMLAppControl;
		fstream debugLog;
};


