#pragma once



class CMMLRawSourcePin;

class CMMLRawSourceFilter
	:	public CBaseFilter
{
public:
	CMMLRawSourceFilter(void);
	virtual ~CMMLRawSourceFilter(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);

	virtual int GetPinCount();
	virtual CBasePin* GetPin(int inPinNo);
protected:
	CMMLRawSourcePin* mCMMLSourcePin;
};
