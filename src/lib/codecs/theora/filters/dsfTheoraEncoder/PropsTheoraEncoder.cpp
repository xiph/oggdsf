#include "StdAfx.h"
#include "propstheoraencoder.h"

PropsTheoraEncoder::PropsTheoraEncoder(LPUNKNOWN inUnk, HRESULT* outHR)
	:	CBasePropertyPage(NAME("illiminable Directshow Filters"), inUnk, IDD_THEORA_ENCODE_SETTINGS, IDS_THEORA_ENC_PROPS_STRING)
	,	mTheoraEncodeSettings(NULL)

{
	*outHR = S_OK;
}

PropsTheoraEncoder::~PropsTheoraEncoder(void)
{
}

CUnknown* PropsTheoraEncoder::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsTheoraEncoder(inUnk, outHR);
}

HRESULT PropsTheoraEncoder::OnActivate(void)
{
    
    char* locStrBuff = new char[16];

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, 0, MAKELONG(0, 63));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, mTheoraEncodeSettings->quality());

    itoa(mTheoraEncodeSettings->targetBitrate(), locStrBuff, 10);

	SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);
	itoa(mTheoraEncodeSettings->keyframeFreq(), locStrBuff, 10);
	SendDlgItemMessage(m_Dlg, IDC_COMBO_KEYFRAME_FREQ, WM_SETTEXT, 0, (LPARAM)locStrBuff);

	delete locStrBuff;
    return S_OK;
}

HRESULT PropsTheoraEncoder::OnConnect(IUnknown *pUnk)
{
    
	if (mTheoraEncodeSettings != NULL) {
		//mTheoraEncodeSettings->Release();
		mTheoraEncodeSettings = NULL;
	}

    HRESULT locHR;
    // Query pUnk for the filter's custom interface.
    locHR = pUnk->QueryInterface(IID_ITheoraEncodeSettings, (void**)(&mTheoraEncodeSettings));
    return locHR;
}

HRESULT PropsTheoraEncoder::OnDisconnect(void)
{
	if (mTheoraEncodeSettings != NULL) {
		//mTheoraEncodeSettings->Release();
		mTheoraEncodeSettings = NULL;
	}
    return S_OK;
}

INT_PTR PropsTheoraEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_SLIDER_QUALITY)
        {
            UINT m_lNewVal = GetDlgItemInt(m_Dlg, IDC_SLIDER_QUALITY, 0, TRUE);
            //SetDirty();
            return (INT_PTR)TRUE;
        }
        break;
    } // switch

    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}


