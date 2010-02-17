#include "stdafx.h"
#include "propstheoraencoder.h"

PropsTheoraEncoder::PropsTheoraEncoder(     LPUNKNOWN inUnk
                                        ,   HRESULT* outHR)
	:	CBasePropertyPage(      NAME("Xiph.Org Theora Encoder")
                            ,   inUnk
                            ,   IDD_THEORA_ENCODE_SETTINGS
                            ,   IDS_THEORA_ENC_PROPS_STRING)
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

unsigned long PropsTheoraEncoder::log2(unsigned long inNum) 
{
	unsigned long ret = 0;
	while (inNum != 0) {
		inNum>>=1;
		ret++;
	}
	return ret - 1;
}

unsigned long PropsTheoraEncoder::pow2(unsigned long inNum) 
{
	return 1 << (inNum);
}

HRESULT PropsTheoraEncoder::OnApplyChanges(void)
{
	if (mTheoraEncodeSettings == NULL) {
		return E_POINTER;
	}

    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_QUALITY_MODE, BM_GETCHECK, NOT_USED, NOT_USED)) {
        mTheoraEncodeSettings->setIsUsingQualityMode(true);
        mTheoraEncodeSettings->setQuality(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, NOT_USED, NOT_USED));
        mTheoraEncodeSettings->setTargetBitrate(0);
        mTheoraEncodeSettings->setKeyframeDataBitrate(0);

    } else {
        mTheoraEncodeSettings->setIsUsingQualityMode(true);
        mTheoraEncodeSettings->setQuality(0);
        mTheoraEncodeSettings->setTargetBitrate(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, NOT_USED, NOT_USED) * 1000);
        mTheoraEncodeSettings->setKeyframeDataBitrate(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED) * 1000);

    }

    if (SendDlgItemMessage(m_hwnd,IDC_FIXED_KFI_CHECK, BM_GETCHECK, NOT_USED, NOT_USED)) {
        //Fixed keyframe interval, no auto - keyframing
        mTheoraEncodeSettings->setIsFixedKeyframeInterval(true);
        mTheoraEncodeSettings->setKeyframeFreq(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED)));
        mTheoraEncodeSettings->setKeyframeFreqMin(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_GETPOS, NOT_USED, NOT_USED)));
        mTheoraEncodeSettings->setKeyframeAutoThreshold(SendDlgItemMessage(m_hwnd,IDC_SLIDER_KF_THRESHOLD, TBM_GETPOS, NOT_USED, NOT_USED));

    } else {
        mTheoraEncodeSettings->setIsFixedKeyframeInterval(false);
        mTheoraEncodeSettings->setKeyframeFreq(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED)));
        mTheoraEncodeSettings->setKeyframeFreqMin(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_GETPOS, NOT_USED, NOT_USED)));
        mTheoraEncodeSettings->setKeyframeAutoThreshold(SendDlgItemMessage(m_hwnd,IDC_SLIDER_KF_THRESHOLD, TBM_GETPOS, NOT_USED, NOT_USED));


    }

    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_QUICK_MODE, BM_GETCHECK, NOT_USED, NOT_USED)) {
        mTheoraEncodeSettings->setIsUsingQuickMode(true);
    } else {
        mTheoraEncodeSettings->setIsUsingQuickMode(false);
    }

    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_ALLOW_DROP_FRAMES, BM_GETCHECK, NOT_USED, NOT_USED)) {
        mTheoraEncodeSettings->setAllowDroppedFrames(true);
    } else {
        mTheoraEncodeSettings->setAllowDroppedFrames(false);
    }

    if (SendDlgItemMessage(m_hwnd,IDC_FLIP_VERTICAL_IMAGE, BM_GETCHECK, NOT_USED, NOT_USED)) {
        mTheoraEncodeSettings->setFlipImageVerticaly(true);
    } else {
        mTheoraEncodeSettings->setFlipImageVerticaly(false);
    }

    mTheoraEncodeSettings->setNoiseSensitivity(SendDlgItemMessage(m_hwnd,IDC_LIST_NOISE_SENS, LB_GETCURSEL, NOT_USED, NOT_USED));
    mTheoraEncodeSettings->setSharpness(SendDlgItemMessage(m_hwnd,IDC_LIST_SHARPNESS, LB_GETCURSEL, NOT_USED, NOT_USED));


	SetClean();
    return S_OK;
}

HRESULT PropsTheoraEncoder::OnActivate(void)
{
    
    if (mTheoraEncodeSettings->canModifySettings()) {

        wchar_t* locStrBuff = new wchar_t[16];

	    //SetupBitrateCombo();
	    //SetupKeyframeFreqCombo();
    	
	    //Set up the sliders
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELONG(1, 63));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETTICFREQ, 1, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_QUALITY, TBM_SETPOS, 1, mTheoraEncodeSettings->quality());

	    SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETRANGE, TRUE, MAKELONG(0, 13));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETTICFREQ, 1, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME, TBM_SETPOS, 1, log2(mTheoraEncodeSettings->keyframeFreq()));

	    SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_SETRANGE, TRUE, MAKELONG(0, 13));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_SETTICFREQ, 1, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_SETPOS, 1, log2(mTheoraEncodeSettings->keyframeFreqMin()));

	    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETRANGE, TRUE, MAKELONG(64, 3968));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETTICFREQ, 32, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE, TBM_SETPOS, 1, mTheoraEncodeSettings->targetBitrate() / 1000);

	    SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE_KEYFRAME, TBM_SETRANGE, TRUE, MAKELONG(64, 7936));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE_KEYFRAME, TBM_SETTICFREQ, 32, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_BITRATE_KEYFRAME, TBM_SETPOS, 1, mTheoraEncodeSettings->keyFrameDataBitrate() / 1000);

	    SendDlgItemMessage(m_Dlg, IDC_SLIDER_KF_THRESHOLD, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_KF_THRESHOLD, TBM_SETTICFREQ, 1, 0);
        SendDlgItemMessage(m_Dlg, IDC_SLIDER_KF_THRESHOLD, TBM_SETPOS, 1, mTheoraEncodeSettings->keyframeAutoThreshold());





        //Fill out the labels
	    _itow(mTheoraEncodeSettings->quality(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_QUALITY, WM_SETTEXT, NOT_USED, (LPARAM)locStrBuff);

	    _itow(mTheoraEncodeSettings->keyframeFreq(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_LOG_KEYFRAME, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);

	    _itow(mTheoraEncodeSettings->targetBitrate(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_BITRATE, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);

	    _itow(mTheoraEncodeSettings->keyFrameDataBitrate(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_BITRATE_KEYFRAME, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);


	    _itow(mTheoraEncodeSettings->keyframeFreqMin(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_LOG_KEYFRAME_MIN, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);


	    _itow(mTheoraEncodeSettings->keyframeAutoThreshold(), locStrBuff, 10);
	    SendDlgItemMessage(m_Dlg, IDC_LABEL_KF_THRESHOLD, WM_SETTEXT,NOT_USED,  (LPARAM)locStrBuff);



        //Setup the list boxes
        wstring locListString = L"0 - Sharpest (default)";
        SendDlgItemMessage(m_Dlg, IDC_LIST_SHARPNESS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"1 - Sharper";
        SendDlgItemMessage(m_Dlg, IDC_LIST_SHARPNESS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"2 - Sharp (fastest)";
        SendDlgItemMessage(m_Dlg, IDC_LIST_SHARPNESS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());


        SendDlgItemMessage(m_Dlg, IDC_LIST_SHARPNESS, LB_SETCURSEL, (WPARAM)mTheoraEncodeSettings->sharpness(), NOT_USED);

        locListString = L"0 - Most sensitive";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"1 - Theora default";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"2 - VP3 default";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"3 - ";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"4 - ";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"5 - ";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"6 - Least sensitive";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        locListString = L"? - Fallback (2.5)";
        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_ADDSTRING, NOT_USED,  (LPARAM)locListString.c_str());

        SendDlgItemMessage(m_Dlg, IDC_LIST_NOISE_SENS, LB_SETCURSEL, (WPARAM)mTheoraEncodeSettings->noiseSensitivity(), NOT_USED);

        //Set the checkboxes
        SendDlgItemMessage(m_Dlg, IDC_CHECK_QUALITY_MODE, BM_SETCHECK, (WPARAM)(mTheoraEncodeSettings->isUsingQualityMode() ? BST_CHECKED : BST_UNCHECKED), NOT_USED);
        SendDlgItemMessage(m_Dlg, IDC_CHECK_ALLOW_DROP_FRAMES, BM_SETCHECK, (WPARAM)(mTheoraEncodeSettings->allowDroppedFrames() ? BST_CHECKED : BST_UNCHECKED), NOT_USED);
        SendDlgItemMessage(m_Dlg, IDC_CHECK_QUICK_MODE, BM_SETCHECK, (WPARAM)(mTheoraEncodeSettings->isUsingQuickMode() ? BST_CHECKED : BST_UNCHECKED), NOT_USED);
        SendDlgItemMessage(m_Dlg, IDC_FIXED_KFI_CHECK, BM_SETCHECK, (WPARAM)(mTheoraEncodeSettings->isFixedKeyframeInterval() ? BST_CHECKED : BST_UNCHECKED), NOT_USED);
        SendDlgItemMessage(m_Dlg, IDC_FLIP_VERTICAL_IMAGE, BM_SETCHECK, (WPARAM)(mTheoraEncodeSettings->getFlipImageVerticaly() ? BST_CHECKED : BST_UNCHECKED), NOT_USED);

        //TODO::: Disable
        setDialogQualityModeView(mTheoraEncodeSettings->isUsingQualityMode());
        setFixedKFIModeView(mTheoraEncodeSettings->isFixedKeyframeInterval());
       

	    delete[] locStrBuff;
        return S_OK;
    } else {

        //TODO::: DISABLE EVERYTHING!
        return S_OK;
    }
}

HRESULT PropsTheoraEncoder::OnConnect(IUnknown *pUnk)
{
    
	if (mTheoraEncodeSettings != NULL) {
		mTheoraEncodeSettings->Release();
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
		mTheoraEncodeSettings->Release();
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

void PropsTheoraEncoder::setDialogQualityModeView(bool inUsingQualityMode)
{
    if (inUsingQualityMode) {
        //Quality Mode
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE_KEYFRAME), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_BITRATE_KEYFRAME), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_KEYFRAME), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_QUALITY), TRUE);

    } else {
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE_KEYFRAME), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY), FALSE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_BITRATE_KEYFRAME), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_BITRATE_KEYFRAME), TRUE);

        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_Q_VALUE), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_QUALITY), FALSE);

    }
}


void PropsTheoraEncoder::setFixedKFIModeView(bool inIsFixedKFIMode)
{

    if (inIsFixedKFIMode) {
        //Fixed keyframe interval... shouldn't really ever use this.

        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KF_MAX), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_LOG_KEYFRAME), TRUE);


        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME_MIN), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_LOG_KEYFRAME_MIN), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KFI_MIN), FALSE);


        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_KF_THRESHOLD), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_KF_THRESHOLD), FALSE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KFI_THRESHOLD), FALSE);



    } else {
        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KF_MAX), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_LOG_KEYFRAME), TRUE);


        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME_MIN), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_LOG_KEYFRAME_MIN), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KFI_MIN), TRUE);


        EnableWindow(GetDlgItem(m_hwnd, IDC_SLIDER_KF_THRESHOLD), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_LABEL_KF_THRESHOLD), TRUE);
        EnableWindow(GetDlgItem(m_hwnd, IDC_STATIC_KFI_THRESHOLD), TRUE);

    }
}
INT_PTR PropsTheoraEncoder::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wchar_t locBuff[16];
    
    switch (uMsg)    {
		case WM_COMMAND:
            //TODO::: Need to check the high wparam ??
            if (HIWORD(wParam) == BN_CLICKED) {
                if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_FIXED_KFI_CHECK)) {
                    SetDirty();
                    setFixedKFIModeView(SendDlgItemMessage(m_hwnd,IDC_FIXED_KFI_CHECK, BM_GETCHECK, NOT_USED, NOT_USED));
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_ALLOW_DROP_FRAMES)) {
                    SetDirty();
                    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_ALLOW_DROP_FRAMES, BM_GETCHECK, NOT_USED, NOT_USED)) {
                        //Do we even need to catch this one?
                    }
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_QUICK_MODE)) {
                    SetDirty();
                    if (SendDlgItemMessage(m_hwnd,IDC_CHECK_QUICK_MODE, BM_GETCHECK, NOT_USED, NOT_USED)) {
                        //Do we even need to catch this one?
                    }
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_CHECK_QUALITY_MODE)) {
                    SetDirty();
                    setDialogQualityModeView(SendDlgItemMessage(m_hwnd,IDC_CHECK_QUALITY_MODE, BM_GETCHECK, NOT_USED, NOT_USED));
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_FLIP_VERTICAL_IMAGE)) {
                    SetDirty();
                    if (SendDlgItemMessage(m_hwnd,IDC_FLIP_VERTICAL_IMAGE, BM_GETCHECK, NOT_USED, NOT_USED)) {
                        //Do we even need to catch this one?
                    }
                }
            } else if (HIWORD(wParam) == LBN_SELCHANGE) {
                if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_LIST_SHARPNESS)) {
                    SetDirty();
                } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_LIST_SHARPNESS)) {
                    SetDirty();
                }
            }
            break;
		
		case WM_HSCROLL:
			if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_QUALITY)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_QUALITY, TBM_GETPOS, NOT_USED, NOT_USED), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_QUALITY, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE, TBM_GETPOS, NOT_USED, NOT_USED) * 1000, locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_BITRATE, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_BITRATE_KEYFRAME)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_BITRATE_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED) * 1000, locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_SLIDER_BITRATE_KEYFRAME, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);


                return (INT_PTR)TRUE;

			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME)) {
				SetDirty();
				_itow(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME, TBM_GETPOS, NOT_USED, NOT_USED)), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_LOG_KEYFRAME, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
			} else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_LOG_KEYFRAME_MIN)) {
				SetDirty();
				_itow(pow2(SendDlgItemMessage(m_hwnd,IDC_SLIDER_LOG_KEYFRAME_MIN, TBM_GETPOS, NOT_USED, NOT_USED)), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_LOG_KEYFRAME_MIN, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
            } else if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_KF_THRESHOLD)) {
				SetDirty();
				_itow(SendDlgItemMessage(m_hwnd,IDC_SLIDER_KF_THRESHOLD, TBM_GETPOS, NOT_USED, NOT_USED), locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_KF_THRESHOLD, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                return (INT_PTR)TRUE;
            }

			break;
        default:
            break;

         
    } // switch

    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}


