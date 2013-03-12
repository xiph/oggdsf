//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//          (C) 2013 Cristian Adam
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
#include "FilterRegistration.h"
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
#include <libilliCore/StringHelper.h>

class OggMuxInputPin;

class OggMuxFilter
	:	public IFileSinkFilter
	,	public CBaseFilter
	,	public IOggCallback
	,	public IAMFilterMiscFlags
	,	public BasicSeekPassThrough
	,	public INotifyComplete
	,	public IOggMuxProgress
	,	public IOggMuxSettings
	//,	public ISpecifyPropertyPages
{
public:
	OggMuxFilter();
	OggMuxFilter(REFCLSID inFilterGUID);
	virtual ~OggMuxFilter();

	friend class OggMuxInputPin;
	DECLARE_IUNKNOWN

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	// IAMFilterMiscFlags Implementation

	/// Allows the filter to return a flag to tell the graph it's a renderer.
	ULONG __stdcall GetMiscFlags();

	// IOggCallback Implementation

	/// Takes an incoming page, usually from the interleaver.
	virtual bool acceptOggPage(OggPage* inOggPage);
	
	// IFileSinkFilter Implementation

	/// Sets the filename to be used to output to
	HRESULT __stdcall SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

	/// Gets the output filename this filter is currently using.
	HRESULT __stdcall GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);

	// CBaseFilter Pure virtuals

	/// Returns the number of pins this filter has
	virtual int GetPinCount();

	/// Returns the indexed pin or NULL.
	virtual CBasePin* GetPin(int inPinNo);

	// IMediaFilter Overrides

	/// Called when the graph starts playing
	HRESULT __stdcall Run(REFERENCE_TIME tStart);

	/// Called when the graph pauses
	HRESULT __stdcall Pause(void);

	/// Called when the graph stops
	HRESULT __stdcall Stop(void);

	// IOggMuxProgress Implementation

	/// Returns the time in 100 nanosecond units of the last page that was written.
	virtual LONGLONG __stdcall getProgressTime();

	/// Returns the number of bytes written so far.
	virtual LONGLONG __stdcall getBytesWritten();

	//Helpers
	virtual HRESULT addAnotherPin();
	virtual void NotifyComplete();

	// IMediaSeeking Override to give progress. - This is unreliable !!
	virtual HRESULT __stdcall GetPositions(LONGLONG *pCurrent, LONGLONG *pStop);
	virtual HRESULT __stdcall GetCurrentPosition(LONGLONG *pCurrent);

	// IOggMuxSettings Implementation
	unsigned long __stdcall maxPacketsPerPage();
	bool __stdcall setMaxPacketsPerPage(unsigned long inMaxPacketsPerPage);

	// SpecifyPropertyPages Implementation
	//STDMETHODIMP OggMuxFilter::GetPages(CAUUID* outPropPages);

protected:

	bool SetupOutput();
	bool CloseOutput();

	std::wstring mFileName;
	std::vector<OggMuxInputPin*> mInputPins;

	OggPageInterleaver* mInterleaver;

	CCritSec* mStreamLock;

	std::fstream mOutputFile;
	HRESULT mHR;
};
