#pragma once

#include <string>
#include <map>
#include <libCMMLParse/libCMMLParse.h>
#include <libCMMLParse/CMMLParser.h>
#include <libTemporalURI/C_TimeStamp.h>
#include "CMMLRawSourcePin.h"
using namespace std;

class CMMLRawSourcePin;

class CMMLRawSourceFilter
	:	public CBaseFilter
	,	public CAMThread
	,	public IFileSourceFilter
	,	public IAMFilterMiscFlags
{
public:

	friend class CMMLRawSourcePin;

	enum eThreadCommands {
		THREAD_EXIT = 0,
		THREAD_PAUSE = 1,
		THREAD_RUN = 2
	};
	

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	CMMLRawSourceFilter(void);
	virtual ~CMMLRawSourceFilter(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//IBaseFilter Pure Virtuals
	virtual int GetPinCount();
	virtual CBasePin* GetPin(int inPinNo);

	//IAMFilterMiscFlags Interface
	ULONG STDMETHODCALLTYPE GetMiscFlags(void);

	//IFileSource Interface
	virtual STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);
	virtual STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

	//CAMThread
	virtual DWORD ThreadProc(void);


	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

protected:
	virtual HRESULT DataProcessLoop();


	CMMLRawSourcePin* mCMMLSourcePin;
	CMMLParser mCMMLParser;

	C_CMMLDoc* mCMMLDoc;
	wstring mFileName;


	typedef map<wstring, __int64> tTrackMap;

	tTrackMap mTrackMap;

	long mUptoTag;
};
