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

#pragma once
#pragma unmanaged
//#include "CMMLDecoderIIDs.h"
#include <uuids.h>
#include <dshow.h>

#include <d3d9.h>
#include <vmr9.h>
#include <string>

#include "ICMMLCallbacks.h"
#include "ICMMLAppControl.h"
#include "libCMMLTags/libCMMLTags.h"
#include "CMMLCallbackProxy.h"


#include <fstream>

#pragma managed
using namespace System::IO;
#using "System.Drawing.dll"

#using "libCMMLTagsDotNET.dll"
using namespace illiminable::libCMMLTagsDotNET;
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
		DSPlay(IntPtr inWindowHandle, Int32 inLeft, Int32 inTop, Int32 inWidth, Int32 inHeight);
		~DSPlay(void);

		/// Load the named file into the player.
		bool loadFile(String* inFileName);

		/// Start playing the media file.
		bool play();

		/// Pause the media file.
		bool pause();

		/// Stop the media file.
		bool stop();

		/// Seek to the specified time in 100 nanoseconds units. ie 10 000 000 per second.
		Int64 seek(Int64 inTime);

		/// Seek to the start of the media file.
		Int64 seekStart();
		Int64 queryPosition();

		/// Repaints the frame. Only needed for windowless rendering.
		void repaint();

		/// Returns if there is a file loaded.
		bool isLoaded();

		/// Returns the file size in bytes of the media file.
		Int64 fileSize();

		/// Returns the duration of the file in 100 nanosecond units. ie 10 000 000 per second.
		Int64 fileDuration();

		/// Returns a .NET Bitmap class of the current video frame. Requires Renderless mode.
		System::Drawing::Bitmap* GetImage();
	
		/// Sets the callback for media events. Notably EC_COMPLETE.
		bool setMediaEventCallback(IDNMediaEvent* inMediaEventCallback);

		/// Returns a pointer to the current media event callback.
		IDNMediaEvent* getMediaEventCallback();

		/// Sets the callback for CMMLTags. Only valid for annodex files.
		bool setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks);

		/// Triggers an event check, which if needed will fire the callbacks.
		bool checkEvents();

		/// Releases all the interfaces in use and unloads the file.
		void releaseInterfaces();

	protected:
		//static wstring toWStr(std::string inString);
		IGraphBuilder* mGraphBuilder;
		IMediaControl* mMediaControl;
		IMediaSeeking* mMediaSeeking;
		IMediaEvent* mMediaEvent;
		ICMMLAppControl* mCMMLAppControl;
		IVideoWindow* mVideoWindow;

		IVMRWindowlessControl* mVMR7Window;
		IVMRWindowlessControl9* mVMR9Window;

		Int32 mLeft;
		Int32 mTop;
		Int32 mWidth;
		Int32 mHeight;

		HANDLE mEventHandle;
		IntPtr mWindowHandle;

		//IDNCMMLCallbacks* mDNCMMLCallbacks;
		CMMLCallbackProxy* mCMMLProxy;
		IDNMediaEvent* mDNMediaEvent;

		bool mIsLoaded;
		__int64 mFileSize;
		bool isFileAnnodex(String* inFilename);

		__value enum eVideoRenderer {
			VR_VIDEO_WINDOW,
			VR_VMR7,
			VR_VMR9,
			VR_NONE = 100

		};
		eVideoRenderer mVideoRenderType;

		fstream* debugLog;
		

	};
}
}