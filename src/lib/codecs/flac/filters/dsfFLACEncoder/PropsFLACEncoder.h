#pragma once

struct IFLACEncodeSettings;

class PropsFLACEncoder:	public CBasePropertyPage
{
public:
    PropsFLACEncoder(LPUNKNOWN inUnk, HRESULT* outHR);
    virtual ~PropsFLACEncoder(void);

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

    IFLACEncodeSettings* mFLACEncodeSettings;
};
