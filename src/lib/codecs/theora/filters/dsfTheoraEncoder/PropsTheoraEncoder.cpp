#include "StdAfx.h"
#include "propstheoraencoder.h"

PropsTheoraEncoder::PropsTheoraEncoder(LPUNKNOWN inUnk, HRESULT* outHR)
	:	CBasePropertyPage(NAME("illiminable Directshow Filters"), inUnk, IDD_THEORA_ENCODE_SETTINGS, IDS_THEORA_ENC_PROPS_STRING)
	,	mTheoraEncodeSettings(NULL)

{
	//debugLog.open("G:\\logs\\TheoProps.log", ios_base::out);
	*outHR = S_OK;
}

PropsTheoraEncoder::~PropsTheoraEncoder(void)
{
	//debugLog.close();
}

CUnknown* PropsTheoraEncoder::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsTheoraEncoder(inUnk, outHR);
}

//LRESULT PropsTheoraEncoder::addNumberToCombo(int inComboID, int inNum) {
//	char locStrBuff[16];
//	itoa(inNum, (char*)&locStrBuff, 10);
//	return SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE, CB_ADDSTRING, NOT_USED, (LPARAM)&locStrBuff);
//
//}
//
//void PropsTheoraEncoder::SetupBitrateCombo() {
//	addNumberToCombo(IDC_COMBO_BITRATE, 64000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 96000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 128000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 192000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 256000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 384000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 512000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 768000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 1024000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 1536000);
//	addNumberToCombo(IDC_COMBO_BITRATE, 2000000);
//
//}
//
//void PropsTheoraEncoder::SetupKeyframeFreqCombo() {
//	
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 1);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 2);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 3);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 4);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 5);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 6);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 7);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 8);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 9);
//	addNumberToCombo(IDC_COMBO_LOG_KEYFRAME_FREQ, 10);
//
//}

unsigned long PropsTheoraEncoder::log2(unsigned long inNum) {
	unsigned long ret = 0;
	while (inNum != 0) {
		inNum>>=1;
		ret++;
	}
	return ret - 1;
}

unsigned long PropsTheoraEncoder::pow2(unsigned long inNum) {
	return 1 << (inNum);
}

HRESULT PropsTheoraEncoder::OnApplyChanges(void)
{
	if (mTheoraEncodeSettings == NULL) {
		return E_POINTER;
	}

	mTheoraEncodeSettings->setQuality(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, NOT_USED, NOT_USED));
	mTheoraEncodeSettings->setKeyframeFreq(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED)));
	mTheoraEncodeSettings->setTargetBitrate(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, NOT_USED, NOT_USED) * 1000);
	SetClean();
    return S_OK;
}

HRESULT PropsTheoraEncoder::OnActivate(void)
{
    
    char* locStrBuff = new char[16];

	//SetupBitrateCombo();
	//SetupKeyframeFreqCombo();
	
	//Set up the sliders
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELONG(0, 63));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, mTheoraEncodeSettings->quality());

	SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETRANGE, TRUE, MAKELONG(0, 13));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETTICFREQ, 1, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETPOS, 1, log2(mTheoraEncodeSettings->keyframeFreq()));

	SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(64, 1984));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETTICFREQ, 32, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETPOS, 1, mTheoraEncodeSettings->targetBitrate() / 1000);


	itoa(mTheoraEncodeSettings->quality(), locStrBuff, 10);
	SendDlgItemMessage(m_Dlg, IDC_LABEL_QUALITY, WM_SETTEXT, NOT_USED, (LPARAM)locStrBuff);

	itoa(mTheoraEncodeSettings->keyframeFreq(), locStrBuff, 10);
	SendDlgItemMessage(m_Dlg, IDC_LABEL_LOG_KEYFRAME, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);

	itoa(mTheoraEncodeSettings->targetBitrate(), locStrBuff, 10);
	SendDlgItemMessage(m_Dlg, IDC_LABEL_BITRATE, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);

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
void PropsTheoraEncoder::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

void PropsTheoraEncoder::SetClean()
{
    m_bDirty = FALSE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_CLEAN);
    }
}
INT_PTR PropsTheoraEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char locBuff[16];
    switch (uMsg)    {
		case WM_COMMAND:
		
		case WM_HSCROLL:
			if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY)) {
				SetDirty();
				itoa(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, NOT_USED, NOT_USED), (char*)&locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_QUALITY, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                 return (INT_PTR)TRUE;

			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE)) {
				SetDirty();
				itoa(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, NOT_USED, NOT_USED) * 1000, (char*)&locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_BITRATE, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;

			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME)) {
				SetDirty();
				itoa(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED)), (char*)&locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_LOG_KEYFRAME, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
			}

			break;
    } // switch

    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}


