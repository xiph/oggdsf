#pragma once
#include "abstractvideoencoderdllstuff.h"
#include "AbstractVideoEncodeInputPin.h"
#include "AbstractVideoEncodeOutputPin.h"
class ABS_VIDEO_ENC_API AbstractVideoEncodeFilter
	:	public CBaseFilter
{
public:
	friend class AbstractVideoEncodeInputPin;
	friend class AbstractVideoEncodeOutputPin;

	AbstractVideoEncodeFilter(TCHAR* inFilterName, REFCLSID inFilterGUID, unsigned short inVideoFormat );
	virtual ~AbstractVideoEncodeFilter(void);

	static const long NUM_PINS = 2;
	enum eVideoFormat {
		NONE = 0,
		THEORA = 100,
		OTHER_VIDEO = 2000
	};

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	//PURE VIRTUALS
	virtual bool ConstructPins() = 0;
	virtual void DestroyPins();


	//CBaseFilter overrides
	CBasePin* GetPin(int n);
	int GetPinCount(void);

	virtual STDMETHODIMP Stop();
	
	unsigned short mVideoFormat;

protected:

	AbstractVideoEncodeInputPin* mInputPin;
	AbstractVideoEncodeOutputPin* mOutputPin;

};
