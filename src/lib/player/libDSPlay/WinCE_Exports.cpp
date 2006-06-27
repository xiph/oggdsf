#include "StdAfx.h"
#include "WinCE_Exports.h"


static const GUID CLSID_XX_OggDemux =  
				{ 0xc9361f5a, 0x3282, 0x4944, { 0x98, 0x99, 0x6d, 0x99, 0xcd, 0xc5, 0x37, 0xb } };

//0x5a1d945, 0xa794, 0x44ef, 0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55);
static const GUID CLSID_XX_VorbisDecoder =
{ 0x5a1d945, 0xa794, 0x44ef, { 0xb4, 0x1a, 0x2f, 0x85, 0x1a, 0x11, 0x71, 0x55 } };

//0x5187161, 0x5c36, 0x4324, 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d);
static const GUID  CLSID_XX_TheoraDecoder =
{ 0x5187161, 0x5c36, 0x4324, { 0xa7, 0x34, 0x22, 0xbf, 0x37, 0x50, 0x9f, 0x2d } };


///Setup the exported library
bool c_initialise()
{
	//CoInitialize(NULL);
	if (gDSPlay != NULL) {
		return false;
	} else {
		gDSPlay = new DSPlayer;
		return true;
	}
}
///Destroy the internal state of the library
bool c_destroy()
{
	delete gDSPlay;
	CoUninitialize();

	return true;
}

/// Load the named file into the player.
bool c_loadFileWithParams(wchar_t* inFileName, HWND inWindowHandle, int inLeft, int inTop, int inWidth, int inHeight)
{


	//HRESULT locHR;
	//IBaseFilter* locFilter = NULL;

	//fstream debugLog;
	//debugLog.open("\\Storage Card\\comstuff.txt", ios_base::out);

	//locHR = CoInitialize(NULL);
	//debugLog<<"Coinit = "<<locHR<<endl;

	//locHR = CoCreateInstance(CLSID_XX_VorbisDecoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locFilter);
	//debugLog<<"Vorb create = "<<locHR<<endl;

	//locHR = CoCreateInstance(CLSID_XX_OggDemux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locFilter);
	//debugLog<<"demux create = "<<locHR<<endl;

	//locHR = CoCreateInstance(CLSID_XX_TheoraDecoder, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locFilter);
	//debugLog<<"Theo create = "<<locHR<<endl;

	//debugLog.close();

	//return true;


	//IGraphBuilder* locGraphBuilder = NULL;
	//IMediaControl* locMediaControl = NULL;
	//HRESULT locHR = S_FALSE;;
	//CoInitialize(NULL);
	//IBaseFilter* locTheoDecode = NULL;
	//locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);

	//locHR = locGraphBuilder->RenderFile(L"\\Storage Card\\carbon.ogg", NULL);


	//			CoInitialize(NULL);
	//			fstream debugLog;
	//			debugLog.open("\\Storage Card\\internal.txt", ios_base::out);
	//			debugLog<<"Started"<<endl;
	//			

	//			


	//			IGraphBuilder* locGraphBuilder = NULL;
	//			IMediaControl* locMediaControl = NULL;
	//			IBaseFilter* locDemuxer = NULL;
	//			//ICustomSource* locCustomSourceSetter = NULL;
	//			IFileSourceFilter* locFS = NULL;
	//			HRESULT locHR = S_FALSE;;
	//			
	//			locHR = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&locGraphBuilder);

	//			debugLog<<"Graph Create = "<<locHR<<endl;
	//			locHR = CoCreateInstance(CLSID_XX_OggDemux, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&locDemuxer);

	//			debugLog<<"Demux create = "<<locHR<<endl;
	//			locHR = locGraphBuilder->AddFilter(locDemuxer, L"Custom Ogg Source");
	//			debugLog<<"Add demux = "<<locHR<<endl;

	//			locHR = locDemuxer->QueryInterface(IID_IFileSourceFilter, (void**)&locFS);

	//			locHR = locFS->Load(L"\\Storage Card\\do_you_see.ogg", NULL);
	//			debugLog<<"Load = "<<locHR<<endl;


	//			//CustomSourceClass* locCustomFileSourceInterface = new CustomSourceClass;
	//			//locCustomFileSourceInterface->open("D:\\testfile.ogg");
	//			//
	//			//locCustomSourceSetter->setCustomSourceAndLoad(locCustomFileSourceInterface);

	//			//Do not release, it's not really a COM interface
	//			//locCustomSourceSetter->Release();

	//			IEnumPins* locPinEnum = NULL;

	//			locDemuxer->EnumPins(&locPinEnum);

	//			

	//			IPin* locPin = NULL;
	//			ULONG locHowMany = 0;
	//			while (locPinEnum->Next(1, &locPin, &locHowMany) == S_OK) {
	//				locHR = locGraphBuilder->Render(locPin);
	//				debugLog<<"Pin render = "<<locHR<<endl;
	//				locPin->Release();
	//				locPin = NULL;
	//			}





	//locHR = locGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&locMediaControl);


	//locHR = locMediaControl->Run();

	//IMediaEvent* locMediaEvent = NULL;
	//locHR = locGraphBuilder->QueryInterface(IID_IMediaEvent, (void**)&locMediaEvent);
	//
	//HANDLE  hEvent; 
	//long    evCode, param1, param2;
	//BOOLEAN bDone = FALSE;
	//HRESULT hr = S_OK;
	//hr = locMediaEvent->GetEventHandle((OAEVENT*)&hEvent);
	//if (FAILED(hr))
	//{
	//	/* Insert failure-handling code here. */
	//}
	//while(!bDone) 
	//{
	//	if (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, 100))
	//	{ 
	//		while (hr = locMediaEvent->GetEvent(&evCode, &param1, &param2, 0), SUCCEEDED(hr)) 
	//		{
	//			//printf("Event code: %#04x\n Params: %d, %d\n", evCode, param1, param2);
	//			//cout<<"Event : "<<evCode<<" Params : "<<param1<<", "<<param2<<endl;
	//			locMediaEvent->FreeEventParams(evCode, param1, param2);
	//			bDone = (EC_COMPLETE == evCode);
	//		}
	//	}
	//} 

	////cout<<"Finished..."<<endl;
	//int x;
	////cin>>x;
	//locMediaControl->Release();
	//locGraphBuilder->Release();
	//CoUninitialize();
	//return true;
	

	





	wstring locFileName = inFileName;
	
	return gDSPlay->loadFile(locFileName, inWindowHandle, inLeft, inTop, inWidth, inHeight);
	
}

bool c_loadFile(wstring inFileName)
{
	return gDSPlay->loadFile(inFileName);
}

bool c_setWindowPosition(int inLeft, int inTop, int inWidth, int inHeight)
{
	return gDSPlay->setWindowPosition(inLeft, inTop, inWidth, inHeight);
}

/// Start playing the media file.
bool c_play()
{
	return gDSPlay->play();
}

/// Pause the media file.
bool c_pause()
{
	return gDSPlay->pause();
}

/// Stop the media file.
bool c_stop()
{
	return gDSPlay->stop();
}

/// Gets the current volume setting 100 units per db ie 10,000 = 100db
long c_getVolume()
{
	return gDSPlay->getVolume();
}

/// Gets the balance of the channels. Divide by 100 to get attenuation. -10,000 = right channel silent.
long c_getBalance()
{
	return gDSPlay->getBalance();
}

/// Sets the current volume (-10,000 to 10,000)
bool c_setVolume(long inVolume)
{
	return gDSPlay->setVolume(inVolume);
}

///Sets the current balance (-10,000 to 10,000)
bool c_setBalance(long inBalance)
{
	return gDSPlay->setBalance(inBalance);
}

/// Gets the average time per frame in ds units. Returns 0 if unknown or no video.
__int64 c_averageTimePerFrame()
{
	return gDSPlay->averageTimePerFrame();
}

/// Gets the average frame rate in fps*100 (ie 29.97 fps = 2997)
__int64 c_averageFrameRate()
{
	return gDSPlay->averageFrameRate();
}

/// Gets the width of the video data. Not necessarily the same as the display size.
int c_videoWidth()
{
	return gDSPlay->videoWidth();
}

/// Gets the height of the video data. Not necessarily the same as the display size.
int c_videoHeight()
{
	return gDSPlay->videoHeight();
}

/// Steps forward a single video frame. Check canStepFrame() to see if this is possible
bool c_stepFrame()
{
	return gDSPlay->stepFrame();
}

/// Checks if the graph can step forward frame by frame.
bool c_canStepFrame()
{
	return gDSPlay->canStepFrame();
}

/// Seek to the specified time in 100 nanoseconds units. ie 10 000 000 per second.
__int64 c_seek(__int64 inTime)
{
	return gDSPlay->seek(inTime);
}

/// Seek to the start of the media file.
__int64 c_seekStart()
{
	return gDSPlay->seekStart();
}

/// Queries the time in the media in 100ns units. Returns -1 if unknown or no file loaded.
__int64 c_queryPosition()
{
	return gDSPlay->queryPosition();
}

/// Repaints the frame. Only needed for windowless rendering.
void c_repaint()
{
	return gDSPlay->repaint();
}

/// Returns if there is a file loaded.
bool c_isLoaded()
{
	return gDSPlay->isLoaded();
}

/// Returns the file size in bytes of the media file.
__int64 c_fileSize()
{
	return gDSPlay->fileSize();
}

/// Returns the duration of the file in 100 nanosecond units. ie 10 000 000 per second.
__int64 c_fileDuration()
{
	return gDSPlay->fileDuration();
}

/// Returns a .NET Bitmap class of the current video frame. Requires Renderless mode.
//System::Drawing::Bitmap* GetImage();

/// Sets the callback for media events. Notably EC_COMPLETE.
bool c_setMediaEventCallback(IMediaEventNotification* inMediaEventCallback)
{
	return gDSPlay->setMediaEventCallback(inMediaEventCallback);
}
//
///// Returns a pointer to the current media event callback.
//IMediaEventNotification* getMediaEventCallback()
//{
//	return gDSPlay->getMediaEventCallback();
//}

/// Sets the callback for CMMLTags. Only valid for annodex files.
//bool setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks);

/// Triggers an event check, which if needed will fire the callbacks.
bool c_checkEvents()
{
	return gDSPlay->checkEvents();
}

/// Releases all the interfaces in use and unloads the file.
void c_releaseInterfaces()
{
	return gDSPlay->releaseInterfaces();
}

