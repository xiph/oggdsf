#pragma once

//Local Includes
#include "dsfseeking.h"

//STL Includes
#include <fstream>
using namespace std;

class DSFSEEKING_API BasicSeekable
	//Base classes.
	:	public IMediaSeeking
{
public:
	//Constructors
	BasicSeekable(void);
	BasicSeekable(IMediaSeeking* inDelegate);
	virtual ~BasicSeekable(void);

	//Delegate Control Methods.
	bool SetDelegate(IMediaSeeking* inDelegate);
	bool ReleaseDelegate();
	
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

protected:
	//Member data
	IMediaSeeking* mSeekDelegate;
	HRESULT mHR;
	DWORD mSeekingCap;

	//Debug only.
	fstream seekDebug;
};
