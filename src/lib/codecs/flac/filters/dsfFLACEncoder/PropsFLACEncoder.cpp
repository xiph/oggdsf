#include "StdAfx.h"
#include "resource.h"
#include "PropsFLACEncoder.h"
#include "flacencoderdllstuff.h"

PropsFLACEncoder::PropsFLACEncoder(     LPUNKNOWN inUnk
                                        ,   HRESULT* outHR)
	:	CBasePropertyPage(      NAME("illiminable FLAC Encoder")
                            ,   inUnk
                            ,   IDD_FLAC_ENCODE_SETTINGS
                            ,   IDS_FLAC_ENC_PROPS_STRING)
    ,   mFLACEncodeSettings(NULL)
{
}

PropsFLACEncoder::~PropsFLACEncoder(void)
{
}

CUnknown* WINAPI PropsFLACEncoder::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsFLACEncoder(inUnk, outHR);
}

HRESULT PropsFLACEncoder::OnActivate(void)
{
    wstring locListString = L"0 - Fastest";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"1";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"2";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"3";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());


    locListString = L"4";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());


    locListString = L"5 - Default";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"6";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"7";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    locListString = L"8 - Smallest";
    SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_ADDSTRING, 0,  (LPARAM)locListString.c_str());

    long locEncoderLevel = mFLACEncodeSettings->encoderLevel();

    if (locEncoderLevel < 0) {
        //Not using standard level. Disable. (Noti mplemented yet)
    } else {
        SendDlgItemMessage(m_Dlg, IDC_COMBO_ENCODER_LEVEL, CB_SETCURSEL, (WPARAM)locEncoderLevel, 0);
    }
    return S_OK;
    
   // //TODO::: Setup everything
   // wchar_t* locStrBuff = new wchar_t[16];

   // FLACEncodeSettings locSettings = mFLACEncodeSettings->getEncoderSettings();

   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELONG(0, 99));
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, locSettings.mQuality);

   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETTICFREQ, 16, 0);
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_TARGET_BITRATE, TBM_SETPOS, 1, locSettings.mBitrate);

   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETTICFREQ, 16, 0);
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MIN_BITRATE, TBM_SETPOS, 1, locSettings.mMinBitrate);

   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(16, 1024));
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETTICFREQ, 16, 0);
   // SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_BITRATE, TBM_SETPOS, 1, locSettings.mMaxBitrate);

   ////Fill out the labels
   // _itow(locSettings.mQuality, locStrBuff, 10);
   // SendDlgItemMessage(m_Dlg, IDC_LABEL_Q_VALUE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

   // _itow(locSettings.mBitrate, locStrBuff, 10);
   // SendDlgItemMessage(m_Dlg, IDC_LABEL_TARGET_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

   // _itow(locSettings.mMinBitrate, locStrBuff, 10);
   // SendDlgItemMessage(m_Dlg, IDC_LABEL_MIN_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

   // _itow(locSettings.mMaxBitrate, locStrBuff, 10);
   // SendDlgItemMessage(m_Dlg, IDC_LABEL_MAX_BITRATE, WM_SETTEXT, 0, (LPARAM)locStrBuff);

   // SendDlgItemMessage(m_Dlg, IDC_CHECK_Q_MODE, BM_SETCHECK, (WPARAM)(locSettings.mIsQualitySet ? BST_CHECKED : BST_UNCHECKED), 0);
   // setUsingQualityMode(locSettings.mIsQualitySet);

   // delete[] locStrBuff;

   // return S_OK;
}

//void PropsFLACEncoder::setUsingQualityMode(bool inIsUsingQualityMode)
//{
//    if (inIsUsingQualityMode) {
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_Q_VALUE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), TRUE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_TARGET_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_TARGET_BITRATE), FALSE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MIN_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MIN_BITRATE), FALSE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MAX_BITRATE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MAX_BITRATE), FALSE);
//    } else {
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_Q_VALUE), FALSE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), FALSE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_TARGET_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_TARGET_BITRATE), TRUE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MIN_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MIN_BITRATE), TRUE);
//
//        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_MAX_BITRATE), TRUE);
//        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_MAX_BITRATE), TRUE);
//    }
//}
HRESULT PropsFLACEncoder::OnConnect(IUnknown *pUnk)
{
	if (mFLACEncodeSettings != NULL) {
		mFLACEncodeSettings->Release();
		mFLACEncodeSettings = NULL;
	}

    HRESULT locHR;
    // Query pUnk for the filter's custom interface.
    locHR = pUnk->QueryInterface(IID_IFLACEncodeSettings, (void**)(&mFLACEncodeSettings));
    return locHR;
}
HRESULT PropsFLACEncoder::OnDisconnect(void)
{
	if (mFLACEncodeSettings != NULL) {
		mFLACEncodeSettings->Release();
		mFLACEncodeSettings = NULL;
	}
    return S_OK;
}
INT_PTR PropsFLACEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//wchar_t locBuff[16];

    switch (uMsg)    {
	    case WM_COMMAND:

            if (HIWORD(wParam) == CBN_SELCHANGE) {
                if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_COMBO_ENCODER_LEVEL)) {
                    SetDirty();
                    return (INT_PTR)TRUE;
                }
            }
            break;
        default:
            break;
    }

    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);


 //   
 //   switch (uMsg)    {
	//	case WM_COMMAND:
 //           //TODO::: Need to check the high wparam ??
 //           if (HIWORD(wParam) == BN_CLICKED) {
 //               if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_Q_MODE)) {
 //                   SetDirty();
 //                   setUsingQualityMode(SendDlgItemMessage(m_hwnd,IDC_CHECK_Q_MODE, BM_GETCHECK, 0, 0));
 //               }
 //           }
 //           break;
	//	case WM_HSCROLL:
	//		if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY)) {
	//			SetDirty();
	//			_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, 0, 0), locBuff, 10);
	//			SendDlgItemMessage(m_hwnd, IDC_LABEL_Q_VALUE, WM_SETTEXT, 0, (LPARAM)&locBuff);


 //               return (INT_PTR)TRUE;
 //           } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_TARGET_BITRATE)) {
	//			SetDirty();
	//			_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_TARGET_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
	//			SendDlgItemMessage(m_hwnd, IDC_LABEL_TARGET_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


 //               return (INT_PTR)TRUE;

 //           } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_MIN_BITRATE)) {
	//			SetDirty();
	//			_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MIN_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
	//			SendDlgItemMessage(m_hwnd, IDC_LABEL_MIN_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


 //               return (INT_PTR)TRUE;

 //           } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_MAX_BITRATE)) {
	//			SetDirty();
	//			_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MAX_BITRATE, TBM_GETPOS, 0, 0), locBuff, 10);
	//			SendDlgItemMessage(m_hwnd, IDC_LABEL_MAX_BITRATE, WM_SETTEXT, 0, (LPARAM)&locBuff);


 //               return (INT_PTR)TRUE;

 //           }
 //           break;
 //       default:
 //           break;
 //   }

    //// Did not handle the message.
    //return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}
HRESULT PropsFLACEncoder::OnApplyChanges(void)
{
	if (mFLACEncodeSettings == NULL) {
		return E_POINTER;
	}
 //   //TODO::: Apply the settings.

    mFLACEncodeSettings->setEncodingLevel(SendDlgItemMessage(m_hwnd,IDC_COMBO_ENCODER_LEVEL, CB_GETCURSEL, 0, 0));

	SetClean();
    return S_OK;
}

void PropsFLACEncoder::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

void PropsFLACEncoder::SetClean()
{
    m_bDirty = FALSE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_CLEAN);
    }
}