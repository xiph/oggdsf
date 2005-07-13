#pragma once

#include "IOggMuxSettings.h"
#include "resource.h"

#include <commctrl.h>
//Debug
//#include <fstream>
using namespace std;
//


class PropsOggMux
	:	public CBasePropertyPage
{
public:
	static const UINT NOT_USED = 0;
	PropsOggMux(LPUNKNOWN inUnk, HRESULT* outHR);
	virtual ~PropsOggMux(void);

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
	//
	IOggMuxSettings* mOggMuxSettings;

	//Debug

	//fstream debugLog;
	//
};
