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

#include <Dshow.h>
#include <Initguid.h>
#include <Qnetwork.h>
#include "oggdllstuff.h"
#include "OggDemuxSourcePin.h"
#include "OggStreamMapper.h"
#include <libOOOggSeek/AutoOggSeekTable.h>
#include <libOOOgg/OggDataBuffer.h>
#include "IFilterDataSource.h"
#include "DataSourceFactory.h"
#include "PropsAbout.h"
#include <fstream>
using namespace std;


class OggStreamMapper;

class OGG_DEMUX_API OggDemuxSourceFilter 
	:	public CBaseFilter,
		public CAMThread,
		public IFileSourceFilter,
		public IOggCallback
	,	public BasicSeekPassThrough
	,	public ISpecifyPropertyPages
	,	public IAMFilterMiscFlags
	,	public IAMMediaContent
		
{
public:
	friend class OggStream;
	static const unsigned long RAW_BUFFER_SIZE = 24;
	//Com Stuff
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	enum eThreadCommands {
		THREAD_EXIT = 0,
		THREAD_PAUSE = 1,
		THREAD_RUN = 2
	};
	//Constructors
	OggDemuxSourceFilter();
	OggDemuxSourceFilter(REFCLSID inFilterGUID);
	virtual ~OggDemuxSourceFilter(void);

	//IAMFilterMiscFlags Interface
	ULONG STDMETHODCALLTYPE GetMiscFlags(void);
	//

	//IFileSource Interface
	virtual STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);
	virtual STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

	//PURE VIRTUALS From CBaseFilter
	virtual int GetPinCount();
	virtual CBasePin* GetPin(int inPinNo);

	//IOggCallback Interface
	virtual bool acceptOggPage(OggPage* inOggPage);

	//ISpecifyPropertyPages
	virtual STDMETHODIMP GetPages(CAUUID* outPropPages);

	//Streaming MEthods
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

	//IMediaSeeking
	virtual STDMETHODIMP GetDuration(LONGLONG* outDuration);
	virtual STDMETHODIMP GetCapabilities(DWORD* inCapabilities);
	 
	virtual STDMETHODIMP CheckCapabilities(DWORD *pCapabilities);
	virtual STDMETHODIMP IsFormatSupported(const GUID *pFormat);
	virtual STDMETHODIMP QueryPreferredFormat(GUID *pFormat);
	virtual STDMETHODIMP SetTimeFormat(const GUID *pFormat);
	virtual STDMETHODIMP GetTimeFormat( GUID *pFormat);
	
	virtual STDMETHODIMP GetStopPosition(LONGLONG *pStop);
	virtual STDMETHODIMP GetCurrentPosition(LONGLONG *pCurrent);
	virtual STDMETHODIMP ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat);
	virtual STDMETHODIMP SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags);
	virtual STDMETHODIMP GetPositions(LONGLONG *pCurrent, LONGLONG *pStop);
	virtual STDMETHODIMP GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest);
	virtual STDMETHODIMP SetRate(double dRate);
	virtual STDMETHODIMP GetRate(double *dRate);
	virtual STDMETHODIMP GetPreroll(LONGLONG *pllPreroll);
	virtual STDMETHODIMP IsUsingTimeFormat(const GUID *pFormat);
	
	//IAMMediaContent Interface
	virtual STDMETHODIMP get_AuthorName(BSTR* outAuthorName);
	virtual STDMETHODIMP get_Title(BSTR* outTitle);
	virtual STDMETHODIMP get_Rating(BSTR* outRating);
	virtual STDMETHODIMP get_Description(BSTR* outDescription);
	virtual STDMETHODIMP get_Copyright(BSTR* outCopyright);
	virtual STDMETHODIMP get_BaseURL(BSTR* outBaseURL);
	virtual STDMETHODIMP get_LogoURL(BSTR* outLogoURL);
	virtual STDMETHODIMP get_LogoIconURL(BSTR* outLogoIconURL);
	virtual STDMETHODIMP get_WatermarkURL(BSTR* outWatermarkURL);
	virtual STDMETHODIMP get_MoreInfoURL(BSTR* outMoreInfoURL);
	virtual STDMETHODIMP get_MoreInfoBannerImage(BSTR* outMoreInfoBannerImage);
	virtual STDMETHODIMP get_MoreInfoBannerURL(BSTR* outMoreInfoBannerURL);
	virtual STDMETHODIMP get_MoreInfoText(BSTR* outMoreInfoText);


	//IDispatch Interface (Base interface of IAMMediaContent) - evil evil thing.
	// Trying not to implement it ! I'm sure it was all very funny when they came up with it.
	virtual STDMETHODIMP GetTypeInfoCount(	unsigned int FAR*  pctinfo );
	virtual STDMETHODIMP GetIDsOfNames(		REFIID  riid, 
											OLECHAR FAR* FAR* rgszNames, 
											unsigned int cNames, 
											LCID lcid, 
											DISPID FAR* rgDispId );
	virtual STDMETHODIMP GetTypeInfo(		unsigned int iTInfo, 
											LCID lcid, 
											ITypeInfo FAR* FAR*  ppTInfo );
	virtual STDMETHODIMP Invoke(			DISPID  dispIdMember,
											REFIID  riid,
											LCID  lcid,
											WORD  wFlags,
											DISPPARAMS FAR*  pDispParams,  
											VARIANT FAR*  pVarResult,  
											EXCEPINFO FAR*  pExcepInfo,  
											unsigned int FAR*  puArgErr );


	 CCritSec* theLock();
	//CAMThread
	virtual DWORD ThreadProc(void);

	
	CCritSec* mStreamLock;

	REFERENCE_TIME mSeekTimeBase;  //Don't ask !

protected:
	//Internal helper methods
	void resetStream();
	void DeliverEOS();
	void DeliverBeginFlush();
	void DeliverEndFlush();
	void DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

	virtual HRESULT DataProcessLoop();
	virtual HRESULT SetUpPins();
	//virtual bool AddPin(OggDemuxSourcePin* inPin, unsigned long inSerialNo);


	CCritSec* mSourceFileLock;
	CCritSec* mDemuxLock;
	AutoOggSeekTable* mSeekTable;
	wstring mFileName;

	OggDataBuffer mOggBuffer;

	bool mJustReset;  //This is pretty dodgy !

	bool mSetIgnorePackets;

	//SOURCE ABSTRACTION::: declaration
	//fstream mSourceFile;
	//
	IFilterDataSource* mDataSource;

	OggStreamMapper* mStreamMapper;

	//DEBUG
	fstream debugLog;

	
};
