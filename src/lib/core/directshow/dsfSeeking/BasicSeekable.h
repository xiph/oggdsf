#pragma once
#include "dsfseeking.h"
#include <fstream>
using namespace std;
class DSFSEEKING_API BasicSeekable
	:	public IMediaSeeking
{
public:
	BasicSeekable(void);
	BasicSeekable(IMediaSeeking* inDelegate);
	~BasicSeekable(void);

	bool SetDelegate(IMediaSeeking* inDelegate);


	//
	//IMediaSeeking Interface
	 virtual STDMETHODIMP GetCapabilities(DWORD *pCapabilities);
	 virtual STDMETHODIMP CheckCapabilities(DWORD *pCapabilities);
	 virtual STDMETHODIMP IsFormatSupported(const GUID *pFormat);
	 virtual STDMETHODIMP QueryPreferredFormat(GUID *pFormat);
	 virtual STDMETHODIMP SetTimeFormat(const GUID *pFormat);
	 virtual STDMETHODIMP GetTimeFormat( GUID *pFormat);
	 virtual STDMETHODIMP GetDuration(LONGLONG *pDuration);
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
	//







	//STDMETHODIMP QueryInterface( REFIID iid, void **ppvObject) {
	//	debugLog <<"Querying interface."<<endl;
	//	if (iid == IID_IMediaSeeking) {
	//		debugLog <<"Querying interface for IMedaSeeking"<<endl;
	//		*ppvObject = (IMediaSeeking*)this;
	//		debugLog<<"Address of this is "<<(int)this<<endl<<"Address of IMediaSeeking is "<<(int)(*ppvObject)<<endl;
	//		return S_OK;
	//	} else {
	//		debugLog <<"Failure !!!"<<endl;
	//		return S_FALSE;
	//	}
	//	
	//}
	//STDMETHODIMP_(ULONG) AddRef() {
	//	debugLog <<"Ref count was "<<mRefCount<<endl;
	//	return ++mRefCount;
	//}
	//STDMETHODIMP_(ULONG) Release() {
	//	debugLog <<"Ref count was "<<mRefCount<<endl;
	//	mRefCount--;
	//	if (mRefCount <=0) {
	//		
	//		delete this;
	//		return mRefCount;
	//	} else {
	//		return mRefCount;
	//	}

	//}

protected:
	IMediaSeeking* mSeekDelegate;

	REFERENCE_TIME mSeekTimeBase;
	HRESULT mHR;

	fstream seekDebug;

	DWORD mSeekingCap;


};
