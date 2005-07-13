//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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
#include "oggmuxdllstuff.h"
#include "OggMuxInputPin.h"
#include "IOggMuxProgress.h"
#include "IOggMuxSettings.h"
#include "PropsOggMux.h"
#include "BasicSeekPassThrough.h"
#include <libOOOgg/OggPageInterleaver.h>
#include <libOOOgg/INotifyComplete.h>

#include <string>

#include <fstream>
#include <libOOOgg/IOggCallback.h>
using namespace std;
#include <libilliCore/StringHelper.h>
class OggMuxInputPin;

class OGG_MUX_API OggMuxFilter
	:	public IFileSinkFilter
	,	public CBaseFilter
	,	public IOggCallback
	,	public IAMFilterMiscFlags
	,	public BasicSeekPassThrough
	,	public INotifyComplete
	,	public IOggMuxProgress
	,	public IOggMuxSettings
	,	public ISpecifyPropertyPages
{
public:
	OggMuxFilter(void);
	OggMuxFilter(REFCLSID inFilterGUID);
	virtual ~OggMuxFilter(void);


	friend class OggMuxInputPin;
	//Com Stuff
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	// *********************************************
	// ***** IAMFilterMiscFlags Implementation *****
	// *********************************************

	/// Allows the filter to return a flag to tell the graph it's a renderer.
	ULONG STDMETHODCALLTYPE GetMiscFlags(void);

	// ***************************************
	// ***** IOggCallback Implementation *****
	// ***************************************

	/// Takes an incoming page, usually from the interleaver.
	virtual bool acceptOggPage(OggPage* inOggPage);
	
	// ******************************************
	// ***** IFileSinkFilter Implementation *****
	// ******************************************

	/// Sets the filename to be used to output to
	STDMETHODIMP SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

	/// Gets the output filename this filter is currently using.
	STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);

	// *************************************
	// ***** CBaseFilter Pure Virtuals *****
	// *************************************

	/// Returns the number of pins this filter has
	virtual int GetPinCount();

	/// Returns the indexed pin or NULL.
	virtual CBasePin* GetPin(int inPinNo);

	// **********************************
	// ***** IMediaFilter Overrides *****
	// **********************************

	/// Called when the graph starts playing
	STDMETHODIMP Run(REFERENCE_TIME tStart);

	/// Called when the graph pauses
	STDMETHODIMP Pause(void);

	/// Called when the graph stops
	STDMETHODIMP Stop(void);

	// ******************************************
	// ***** IOggMuxProgress Implementation *****
	// ******************************************

	/// Returns the time in 100 nanosecond units of the last page that was written.
	virtual STDMETHODIMP_(LONGLONG) getProgressTime();

	/// Returns the number of bytes written so far.
	virtual STDMETHODIMP_(LONGLONG) getBytesWritten();


	//Helpers
	virtual HRESULT addAnotherPin();
	virtual void NotifyComplete();

	//IMediaSeeking Override to give progress. - This is unreliable !!
	virtual STDMETHODIMP GetPositions(LONGLONG *pCurrent, LONGLONG *pStop);
	virtual STDMETHODIMP GetCurrentPosition(LONGLONG *pCurrent);

	//IOggMuxSettings Implementation
	STDMETHODIMP_(unsigned long) maxPacketsPerPage();
	STDMETHODIMP_(bool) setMaxPacketsPerPage(unsigned long inMaxPacketsPerPage);

	//SpecifyPropertyPages Implementation
	STDMETHODIMP OggMuxFilter::GetPages(CAUUID* outPropPages);

protected:

	bool SetupOutput();
	bool CloseOutput();

	wstring mFileName;
	vector<OggMuxInputPin*> mInputPins;

	OggPageInterleaver* mInterleaver;

	CCritSec* mStreamLock;

	fstream mOutputFile;
	fstream debugLog;
	HRESULT mHR;


};
