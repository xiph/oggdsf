#pragma once
#pragma unmanaged
#include <dshow.h>

#include <string>

#pragma managed
#include "libDSPlayDotNet.h"
//#include "Wrappers.h"
using namespace illiminable::libiWrapper;
using namespace std;

namespace illiminable {
namespace libDSPlayDotNET
{
	public __gc class DSPlay {
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
	

		void releaseInterfaces();
	protected:
		//static wstring toWStr(std::string inString);
		IGraphBuilder* mGraphBuilder;
		IMediaControl* mMediaControl;
		IMediaSeeking* mMediaSeeking;
		bool mIsLoaded;
	};
}
}