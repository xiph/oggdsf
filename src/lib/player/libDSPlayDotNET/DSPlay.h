#pragma once
#pragma unmanaged
#include <dshow.h>

#include <string>
#include "ICMMLCallbacks.h"
#include "libCMMLTags.h"

#pragma managed
#using "libCMMLTagsDotNET.dll"
using namespace CSIRO::libCMMLTagsDotNET;
#include "libDSPlayDotNet.h"
#include "IDNCMMLCallbacks.h"
#include "IDNMediaEvent.h"
//#include "Wrappers.h"
using namespace illiminable::libiWrapper;
using namespace std;

namespace illiminable {
namespace libDSPlayDotNET
{
	public __gc class DSPlay 
	{
	public:
		DSPlay(void);
		~DSPlay(void);

		bool loadFile(String* inFileName);
		bool play();
		bool pause();
		bool stop();
		Int64 seek(Int64 inTime);
		Int64 queryPosition();

		bool isLoaded();
		Int64 fileSize();
		Int64 fileDuration();
	

		bool setMediaEventCallback(IDNMediaEvent* inMediaEventCallback);
		IDNMediaEvent* getMediaEventCallback();
		bool checkEvents();

		void releaseInterfaces();
	protected:
		//static wstring toWStr(std::string inString);
		IGraphBuilder* mGraphBuilder;
		IMediaControl* mMediaControl;
		IMediaSeeking* mMediaSeeking;
		IMediaEvent* mMediaEvent;

		HANDLE mEventHandle;

		IDNCMMLCallbacks* mDNCMMLCallbacks;
		IDNMediaEvent* mDNMediaEvent;

		bool mIsLoaded;
		bool isFileAnnodex(String* inFilename);
	};
}
}