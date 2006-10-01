#include "StdAfx.h"
#include "PropsVorbisEncoder.h"

PropsVorbisEncoder::PropsVorbisEncoder(     LPUNKNOWN inUnk
                                        ,   HRESULT* outHR)
	:	CBasePropertyPage(      NAME("illiminable Vorbis Encoder")
                            ,   inUnk
                            ,   IDD_VORBIS_ENCODE_SETTINGS
                            ,   IDS_VORBIS_ENC_PROPS_STRING)
{
}

PropsVorbisEncoder::~PropsVorbisEncoder(void)
{
}

CUnknown* WINAPI PropsVorbisEncoder::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsVorbisEncoder(inUnk, outHR);
}

HRESULT PropsVorbisEncoder::OnActivate(void)
{
    //TODO::: Setup everything
    wchar_t* locStrBuff = new wchar_t[16];

    VorbisEncodeSettings locSettings = mVorbisEncodeSettings->getEncoderSettings();

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELONG(0, 99));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, locSettings.mQuality);

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETTICFREQ, 16, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETPOS, 1, locSettings.mBitrate);

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETTICFREQ, 16, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETPOS, 1, locSettings.mMinBitrate);

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETTICFREQ, 16, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETPOS, 1, locSettings.mMaxBitrate);

   //Fill out the labels
    _itow(locSettings.mQuality, locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_LABEL_Q_VALUE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    _itow(locSettings.mBitrate, locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_LABEL_TARGET_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    _itow(locSettings.mMinBitrate, locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_LABEL_MIN_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    _itow(locSettings.mMaxBitrate, locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_LABEL_MAX_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    SendDlgItemMessage(m_Dlg, IDC_CHECK_Q_MODE, BM_SETCHECK, (WPARAM)(locSettings.mIsQualitySet ? BST_CHECKED : BST_UNCHECKED), 0);
    setUsingQualityMode(locSettings.mIsQualitySet);

    delete[] locStrBuff;





    return S_OK;
}

void PropsVorbisEncoder::setUsingQualityMode(bool inIsUsingQualityMode)
{
    if (inIsUsingQualityMode) {
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_Q_VALUE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_TARGET_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_TARGET_BITRATE), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MIN_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MIN_BITRATE), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MAX_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MAX_BITRATE), FALSE);
    } else {
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_Q_VALUE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_TARGET_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_TARGET_BITRATE), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MIN_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MIN_BITRATE), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MAX_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MAX_BITRATE), TRUE);
    }
}
HRESULT PropsVorbisEncoder::OnConnect(IUnknown *pUnk)
{
	if (mVorbisEncodeSettings != NULL) {
		//mTheoraEncodeSettings->Release();
		mVorbisEncodeSettings = NULL;
	}

    HRESULT locHR;
    // Query pUnk for the filter's custom interface.
    locHR = pUnk->QueryInterface(IID_IVorbisEncodeSettings, (void**)(&mVorbisEncodeSettings));
    return locHR;
}
HRESULT PropsVorbisEncoder::OnDisconnect(void)
{
	if (mVorbisEncodeSettings != NULL) {
		//mTheoraEncodeSettings->Release();
		mVorbisEncodeSettings = NULL;
	}
    return S_OK;
}
INT_PTR PropsVorbisEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t locBuff[16];
    
    switch (uMsg)    {
		case WM_COMMAND:
            //TODO::: Need to check the high wparam ??
            if (HIWORD(wParam) == BN_CLICKED) {
                if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_Q_MODE)) {
                    SetDirty();
                    setUsingQualityMode(SendDlgItemMessage(m_hwnd,IDC_CHECK_Q_MODE, BM_GETCHECK, NOT_USED, NOT_USED));
                }
            }
            break;
		case WM_HSCROLL:
			if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_Q_VALUE, WM_SETTEXT, 0, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;
            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_TARGET_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_TARGET_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MIN_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_MIN_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MAX_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_MAX_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

            }
            break;
        default:
            break;
    }

    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}
HRESULT PropsVorbisEncoder::OnApplyChanges(void)
{
	if (mVorbisEncodeSettings == NULL) {
		return E_POINTER;
	}
    //TODO::: Apply the settings.

    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_Q_MODE, BM_GETCHECK, 0, 0)) {
        mVorbisEncodeSettings->setQuality(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0));

    } else {
        mVorbisEncodeSettings->setManaged(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_TARGET_BITRATE, TBM_GETPOS, 0, 0) * 1000
                                            ,   SendDlgItemMessage(m_hwnd,IDC_SLIDER_MIN_BITRATE, TBM_GETPOS, 0, 0) * 1000
                                            ,   SendDlgItemMessage(m_hwnd,IDC_SLIDER_MAX_BITRATE, TBM_GETPOS, 0, 0) * 1000);

    }

	SetClean();
    return S_OK;
}

void PropsVorbisEncoder::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

void PropsVorbisEncoder::SetClean()
{
    m_bDirty = FALSE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_CLEAN);
    }
}