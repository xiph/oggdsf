#pragma once
#include "resource.h"

#include <commctrl.h>
//Debug
//#include <fstream>
using namespace std;
//
class PropsTheoraEncoder
	:	public CBasePropertyPage
{
public:
	static const UINT NOT_USED = 0;
	PropsTheoraEncoder(LPUNKNOWN inUnk, HRESULT* outHR);
	virtual ~PropsTheoraEncoder(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR);

	//CBasePropertyPage Virtual Overrides
	HRESULT OnActivate(void);
	HRESULT OnConnect(IUnknown *pUnk);
	HRESULT OnDisconnect(void);
	INT_PTR OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT OnApplyChanges(void);

	static unsigned long log2(unsigned long inNum);
	static unsigned long pow2(unsigned long inNum);

protected:

	//
	//void SetupBitrateCombo();
	//void SetupKeyframeFreqCombo();
	//LRESULT addNumberToCombo(int inComboID, int inNum);

    void setDialogQualityModeView(bool inUsingQualityMode);
    void setFixedKFIModeView(bool inIsFixedKFIMode);
	
	void SetDirty();
	void SetClean();
	//
	ITheoraEncodeSettings* mTheoraEncodeSettings;

	//Debug

	//fstream debugLog;
	//
};
