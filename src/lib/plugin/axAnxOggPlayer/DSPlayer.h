#pragma once


#include <uuids.h>
#include <dshow.h>

#include <d3d9.h>
#include <vmr9.h>
#include <string>

#include "ICMMLCallbacks.h"
#include "ICMMLAppControl.h"
#include "libCMMLTags.h"



#include <fstream>



using namespace std;
class DSPlayer
{
public:
	DSPlayer(void);
	~DSPlayer(void);

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
	

		//bool setMediaEventCallback(IDNMediaEvent* inMediaEventCallback);
		//IDNMediaEvent* getMediaEventCallback();

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

		HANDLE mEventHandle;

		bool mIsLoaded;
		bool isFileAnnodex(string inFilename);


		fstream debugLog;
};


