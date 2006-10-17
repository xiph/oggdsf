#include "StdAfx.h"
#include "PropsSpeexEncoder.h"


PropsSpeexEncoder::PropsSpeexEncoder(     LPUNKNOWN inUnk
                                        ,   HRESULT* outHR)
	:	CBasePropertyPage(      NAME("illiminable Speex Encoder")
                            ,   inUnk
                            ,   IDD_SPEEX_ENCODE_SETTINGS
                            ,   IDS_SPEEX_ENC_PROPS_STRING)
    ,   mSpeexEncodeSettings(NULL)
{
}

PropsSpeexEncoder::~PropsSpeexEncoder(void)
{
}

CUnknown* WINAPI PropsSpeexEncoder::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsSpeexEncoder(inUnk, outHR);
}

void PropsSpeexEncoder::updateActiveControlsForBitrateMode(int inMode)
{

    //Order is dependant on the order added to the combo control
    switch (inMode) {
        case 0:
            //VBR Quality
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_TEXT), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY_TEXT), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), TRUE);
            SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);




            break;
        case 1:
            //VBR Bitrate
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_TEXT), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY_TEXT), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), TRUE);
            SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
            break;

        case 2:
            //ABR Bitrate
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_TEXT), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY_TEXT), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);

            SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
            EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), FALSE);
            break;
        case 3:
            //CBR Quality
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), TRUE );
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_TEXT), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY_TEXT), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);

            SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
            EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), FALSE);

            break;
        case 4:
            //CBR Bitrate
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_TEXT), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY_TEXT), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);

            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), TRUE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_QUALITY), FALSE);
            EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);

            SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
            EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), FALSE);
            break;
        default:
            break;


    };

}
HRESULT PropsSpeexEncoder::OnActivate(void)
{

    wstring locListString = L"Automatic";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODE_MODE, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"Narrowband";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODE_MODE, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"Wideband";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODE_MODE, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"Ultra-Wideband";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODE_MODE, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"VBR Quality";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"VBR Bitrate";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"ABR Bitrate";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"CBR Quality (default)";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"CBR Bitrate";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());




    SpeexEncodeSettings locSettings = mSpeexEncodeSettings->getEncoderSettings();

    wchar_t* locStrBuff = new wchar_t[16];

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELONG(0, 10));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, locSettings.quality());
    _itow(locSettings.quality(), locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_STATIC_QUALITY_TEXT, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(2, 96));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETTICFREQ, 2, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETPOS, 1, locSettings.targetBitrate());
    _itow(locSettings.targetBitrate(), locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_STATIC_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    SendDlgItemMessage(m_Dlg, IDC_SLIDER_PEAK_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(2, 96));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_PEAK_BITRATE, TBM_SETTICFREQ, 2, 0);
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_PEAK_BITRATE, TBM_SETPOS, 1, locSettings.maxVBRBitrate());
    _itow(locSettings.maxVBRBitrate(), locStrBuff, 10);
    SendDlgItemMessage(m_Dlg, IDC_STATIC_PEAK_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)locStrBuff);

    SendDlgItemMessage(m_Dlg, IDC_CHECK_USE_PEAK, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
    EnableWindow(GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK), FALSE);

    SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_SETCURSEL, 3, 0);
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODE_MODE, CB_SETCURSEL, 0, 0);



    return S_OK;


}


HRESULT PropsSpeexEncoder::OnConnect(IUnknown *pUnk)
{
	if (mSpeexEncodeSettings != NULL) {
		mSpeexEncodeSettings->Release();
		mSpeexEncodeSettings = NULL;
	}

    HRESULT locHR;
    // Query pUnk for the filter's custom interface.
    locHR = pUnk->QueryInterface(IID_ISpeexEncodeSettings, (void**)(&mSpeexEncodeSettings));
    return locHR;
}
HRESULT PropsSpeexEncoder::OnDisconnect(void)
{
	if (mSpeexEncodeSettings != NULL) {
		mSpeexEncodeSettings->Release();
		mSpeexEncodeSettings = NULL;
	}
    return S_OK;
}
INT_PTR PropsSpeexEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	wchar_t locBuff[16];
    
   
    switch (uMsg)    {
        case WM_COMMAND:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_COMBO_BITRATE_CONTROL)) {
                    SetDirty();
                    updateActiveControlsForBitrateMode(SendDlgItemMessage(m_hwnd,IDC_COMBO_BITRATE_CONTROL, CB_GETCURSEL, 0, 0));
                    return (INT_PTR)TRUE;
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_COMBO_ENCODE_MODE)) {
                    SetDirty();
                    //updateActiveControlsForBitrateMode(SendDlgItemMessage(m_hwnd,IDC_COMBO_BITRATE_CONTROL, CB_GETCURSEL, 0, 0));
                    return (INT_PTR)TRUE;
                }
            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_USE_PEAK)) {
                //Only allowed this in VBR modes
                int locBitrateMode = SendDlgItemMessage(m_Dlg, IDC_COMBO_BITRATE_CONTROL, CB_GETCURSEL, 0, 0);
                if ((locBitrateMode == 0) || (locBitrateMode == 1)) {
                    SetDirty();
                    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_USE_PEAK, BM_GETCHECK, 0, 0)) {
                        //USE Peak just checked
                        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), TRUE);
                        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), TRUE);
                        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), TRUE);
                    } else {
                        //Use peak unchecked
                        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE), FALSE);
                        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE), FALSE);
                        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT), FALSE);
                    }
                }
            }
            break;
        case WM_HSCROLL:
	  		if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY)) {
	 			SetDirty();
	 			_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0), locBuff, 10);
                SendDlgItemMessage(m_hwnd, IDC_STATIC_QUALITY_TEXT, WM_SETTEXT, 0, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE)) {
	 			SetDirty();
                int locBitrate = SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, 0, 0);
	 			_itow(locBitrate, locBuff, 10);
                SendDlgItemMessage(m_hwnd, IDC_STATIC_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)&locBuff);

                int locPeakBitrate = SendDlgItemMessage(m_hwnd,IDC_SLIDER_PEAK_BITRATE, TBM_GETPOS, 0, 0);
                if (locBitrate > locPeakBitrate) {
                    _itow(locBitrate, locBuff, 10);
                    SendDlgItemMessage(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)&locBuff);
                    SendDlgItemMessage(m_Dlg, IDC_SLIDER_PEAK_BITRATE, TBM_SETPOS, 1, locBitrate);
                }

                return (INT_PTR)TRUE;
            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_PEAK_BITRATE)) {
	 			SetDirty();
                int locPeakBitrate = SendDlgItemMessage(m_hwnd,IDC_SLIDER_PEAK_BITRATE, TBM_GETPOS, 0, 0);
	 			_itow(locPeakBitrate, locBuff, 10);

                int locBitrate = SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, 0, 0);

                if (locPeakBitrate < locBitrate) {
                    _itow(locPeakBitrate, locBuff, 10);
                    SendDlgItemMessage(m_hwnd, IDC_STATIC_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)&locBuff);    
                    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETPOS, 1, locPeakBitrate);
                }
                SendDlgItemMessage(m_hwnd, IDC_STATIC_PEAK_BITRATE_TEXT, WM_SETTEXT, 0, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
            }
            break;
        default:
            break;
    }



    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);

}
HRESULT PropsSpeexEncoder::OnApplyChanges(void)
{
	if (mSpeexEncodeSettings == NULL) {
		return E_POINTER;
	}
    //TODO::: Apply the settings.

    int locEncodingMode = SendDlgItemMessage(m_hwnd,IDC_COMBO_ENCODE_MODE, CB_GETCURSEL, 0, 0);
    mSpeexEncodeSettings->setMode((SpeexEncodeSettings::eSpeexEncodeMode)locEncodingMode);


    int locBitrateMode = SendDlgItemMessage(m_hwnd,IDC_COMBO_BITRATE_CONTROL, CB_GETCURSEL, 0, 0);

    switch (locBitrateMode) {
        case 0:
            //VBR Quality
            {
            long locMaxBitrate = (SendDlgItemMessage(m_hwnd,IDC_CHECK_USE_PEAK, BM_GETCHECK, 0, 0))     ?   SendDlgItemMessage(m_hwnd,IDC_SLIDER_PEAK_BITRATE, TBM_GETPOS, 0, 0)
                                                                                                        :   -1;

            mSpeexEncodeSettings->setupVBRQualityMode(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0)
                                                       ,   locMaxBitrate);
            }
            break;
        case 1:
            //VBR Bitrate
            {
            long locMaxBitrate = (SendDlgItemMessage(m_hwnd,IDC_CHECK_USE_PEAK, BM_GETCHECK, 0, 0))     ?   SendDlgItemMessage(m_hwnd,IDC_SLIDER_PEAK_BITRATE, TBM_GETPOS, 0, 0)
                                                                                                        :   -1;
            mSpeexEncodeSettings->setupVBRQualityMode(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, 0, 0)
                                                       ,   locMaxBitrate);
            }
            break;

        case 2:
            //ABR
            mSpeexEncodeSettings->setupABR(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, 0, 0));
                                                        
            break;
        case 3:
            mSpeexEncodeSettings->setupCBRBitrateMode(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, 0, 0));
            break;
        case 4:
            mSpeexEncodeSettings->setupCBRQualityMode(      SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0));
            break;
        default:
            break;

    }

	SetClean();
    return S_OK;
}

void PropsSpeexEncoder::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

void PropsSpeexEncoder::SetClean()
{
    m_bDirty = FALSE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_CLEAN);
    }
}