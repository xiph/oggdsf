#pragma once
#include "resource.h"

#include <commctrl.h>
class PropsTheoraEncoder
	:	public CBasePropertyPage
{
public:
	PropsTheoraEncoder(LPUNKNOWN inUnk, HRESULT* outHR);
	virtual ~PropsTheoraEncoder(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR);

	//CBasePropertyPage Virtual Overrides
	HRESULT OnActivate(void);
	HRESULT OnConnect(IUnknown *pUnk);
	HRESULT OnDisconnect(void);
	INT_PTR OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	ITheoraEncodeSettings* mTheoraEncodeSettings;
};
