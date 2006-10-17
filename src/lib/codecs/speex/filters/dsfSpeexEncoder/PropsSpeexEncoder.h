#pragma once

#include "resource.h"

#include <commctrl.h>

class PropsSpeexEncoder
    :	public CBasePropertyPage
{
public:
    PropsSpeexEncoder(LPUNKNOWN inUnk, HRESULT* outHR);
    virtual ~PropsSpeexEncoder(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR);

	//CBasePropertyPage Virtual Overrides
	HRESULT OnActivate(void);
	HRESULT OnConnect(IUnknown *pUnk);
	HRESULT OnDisconnect(void);
	INT_PTR OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT OnApplyChanges(void);

protected:
	void SetDirty();
	void SetClean();

    void updateActiveControlsForBitrateMode(int inMode);

 
    ISpeexEncodeSettings* mSpeexEncodeSettings;
};