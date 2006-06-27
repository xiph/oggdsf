
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
#include "stdafx.h"
#include "dsplayer.h"


static const GUID CLSID_XX_VorbisDecoder =
{ 0x5a1d945, 0xa794, 0x44ef, { 0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55 } };

static const GUID CLSID_XX_OggDemux =  
{ 0xc9361f5a, 0x3282, 0x4944, { 0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb } };

static const GUID  CLSID_XX_TheoraDecoder =
{ 0x5187161, 0x5c36, 0x4324, { 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d } };


DSPlayer::DSPlayer(void) 
	:	mGraphBuilder(NULL)
	,	mMediaControl(NULL)
	,	mMediaSeeking(NULL)
	,	mMediaEvent(NULL)
	,	mBasicAudio(NULL)
	,	mEventHandle(INVALID_HANDLE_VALUE)
	//,	mDNCMMLCallbacks(NULL)
	//,	mDNMediaEvent(NULL)
	//,	mCMMLAppControl(NULL)
	,	mWindowHandle(NULL)
	,	mVideoWindow(NULL)
	,	mVideoRenderFilter(NULL)

#ifndef WINCE
	,	mVMR7Window(NULL)
	,	mVMR9Window(NULL)
	,	mVideoFrameStep(NULL)
#endif
	,	mOggBaseTime(NULL)
	,	mLeft(0)
	,	mTop(0)
	,	mWidth(0)
	,	mHeight(0)
	,	mFileSize(0)
	,	mVideoRenderType(VR_NONE)

	,	mAvgTimePerFrame(0)
	,	mVideoWidth(0)
	,	mVideoHeight(0)
	,	mGotVideoInformation(false)
	


{
	CoInitialize(NULL);
	//mCMMLProxy = new CMMLCallbackProxy;			//Need to delete this !
	debugLog = new fstream;
#ifdef OGGCODECS_LOGGING
#ifndef WINCE
	debugLog->open("C:\\logs\\DSPlayer.log", ios_base::out | ios_base::app);
#else
	debugLog->open("\\Storage Card\\dsplay.txt", ios_base::out | ios_base::app);
#endif
#endif
	*debugLog<<"Starting new log"<<endl;
}

DSPlayer::DSPlayer(HWND inWindowHandle, int inLeft, int inTop, int inWidth, int inHeight) 
	:	mGraphBuilder(NULL)
	,	mMediaControl(NULL)
	,	mMediaSeeking(NULL)
	,	mMediaEvent(NULL)
	,	mBasicAudio(NULL)
	,	mEventHandle(INVALID_HANDLE_VALUE)
	//,	mDNCMMLCallbacks(NULL)
	//,	mDNMediaEvent(NULL)
	//,	mCMMLAppControl(NULL)
	,	mWindowHandle(inWindowHandle)
	,	mVideoWindow(NULL)
	,	mVideoRenderFilter(NULL)
#ifndef WINCE
	,	mVMR7Window(NULL)
	,	mVMR9Window(NULL)
	,	mVideoFrameStep(NULL)
#endif
	,	mOggBaseTime(NULL)
	,	mLeft(inLeft)
	,	mTop(inTop)
	,	mWidth(inWidth)
	,	mHeight(inHeight)
	,	mFileSize(0)
	,	mVideoRenderType(VR_NONE)

	,	mAvgTimePerFrame(0)
	,	mVideoWidth(0)
	,	mVideoHeight(0)
{
	CoInitialize(NULL);
	//mCMMLProxy = new CMMLCallbackProxy;			//Need to delete this !
	debugLog = new fstream;
#ifdef OGGCODECS_LOGGING
	debugLog->open("C:\\logs\\DSPlayer.log", ios_base::out | ios_base::app);
#endif
	*debugLog<<"Starting new log"<<endl;
}

//bool DSPlayer::checkEvents() {
//	const DWORD TIMEOUT_WAIT = 0;  //Wait this many ms for handle
//	long locEventCode = 0;
//	long locParam1 = 0;
//	long locParam2 = 0;
//	HRESULT locHR = S_OK;
//
//	if (WAIT_OBJECT_0 == WaitForSingleObject(mEventHandle, TIMEOUT_WAIT))   { 
//			while (locHR = mMediaEvent->GetEvent(&locEventCode, &locParam1, &locParam2, 0), SUCCEEDED(locHR)) 
//			{
//	            
//				//cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
//				
//				//This is dodgy ! param1 and 2 are actually pointers !!
//				if (mDNMediaEvent != NULL) {
//					mDNMediaEvent->eventNotification(locEventCode, locParam1, locParam2);
//				}
//
//				mMediaEvent->FreeEventParams(locEventCode, locParam1, locParam2);
//			}
//	}
//	return true;
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
	            
				*debugLog<<"Event : "<<locEventCode<<" Params : "<<locParam1<<", "<<locParam2<<endl;
				
				
				if (mMediaEventNotify != NULL) {
					mMediaEventNotify->eventNotification(locEventCode, locParam1, locParam2);
				}

				mMediaEvent->FreeEventParams(locEventCode, locParam1, locParam2);
			}
	}
	return true;
}


DSPlayer::~DSPlayer(void) {
	*debugLog<<"Killing DSPlayer"<<endl;
	debugLog->close();
	
	releaseInterfaces();
	delete debugLog;
	CoUninitialize();
}


void DSPlayer::repaint() 
{
#ifndef WINCE
	if ((mVideoRenderType == VR_VMR9) && (mVMR9Window != NULL)) {
		HDC locHDC = GetDC(NULL);

		mVMR9Window->RepaintVideo(mWindowHandle, locHDC);
	}
#endif
}


long DSPlayer::getVolume() {
	long retVolume = -10000;
	if (mBasicAudio != NULL) {
		HRESULT locHR = mBasicAudio->get_Volume(&retVolume);
		if (locHR != S_OK) {
			retVolume = -10000;
		}
	}
	return retVolume;
}


long DSPlayer::getBalance() {
	long retBalance = 0;
	if (mBasicAudio != NULL) {
		HRESULT locHR = mBasicAudio->get_Balance(&retBalance);
		if (locHR != S_OK) {
			retBalance = 0;
		}
	}
	return retBalance;
}


bool DSPlayer::setVolume(long inVolume) {

	if (mBasicAudio != NULL) {
		HRESULT locHR = mBasicAudio->put_Volume(inVolume);
		if (locHR == S_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}


bool DSPlayer::setBalance(long inBalance) {
	if (mBasicAudio != NULL) {
		HRESULT locHR = mBasicAudio->put_Balance(inBalance);
		if (locHR == S_OK) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
void DSPlayer::releaseInterfaces() {

	mVideoRenderType = VR_NONE;
	*debugLog<<"********* Releasing interfaces *********"<<endl;
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

	if (mBasicAudio != NULL) {
		numRef = 
			mBasicAudio->Release();

		*debugLog<<"BasicAudio count = "<<numRef<<endl;
		mBasicAudio = NULL;
	}


	//if (mCMMLAppControl != NULL) {
	//	numRef = 
	//		mCMMLAppControl->Release();

	//	*debugLog<<"CMML App control count = "<<numRef<<endl;
	//	mCMMLAppControl = NULL;
	//}

	if(mVideoRenderFilter != NULL) {
		numRef =
            mVideoRenderFilter->Release();

		*debugLog<<"Video Render Filter count = "<<numRef<<endl;
		mVideoRenderFilter = NULL;
	}


	if (mVideoWindow != NULL) {
		numRef =
            mVideoWindow->Release();

		*debugLog<<"Video Window count = "<<numRef<<endl;
		mVideoWindow = NULL;
	}
#ifndef WINCE
	if (mVideoFrameStep != NULL) {
		numRef =
            mVideoFrameStep->Release();

		*debugLog<<"Video Frame Step count = "<<numRef<<endl;
		mVideoFrameStep = NULL;
	}

	if (mVMR9Window != NULL) {
		numRef =
            mVMR9Window->Release();

		*debugLog<<"Video Window VMR9 count = "<<numRef<<endl;
		mVMR9Window = NULL;
	}

	if (mVMR7Window != NULL) {
		numRef =
            mVMR7Window->Release();

		*debugLog<<"Video Window VMR7 count = "<<numRef<<endl;
		mVMR7Window = NULL;
	}
#endif
		*debugLog<<"Before Graph release..."<<endl;
	if (mGraphBuilder != NULL) {
		numRef =
            mGraphBuilder->Release();

		*debugLog<<"Graph Builder count = "<<numRef<<endl;
		mGraphBuilder = NULL;
	}


	*debugLog<<"****** After graph release ******"<<endl;

	mOggBaseTime = NULL;
	mIsLoaded = false;
	mGotVideoInformation = false;
	mVideoWidth = 0;
	mVideoHeight = 0;
	//TODO::: Release everything !
}

void DSPlayer::GetVideoInformation() {
	//Check there's even a video renderer.

	*debugLog<<"GetVideoInformation:::"<<endl;
	if (mVideoRenderFilter != NULL) {
		*debugLog<<"GetVideoInformation::: Has video renderer of type "<<(int)mVideoRenderType<< endl;
		//Get an enumerator for the pins
		IEnumPins* locEnumPins = NULL;
		HRESULT locHR = mVideoRenderFilter->EnumPins(&locEnumPins);
		if (locHR == S_OK) {
			*debugLog<<"GetVideoInformation::: got renderer pin enum"<<endl;
			//Get the first pin
			IPin* locPin = NULL;
			ULONG locHowMany = 0;
			locHR = locEnumPins->Next(1, &locPin, &locHowMany);

			
			if (locHR == S_OK) {
				*debugLog<<"GetVideoInformation::: got the first pin"<<endl;
				//Get the media type for the connection
				

				//Debug code
				IPin* locTestConnection = NULL;
				HRESULT locTestHR = locPin->ConnectedTo(&locTestConnection);
				*debugLog<<"ConnectedTo HRESULT first pin - "<<locTestHR<<endl;
				if (locTestConnection == NULL) {
					*debugLog<<"First pin is not connection to anything"<<endl;
				} else {
					locTestConnection->Release();
				}
				//Debug code ends

				AM_MEDIA_TYPE locMediaType;
				locHR = locPin->ConnectionMediaType(&locMediaType);

				if (locHR != S_OK) {
					*debugLog<<"GetVideoInformation::: FAILED to get connection type of first pin"<<endl;
					*debugLog<<"HRESULT was = "<<(int)locHR<<endl;
					do {
						locPin->Release();
						locPin = NULL;
						locHR = locEnumPins->Next(1, &locPin, &locHowMany);
						if (locHR == S_OK) {
							*debugLog<<"GetVideoInformation::: got anoter pin"<<endl;

							//Debug code
							locPin->ConnectedTo(&locTestConnection);
							if (locTestConnection == NULL) {
								*debugLog<<"Another pin is not connection to anything"<<endl;
							} else {
								locTestConnection->Release();
							}
							//End debug code

							locHR = locPin->ConnectionMediaType(&locMediaType);
							if (locHR == S_OK) {
								*debugLog<<"GetVideoInformation::: got another pins media type"<<endl;
							} else {
								*debugLog<<"GetVideoInformation::: FAILED to get media type of another pin"<<endl;
							}
						} else {
							*debugLog<<"GetVideoInformation::: could not get any more pins"<<endl;
							break;
						}
					} while (locHR != S_OK);
				}
				if (locHR == S_OK) {
					*debugLog<<"GetVideoInformation::: got a valid connection media type"<<endl;
					//Make sure it's video
					if (locMediaType.formattype == FORMAT_VideoInfo) {
						VIDEOINFOHEADER* locVideoInfo = (VIDEOINFOHEADER*)locMediaType.pbFormat;

						//Get the info we need
						mAvgTimePerFrame = locVideoInfo->AvgTimePerFrame;
						mVideoWidth = locVideoInfo->bmiHeader.biWidth;
						mVideoHeight = locVideoInfo->bmiHeader.biHeight;

						

					} else if (locMediaType.formattype == FORMAT_VideoInfo2) {
						VIDEOINFOHEADER2* locVideoInfo = (VIDEOINFOHEADER2*)locMediaType.pbFormat;
						//Get the info we need
						mAvgTimePerFrame = locVideoInfo->AvgTimePerFrame;
						mVideoWidth = locVideoInfo->bmiHeader.biWidth;
						mVideoHeight = locVideoInfo->bmiHeader.biHeight;
					} else {
						mAvgTimePerFrame = 0;
						mVideoWidth = 0;
						mVideoHeight = 0;
					}

					//We correctly checked the video information, vid width height can still
					//	be 0 if it was audio. But we know for sure that we didn't check too early
					mGotVideoInformation = true;

					//Free the format block
					if ((locMediaType.cbFormat != 0) && (locMediaType.pbFormat != NULL)) {
						CoTaskMemFree(locMediaType.pbFormat);
					}

					locPin->Release();
					locEnumPins->Release();
					return;

				} else {
					//Failed to get media type or not conencted
					if (locPin != NULL) {
						locPin->Release();
					}
					locEnumPins->Release();
					
				}
			} else {
				//Failed to get pin
				locEnumPins->Release();
			}
		} else {
			//Failed to get enumerator
		}
	} else {
		//There is no video renderer
	}

	mAvgTimePerFrame = 0;
	mVideoWidth = 0;
	mVideoHeight = 0;

}
bool DSPlayer::loadFile(wstring inFileName, HWND inWindowHandle, int inLeft, int inTop, int inWidth, int inHeight)
{
	mWindowHandle = inWindowHandle;
	mLeft = inLeft;
	mTop = inTop;
	mWidth = inWidth;
	mHeight = inHeight;
	*debugLog<<"HWND = "<<(int)inWindowHandle<<endl;
	*debugLog<<"Size of string = "<<inFileName.size();
	for (int i = 0; i < inFileName.size(); i++) {
		*debugLog<<(int)inFileName[i]<< ":";
		*debugLog<<(char)inFileName[i]<< " - ";
	}
	*debugLog<<endl;
	
	return loadFile(inFileName);
}

bool DSPlayer::setWindowPosition(int inLeft, int inTop, int inWidth, int inHeight)
{
#ifndef WINCE
	if (mVMR9Window != NULL) {
		//The incoming co-ordinate system should be the 0's in the corner (top left)
		//	and the height and width of the clipping window. This will be used as the boundary to
		//	perform aspect ratio correction.

		int locTopPad = 0;
		int locLeftPad = 0;

		if (!mGotVideoInformation) {
			GetVideoInformation();
		}	

		if	(		(mVideoWidth != 0) 
				&&	(mVideoHeight != 0)
				&&	(inWidth != 0)
				&&	(inHeight != 0)
			) 
		{
			//There is a known video width
			double locVideoRatio = (double)mVideoWidth / (double)mVideoHeight;
			double locWindowRatio = (double)inWidth / (double)inHeight;

			
			if (locWindowRatio > locVideoRatio) {
				//Sides are padded	
				locLeftPad = (int)((inWidth - ((double)inHeight * locVideoRatio)) / 2.0);
				locTopPad = 0;
			} else {
				//Letterbox
				locTopPad = (int)((inHeight - ((double)inWidth / locVideoRatio)) / 2.0);
				locLeftPad = 0;
			}

		}
		mLeft = inLeft + locLeftPad;
		mTop = inTop + locTopPad;
		mWidth = inWidth - (2*locLeftPad);
		mHeight = inHeight - (2*locTopPad);

		RECT locRect;
		locRect.left = mLeft;
		locRect.top = mTop;
		locRect.right = mLeft + mWidth;
		locRect.bottom = mTop + mHeight;

		
		HRESULT locHR = mVMR9Window->SetVideoPosition(NULL, &locRect);
		
	
		return true;
	} else {
		return false;
	}
#else
	return false;
#endif
}
bool DSPlayer::loadFile(wstring inFileName) 
{

	//Debugging only
	ULONG numRef = 0;
	//

	bool locDeferWindowSetup = false;


	//TODO:::: This should only do this for annodex right now. It should leave alone other
	//				query URLS\'s.
	size_t locQPos = inFileName.find(L'?');
	if (locQPos != wstring::npos) {
		//This has a question mark in the url/filename
		//Directshow has a serious bug in it's url parsing, and will not recognise
		//	extensions when suffixed with query strings. So we have to get around this
		//
		//What we do is add an extra string of characters to the end, which hopefully, could never
		//	occur by chance. eg XXS##$$@$.anx giving it a .anx extension as far as directshow is concerned
		//
		//Then inside the directshow filter, we always check for the strange string, and when we find it, strip
		//	it from the url before continuing to process it.

		inFileName += L"XsZZfQ__WiiPFD.anx";
	}



	releaseInterfaces();
	HRESULT locHR = S_OK;

	wstring locWFileName = inFileName; //illiminable::libDSPlayerDotNET::toWStr(locFileName);
	

	//Get filter graph with a graph builder interface
	IGraphBuilder* locGraphBuilder = NULL;
	locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);
	mGraphBuilder = locGraphBuilder;
	
	//Bail out if that failed, we can't build a graph without a filter graph
	if (locHR != S_OK) {
		mIsLoaded = false;
		return false;
	}
	*debugLog<<"Made filter graphs"<<endl;
	
	

	mVideoRenderType = VR_NONE;

#ifndef WINCE
	//Attempt to use VMR9
	IBaseFilter* locVMR9 = NULL;

	//First preference is a VMR 9 renderer
	*debugLog<<"Attempting VMR9 creation... making it !"<<endl;
	locHR= CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR9);
	locVMR9->AddRef();
	*debugLog<<"Initial VMR9 ref count = "<<locVMR9->Release()<<endl;
	if (locHR == S_OK) {
		locHR = mGraphBuilder->AddFilter(locVMR9, L"Video Mixing Renderer 9");
		
		//TODO::: Need an error check ?
		if (locHR != S_OK) {
			*debugLog<<"FAILED to add VMR9 to the graph - "<<(int)locHR<<endl;
		}

		IVMRFilterConfig9* locVMR9Config = NULL;
		locHR = locVMR9->QueryInterface(IID_IVMRFilterConfig9, (void**)&locVMR9Config);

		if (locHR == S_OK) {
			locVMR9Config->SetRenderingMode(VMRMode_Windowless);
			locVMR9Config->Release();

			//Get the windowless control
			IVMRWindowlessControl9* locVMR9Windowless = NULL;
			locHR = locVMR9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&locVMR9Windowless);

			if (locHR == S_OK) {
				mVMR9Window = locVMR9Windowless;
				mVideoRenderType = VR_VMR9;
				*debugLog<<"We got our VMR9 window"<<endl;
			}

		}
		
		numRef = locVMR9->Release();
		*debugLog<<"VMR9 ref count = "<<numRef<<endl;
		mVideoRenderFilter = locVMR9;
	}

	//If the video renderer type wasn't set, it means we couldn't get a VMR9
	//	Next best is to try for a VMR7
	if (mVideoRenderType == VR_NONE) {
		//Attempt to use VMR7
		IBaseFilter* locVMR7 = NULL;

		*debugLog<<"Attempting VMR7 creation... making it !"<<endl;
		locHR= CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&locVMR7);
		if (locHR == S_OK) {
			locHR = mGraphBuilder->AddFilter(locVMR7, L"Video Mixing Renderer 7");
			
			//TODO::: Need an error check ?

			IVMRFilterConfig* locVMR7Config = NULL;
			locHR = locVMR7->QueryInterface(IID_IVMRFilterConfig, (void**)&locVMR7Config);

			if (locHR == S_OK) {
				locVMR7Config->SetRenderingMode(VMRMode_Windowless);
				locVMR7Config->Release();

				//Get the windowless control
				IVMRWindowlessControl* locVMR7Windowless = NULL;
				locHR = locVMR7->QueryInterface(IID_IVMRWindowlessControl, (void**)&locVMR7Windowless);

				if (locHR == S_OK) {
					mVMR7Window = locVMR7Windowless;
					mVideoRenderType = VR_VMR7;
				}

			}
			
			numRef = locVMR7->Release();
			//*debugLog<<"VMR7 ref count = "<<numRef<<endl;

			mVideoRenderFilter = locVMR7;
		}
	}
#endif
	if (mWindowHandle != NULL) {
		//If we are here, the user provided a HWND it wants us to draw in, so
		//	try and use that.
		*debugLog<<"Setting up video window pointer..."<<endl;

#ifndef WINCE
		IVMRWindowlessControl9* locVMR9Window = NULL;
		IVMRWindowlessControl* locVMR7Window = NULL;
#endif
		IVideoWindow* locVideoWindow = NULL;

		switch ((int)mVideoRenderType) {
#ifndef WINCE
			case VR_VMR9:
				*debugLog<<"Attemping to use VMR9 windowless"<<endl;
				//Get the IVMRWindowlessControl9 interface.
				
				locHR = locVMR9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&locVMR9Window);
	
				if (locHR == S_OK) {
					*debugLog<<"Got VMR9 windowless interface"<<endl;
					mVMR9Window = locVMR9Window;
					
					locHR = mVMR9Window->SetVideoClippingWindow(  ((HWND)(mWindowHandle)));
					if (locHR == S_OK) {
						*debugLog<<"Clipping window set"<<endl;
						RECT locRect;
						locRect.left = mLeft;
						locRect.top = mTop;
						locRect.right = mLeft + mWidth;
						locRect.bottom = mTop + mHeight;
	
						locHR = mVMR9Window->SetVideoPosition(NULL, &locRect);
						if (locHR == S_OK) {
							*debugLog<<"video pos set"<<endl;
						} else {
							*debugLog<<"video pos set FAILED"<<endl;
						}
					} else {
						*debugLog<<"Clipping window set FAILED"<<endl;
					}
				}
				

				break;
			case VR_VMR7:
				//TODO::: Need to handle this.
				break;
#endif
			default:
				//Get the IVideoWindow interface.

				locHR = mGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&locVideoWindow);
	
				if (locHR == S_OK) {
					locDeferWindowSetup = true;
					//*debugLog<<"Got IVideoWindow"<<endl;
					mVideoWindow = locVideoWindow;
					//HRESULT locVHR = S_OK;
					//locVHR = mVideoWindow->put_Owner((OAHWND)mWindowHandle);
					//*debugLog<<"Put owner = "<<locVHR<<endl;
					//locVHR = mVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
					//*debugLog<<"Put window style = "<<locVHR<<endl;

					//RECT locRect;
					//GetClientRect(mWindowHandle, &locRect);
					//locVHR = mVideoWindow->SetWindowPosition(0, 0, locRect.right, locRect.bottom);
					//*debugLog<<"Set win pos = "<<locVHR<<endl;
					
				}
				break;
		}
	}




	*debugLog<<"About to call render on "<<endl;
	for (int i = 0; i < inFileName.size(); i++) {
		*debugLog<<(int)inFileName[i]<< ":";
		*debugLog<<(char)inFileName[i]<< " - ";
	}
	*debugLog<<endl;
	//Build the graph
#ifndef DSPLAY_OGG_SPECIFIC
	locHR = mGraphBuilder->RenderFile(locWFileName.c_str(), NULL);
	if (locHR != S_OK) {
		if (locHR < 0) {
			*debugLog<<"Render File FAILED !!"<<endl;
			*debugLog<<"Error is "<<locHR<<endl;
			mIsLoaded = false;
			return false;
		} else {
			*debugLog<<"Render file Partial success"<<endl;
			*debugLog<<"Code is "<<locHR<<endl;
		}
	}

#else

	*debugLog<<"Custom graph building for ogg"<<endl;
	IBaseFilter* locDemuxer = NULL;
	IFileSourceFilter* locFS = NULL;
	//HRESULT locHR = S_FALSE;
	
	locHR = CoCreateInstance(CLSID_XX_OggDemux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locDemuxer);

	*debugLog<<"Demux create = "<<locHR<<endl;
	locHR = mGraphBuilder->AddFilter(locDemuxer, L"Custom Ogg Source");
	*debugLog<<"Add demux to graph= "<<locHR<<endl;

	locHR = locDemuxer->QueryInterface(IID_IFileSourceFilter, (void**)&locFS);

	locHR = locFS->Load(locWFileName.c_str(), NULL);
	*debugLog<<"Load = "<<locHR<<endl;


	IEnumPins* locPinEnum = NULL;

	locDemuxer->EnumPins(&locPinEnum);

	

	IBaseFilter* locVorbisDecoder = NULL;
	IBaseFilter* locTheoraDecoder = NULL;
	IPin* locPin = NULL;
	ULONG locHowMany = 0;
	ULONG locHowManyDecoderPins = 0;
	while (locPinEnum->Next(1, &locPin, &locHowMany) == S_OK) {
		//locHR = locGraphBuilder->Render(locPin);
		*debugLog<<"Pre Pin render attempt vorbis = "<<locHR<<endl;

		locHR = CoCreateInstance(CLSID_XX_VorbisDecoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locVorbisDecoder);
		*debugLog<<"Create vorbis decoder = "<<locHR<<endl;
		
		locHR = mGraphBuilder->AddFilter(locVorbisDecoder, L"Vorbis Decoder");
		*debugLog<<"Add vorbis filter = "<<locHR<<endl;

		IEnumPins* locDecoderPinEnum = NULL;
		locHR = locVorbisDecoder->EnumPins(&locDecoderPinEnum);
		*debugLog<<"Get vorbis decoder pin enum = "<<locHR<<endl;

		IPin* locDecoderPin = NULL;
		IPin* locDecoderOutputPin = NULL;
		locHR = locDecoderPinEnum->Next(1, &locDecoderPin, &locHowManyDecoderPins);
		*debugLog<<"Get vorb pin from enum = "<<locHR<<endl;
		PIN_DIRECTION locDirn;
		locHR = locDecoderPin->QueryDirection(&locDirn);
		*debugLog<<"Query dirn = "<<locHR<<endl;
		if (locDirn != PINDIR_INPUT) {
			*debugLog<<"First pin NOT AN INPUT PIN"<<endl;
			//locDecoderPin->Release();
			locDecoderOutputPin = locDecoderPin;
			locDecoderPinEnum->Next(1, &locDecoderPin, &locHowManyDecoderPins);
		}

		*debugLog<<"Pre connect attempt for vorbis"<<endl;
		locHR = mGraphBuilder->ConnectDirect(locPin, locDecoderPin, NULL);
		*debugLog<<"Attempt connection to vorbis = "<<locHR<<endl;

		if (locHR != S_OK) {
			*debugLog<<"Begin theora attempt"<<endl;

			locDecoderOutputPin = NULL;
			locDecoderPin->Release();
			locDecoderPinEnum->Release();
			mGraphBuilder->RemoveFilter(locVorbisDecoder);
			locVorbisDecoder->Release();


			locHR = CoCreateInstance(CLSID_XX_TheoraDecoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locTheoraDecoder);
			*debugLog<<"Create theora filter = "<<locHR<<endl;
			
			locHR = mGraphBuilder->AddFilter(locTheoraDecoder, L"Theora Decoder");
			*debugLog<<"Add theora filter = "<<locHR<<endl;

			locDecoderPinEnum = NULL;
			locHR = locTheoraDecoder->EnumPins(&locDecoderPinEnum);
			*debugLog<<"Enum theo pins = "<<endl;

			locDecoderPin = NULL;
			locDecoderPinEnum->Next(1, &locDecoderPin, &locHowManyDecoderPins);
			PIN_DIRECTION locDirn;
			locDecoderPin->QueryDirection(&locDirn);
			if (locDirn != PINDIR_INPUT) {
				//locDecoderPin->Release();
				locDecoderOutputPin = locDecoderPin;
				locDecoderPinEnum->Next(1, &locDecoderPin, &locHowManyDecoderPins);
			}

			*debugLog<<"Pre theora connect attempt"<<endl;
			locHR = mGraphBuilder->ConnectDirect(locPin, locDecoderPin,NULL);
			*debugLog<<"Attempt connection to theora = "<<locHR<<endl;

			if (locHR != S_OK) {
				*debugLog<<"FAILED TO CONNECT TO THEORA FILTER"<<endl;
				locDecoderPin->Release();
				locDecoderPinEnum->Release();
				mGraphBuilder->RemoveFilter(locTheoraDecoder);
				locTheoraDecoder->Release();

			} else {
				*debugLog<<"++ Successfully conencted to theora filter"<<endl;
				if (locDecoderOutputPin == NULL) {
					locDecoderPinEnum->Next(1, &locDecoderOutputPin, &locHowManyDecoderPins);
				}

				locHR = mGraphBuilder->Render(locDecoderOutputPin);
				*debugLog<<"Rendering theora output pin = "<<locHR<<endl;
			}
		} else {
			*debugLog<<"++ Successfully connected to vorbis filter"<<endl;
			if (locDecoderOutputPin != NULL) {

			} else {
				locDecoderPinEnum->Next(1, &locDecoderOutputPin, &locHowManyDecoderPins);
			}

			locHR = mGraphBuilder->Render(locDecoderOutputPin);
			*debugLog<<"Rendering vorbis out put pin = "<<locHR<<endl;
		}



		locPin->Release();
		locPin = NULL;
	}

	
#endif

	if (locDeferWindowSetup) {

		*debugLog<<"Got IVideoWindow"<<endl;
		
		HRESULT locVHR = S_OK;
		locVHR = mVideoWindow->put_Owner((OAHWND)mWindowHandle);
		*debugLog<<"Put owner = "<<locVHR<<endl;
		locVHR = mVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
		*debugLog<<"Put window style = "<<locVHR<<endl;

		RECT locRect;
		GetClientRect(mWindowHandle, &locRect);
		locVHR = mVideoWindow->SetWindowPosition(0, 0, locRect.right, locRect.bottom);
		*debugLog<<"Set win pos = "<<locVHR<<endl;
	}


	*debugLog<<"Pre video ifo"<<endl;

	//CHANGES::: Use this to get information about the video, once it's been rendered.
	GetVideoInformation();

	*debugLog<<"Render must have been ok"<<endl;


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

	//Get the IBasicAudio Interface
	IBasicAudio* locBasicAudio = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&locBasicAudio);
	if (locHR == S_OK) {
		mBasicAudio = locBasicAudio;
	} else {
		mBasicAudio = NULL;
	}

	*debugLog<<"After various interfaces got"<<endl;

#ifndef WINCE
	//Get the IVideFrameStep if ity exists
	IVideoFrameStep* locVideoStep = NULL;
	locHR = mGraphBuilder->QueryInterface(IID_IVideoFrameStep, (void**)&locVideoStep);
	if (locHR == S_OK) {
		mVideoFrameStep = locVideoStep;
	} else {
		mVideoFrameStep = NULL;
	}
#endif

	IOggBaseTime* locOggBaseTime = NULL;
	IBaseFilter* locOggDemuxFilter = NULL;
	IBaseFilter* locTestFilter = NULL;


	IEnumFilters* locFilterEnum = NULL;
	locHR = mGraphBuilder->EnumFilters(&locFilterEnum);
	if (locHR == S_OK) {
		bool locDoneSearch = false;
		ULONG locHowMany = 0;
		while (!locDoneSearch) {

			locHR = locFilterEnum->Next(1, &locTestFilter, &locHowMany);
			if (locHR == S_OK) {
				*debugLog<<"Found filter in graph..."<<endl;
				CLSID locCLSID;
				if (locTestFilter->GetClassID(&locCLSID) == S_OK) {
					if (locCLSID == X_CLSID_OggDemuxPacketSourceFilter) {
						locOggDemuxFilter = locTestFilter;
						//Addref here... since we always do a release below
						locOggDemuxFilter->AddRef();
						//locDoneSearch = true;
						
					}
					*debugLog<<"Filter guid = "<<locCLSID.Data1<<"-"<<locCLSID.Data2<<"-"<<locCLSID.Data3<<endl;
				}

				locTestFilter->Release();
			} else {
				locDoneSearch = true;
				locOggDemuxFilter = NULL;
			}
		}

		locFilterEnum->Release();


		if (locOggDemuxFilter != NULL) {
			//There is an ogg filter - this query does not addref
			locHR = locOggDemuxFilter->QueryInterface(IID_IOggBaseTime, (void**)&locOggBaseTime);
			if (locHR == S_OK) {
				mOggBaseTime = locOggBaseTime;
			}
			locOggDemuxFilter->Release();
			locOggDemuxFilter = NULL;
		}

	}

	*debugLog<<"Almost done"<<endl;

	//// {EB5AED9C-8CD0-4c4b-B5E8-F5D10AD1314D}
	//DEFINE_GUID(IID_IOggBaseTime, 
	//0xeb5aed9c, 0x8cd0, 0x4c4b, 0xb5, 0xe8, 0xf5, 0xd1, 0xa, 0xd1, 0x31, 0x4d);



	//if (locHR == S_OK) {
	//	//There is an ogg filter - this query does not addref
	//	locHR = locOggDemuxFilter->QueryInterface(IID_IOggBaseTime, (void**)&locOggBaseTime);
	//	if (locHR == S_OK) {
	//		mOggBaseTime = locOggBaseTime;
	//	}
	//	locOggDemuxFilter->Release();
	//	locOggDemuxFilter = NULL;
	//}


//	if (FAILED(hr))

	return true;


}

//System::Drawing::Bitmap* DSPlayer::GetImage() {
//	HRESULT locHR = S_OK;
//	System::Drawing::Bitmap* locBitmap = NULL;
//	BYTE* locBytes = NULL;
//
//	switch (mVideoRenderType) {
//		case VR_VMR9:
//			if (mVMR9Window != NULL) {
//				locHR = mVMR9Window->GetCurrentImage(&locBytes);
//				if (locHR == S_OK) {
//					BITMAPINFOHEADER* locBMIH = (BITMAPINFOHEADER*) locBytes;
//					*debugLog<<"BHIM : Bit count = "<<locBMIH->biBitCount<<endl;
//					*debugLog<<"BHIM : Compresio = "<<locBMIH->biCompression<<endl;
//					*debugLog<<"BHIM : Colours   = "<<locBMIH->biClrUsed<<endl;
//					*debugLog<<"BHIM : Width     = "<<locBMIH->biWidth<<endl;
//					*debugLog<<"BHIM : Height    = "<<locBMIH->biHeight<<endl;
//					*debugLog<<"BHIM : Size Img  = "<<locBMIH->biSizeImage<<endl;
//					*debugLog<<"BHIM : Size      = "<<locBMIH->biSize<<endl;
//					*debugLog<<"BHIM : Size BMIH = "<<sizeof(BITMAPINFOHEADER)<<endl;
//					*debugLog<<"BHIM : Size BMI  = "<<sizeof(BITMAPINFO)<<endl;
//
//					
//
//					
//
//
//					if (locBMIH->biBitCount == 32) {
//						unsigned char* locBuffer = new unsigned char[locBMIH->biSizeImage];
//						memcpy((void*)locBuffer, (const void*)(locBytes + 44), locBMIH->biSizeImage);
//						locBitmap = new System::Drawing::Bitmap(locBMIH->biWidth, locBMIH->biHeight, locBMIH->biWidth * 4, System::Drawing::Imaging::PixelFormat::Format32bppRgb, (System::IntPtr)(locBuffer));
//						locBitmap->RotateFlip(System::Drawing::RotateFlipType::RotateNoneFlipY);
//					}
//
//					/* .... */
//					
//					
//					CoTaskMemFree(locBytes);
//					
//
//				}
//
//			}
//
//		default:
//			break;
//	};
//
//	return locBitmap;;
//}

//bool DSPlayer::setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks) {
//	return mCMMLProxy->setManagedDelegate(inCMMLCallbacks);
//}

bool DSPlayer::isLoaded() {
	return mIsLoaded;
}
bool DSPlayer::play() {
	*debugLog<<"Play..."<<endl;
	if (mIsLoaded) {
		HRESULT locHR = mMediaControl->Run();
		if (SUCCEEDED(locHR)) {
			return true;
		} else {
			return false;
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
			seek(0);
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

bool DSPlayer::canStepFrame() {
#ifndef WINCE
	if (mVideoFrameStep != NULL) {
		HRESULT locHR = mVideoFrameStep->CanStep(0, NULL);
		if (locHR == S_OK) {
			return true;
		}
	}
#endif
	return false;
}

bool DSPlayer::stepFrame() {
#ifndef WINCE
	if (mVideoFrameStep != NULL) {
		HRESULT locHR = mVideoFrameStep->Step(1, NULL);
		if (locHR == S_OK) {
			return true;
		}
	}
#endif
	return false;
}

__int64 DSPlayer::seekStart() {
	return seek(0);
}

__int64 DSPlayer::queryPosition() {
	if (mIsLoaded && (mMediaSeeking != NULL)) {
		__int64 locStart = -1;
		__int64 locStop = -1;
		HRESULT locHR = mMediaSeeking->GetPositions(&locStart, &locStop);
		if (locStart != -1) {
			//TODO::: This really should try and be in the filter
			if (mOggBaseTime != NULL) {
				locStart += mOggBaseTime->getGlobalBaseTime();
			}
			return locStart;
		} else {
			return -1;
		}
	} else {
		return -1;	
	}
}

__int64 DSPlayer::fileSize() {
	if (mIsLoaded) {
		return mFileSize;	
	} else {
		return -1;
	}
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

//bool DSPlayer::isFileAnnodex(String* inFilename)
//{
//	String* locExt = (inFilename->Substring(inFilename->Length - 4, 4))->ToUpper();
//	if (locExt->Equals(".ANX")) {
//		return true;
//	} else {
//		return false;
//	}
//}

//bool DSPlayer::setMediaEventCallback(IDNMediaEvent* inMediaEventCallback) {
//	mDNMediaEvent = inMediaEventCallback;
//	return true;
//}
//IDNMediaEvent* DSPlayer::getMediaEventCallback() {
//	return mDNMediaEvent;
//}

bool DSPlayer::setMediaEventCallback(IMediaEventNotification* inMediaEventCallback) {
	mMediaEventNotify = inMediaEventCallback;
	return true;
}
IMediaEventNotification* DSPlayer::getMediaEventCallback() {
	return mMediaEventNotify;
}
		/// Gets the average time per frame in ds units. Returns 0 if unknown or no video.
__int64 DSPlayer::averageTimePerFrame() {
	return mAvgTimePerFrame;
}

		/// Gets the average frame rate in fps*100 (ie 29.97 fps = 2997)
__int64 DSPlayer::averageFrameRate() {

	if (mAvgTimePerFrame != 0) {
		__int64 locFrameRate = (10000000 * 100);
		locFrameRate /= mAvgTimePerFrame;
		return locFrameRate;
	} else {
		return 0;
	}
}

		/// Gets the width of the video data. Not necessarily the same as the display size.
int DSPlayer::videoWidth() {
	return mVideoWidth;
}

		/// Gets the height of the video data. Not necessarily the same as the display size.
int DSPlayer::videoHeight() {
	return mVideoHeight;
}


