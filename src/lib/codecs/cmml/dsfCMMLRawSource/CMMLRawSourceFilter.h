#pragma once

#include <string>
using namespace std;

class CMMLRawSourcePin;

class CMMLRawSourceFilter
	:	public CBaseFilter
	,	public IFileSourceFilter
	,	public IAMFilterMiscFlags
{
public:
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
	//

	//IFileSource Interface
	virtual STDMETHODIMP GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType);
	virtual STDMETHODIMP Load(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType);

protected:
	CMMLRawSourcePin* mCMMLSourcePin;
	wstring mFileName;
};
