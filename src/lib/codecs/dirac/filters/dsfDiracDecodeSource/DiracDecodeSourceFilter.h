
#pragma once
#include "dsfDiracDecodeSource.h"

#include <string>
using namespace std;


class DiracDecodeSourcePin;
class DiracDecodeSourceFilter
	:	public CBaseFilter
	,	public IFileSourceFilter
	,	public IAMFilterMiscFlags
	,	public CAMThread
{
public:
	enum eThreadCommands {
		THREAD_EXIT = 0,
		THREAD_PAUSE = 1,
		THREAD_RUN = 2
	};
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	DiracDecodeSourceFilter(void);
	virtual ~DiracDecodeSourceFilter(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	//IBaseFilter Pure Virtuals
	virtual int GetPinCount();
	virtual CBasePin* GetPin(int inPinNo);

	//IAMFilterMiscFlags Interface
	ULONG STDMETHODCALLTYPE GetMiscFlags(void);
	//

	//IFileSource Interface
	virtual STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);
	virtual STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

	//Streaming MEthods
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause(void);
	STDMETHODIMP Stop(void);

	//CAMThread
	virtual DWORD ThreadProc(void);
protected:

	HRESULT DataProcessLoop();
	DiracDecodeSourcePin* mDiracSourcePin;
	wstring mFileName;
	wstring mHDRFileName;

	dirac_decoder_t* mDecoder;
};
