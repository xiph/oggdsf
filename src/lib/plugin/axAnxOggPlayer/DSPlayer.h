#pragma once


#include <uuids.h>
#include <dshow.h>

#include <d3d9.h>
#include <vmr9.h>
#include <string>

#include "ICMMLCallbacks.h"
#include "ICMMLAppControl.h"
#include "libCMMLTags.h"
#include "IMediaEventNotification.h"
#include "StringHelper.h"



#include <fstream>
#include <string>



using namespace std;

// {6188AD0C-62CB-4658-A14E-CD23CF84EC31}
DEFINE_GUID(X_IID_ICMMLAppControl, 
0x6188ad0c, 0x62cb, 0x4658, 0xa1, 0x4e, 0xcd, 0x23, 0xcf, 0x84, 0xec, 0x31);
class DSPlayer
{
public:
	
	DSPlayer(void);
	~DSPlayer(void);
	//static const GUID X_IID_ICMMLAppControl = {0x6188ad0c, 0x62cb, 0x4658, {0xa1, 0x4e, 0xcd, 0x23, 0xcf, 0x84, 0xec, 0x31};

		bool loadFile(string inFileName);
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


		fstream debugLog;
};


