#pragma once

#include "DSPlayer.h"
DSPlayer* gDSPlay = NULL;




//DSPlayer(void);
//
///// Constructor takes a HWND and the video location/size wrt to the window the handle refers to.
//DSPlayer(HWND inWindowHandle, int inLeft, int inTop, int inWidth, int inHeight);
//~DSPlayer(void);

///Setup the exported library

extern "C" {
bool __cdecl c_initialise();
///Destroy the internal state of the library
bool c_destroy();

/// Load the named file into the player.
bool __cdecl c_loadFileWithParams(wchar_t* inFileName, HWND inWindowHandle, int inLeft, int inTop, int inWidth, int inHeight);

bool c_loadFile(wstring inFileName);

bool c_setWindowPosition(int inLeft, int inTop, int inWidth, int inHeight);

/// Start playing the media file.
bool c_play();

/// Pause the media file.
bool c_pause();

/// Stop the media file.
bool c_stop();

/// Gets the current volume setting 100 units per db ie 10,000 = 100db
long c_getVolume();

/// Gets the balance of the channels. Divide by 100 to get attenuation. -10,000 = right channel silent.
long c_getBalance();

/// Sets the current volume (-10,000 to 10,000)
bool c_setVolume(long inVolume);

///Sets the current balance (-10,000 to 10,000)
bool c_setBalance(long inBalance);

/// Gets the average time per frame in ds units. Returns 0 if unknown or no video.
__int64 c_averageTimePerFrame();

/// Gets the average frame rate in fps*100 (ie 29.97 fps = 2997)
__int64 c_averageFrameRate();

/// Gets the width of the video data. Not necessarily the same as the display size.
int c_videoWidth();

/// Gets the height of the video data. Not necessarily the same as the display size.
int c_videoHeight();

/// Steps forward a single video frame. Check canStepFrame() to see if this is possible
bool c_stepFrame();

/// Checks if the graph can step forward frame by frame.
bool c_canStepFrame();

/// Seek to the specified time in 100 nanoseconds units. ie 10 000 000 per second.
__int64 c_seek(__int64 inTime);

/// Seek to the start of the media file.
__int64 c_seekStart();

/// Queries the time in the media in 100ns units. Returns -1 if unknown or no file loaded.
__int64 c_queryPosition();

/// Repaints the frame. Only needed for windowless rendering.
void c_repaint();

/// Returns if there is a file loaded.
bool c_isLoaded();

/// Returns the file size in bytes of the media file.
__int64 c_fileSize();

/// Returns the duration of the file in 100 nanosecond units. ie 10 000 000 per second.
__int64 c_fileDuration();

/// Returns a .NET Bitmap class of the current video frame. Requires Renderless mode.
//System::Drawing::Bitmap* GetImage();

/// Sets the callback for media events. Notably EC_COMPLETE.
bool c_setMediaEventCallback(IMediaEventNotification* inMediaEventCallback);

///// Returns a pointer to the current media event callback.
//IMediaEventNotification* getMediaEventCallback();

/// Sets the callback for CMMLTags. Only valid for annodex files.
//bool setCMMLCallbacks(IDNCMMLCallbacks* inCMMLCallbacks);

/// Triggers an event check, which if needed will fire the callbacks.
bool c_checkEvents();

/// Releases all the interfaces in use and unloads the file.
void c_releaseInterfaces();
}