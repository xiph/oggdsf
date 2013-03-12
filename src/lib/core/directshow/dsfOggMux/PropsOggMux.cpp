#include "Precompiled.h"
#include "PropsOggMux.h"

PropsOggMux::PropsOggMux(LPUNKNOWN inUnk, HRESULT* outHR)
	:	CBasePropertyPage(NAME("Xiph.Org Directshow Filters"), inUnk, IDD_OGG_MUX_SETTINGS, IDS_OGG_MUX_PROPS_STRING)

{
	*outHR = S_OK;
}

PropsOggMux::~PropsOggMux()
{
}

CUnknown* PropsOggMux::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new (std::nothrow) PropsOggMux(inUnk, outHR);
}


HRESULT PropsOggMux::OnApplyChanges(void)
{
	if (!oggMuxSettings_) 
    {
		return E_POINTER;
	}

	oggMuxSettings_->setMaxPacketsPerPage(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MAX_PACKETS_PER_PAGE, TBM_GETPOS, NOT_USED, NOT_USED));
	SetClean();
    return S_OK;
}

HRESULT PropsOggMux::OnActivate(void)
{
    char* locStrBuff = new char[16];
	
	//Set up the sliders
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_PACKETS_PER_PAGE, TBM_SETRANGE, TRUE, MAKELONG(0, 64));
    SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_PACKETS_PER_PAGE, TBM_SETTICFREQ, 1, 0);
	SendDlgItemMessage(m_Dlg, IDC_SLIDER_MAX_PACKETS_PER_PAGE, TBM_SETPOS, 1, oggMuxSettings_->maxPacketsPerPage());

	itoa(oggMuxSettings_->maxPacketsPerPage(), locStrBuff, 10);
	SendDlgItemMessage(m_Dlg, IDC_LABEL_MAX_PACKETS_PER_PAGE, WM_SETTEXT, NOT_USED, (LPARAM)locStrBuff);

	delete[] locStrBuff;
    return S_OK;
}

HRESULT PropsOggMux::OnConnect(IUnknown *pUnk)
{
	oggMuxSettings_ = 0;

    HRESULT hr = S_OK;
    try
    {
        CHECK_HR( pUnk->QueryInterface(IID_IOggMuxSettings, (void**)(&oggMuxSettings_)) );
    }
    catch (const CAtlException& ex)
    {
        hr = ex;
    }

    return hr;
}

HRESULT PropsOggMux::OnDisconnect(void)
{
	oggMuxSettings_ = 0;

    return S_OK;
}

void PropsOggMux::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

void PropsOggMux::SetClean()
{
    m_bDirty = FALSE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_CLEAN);
    }
}
INT_PTR PropsOggMux::OnReceiveMessage(HWND hwnd,  UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char locBuff[16];
    switch (uMsg)    
    {
		case WM_COMMAND:		
		case WM_HSCROLL:
			if (HWND(lParam) == GetDlgItem(m_hwnd, IDC_SLIDER_MAX_PACKETS_PER_PAGE)) 
            {
				SetDirty();
				itoa(SendDlgItemMessage(m_hwnd,IDC_SLIDER_MAX_PACKETS_PER_PAGE, TBM_GETPOS, NOT_USED, NOT_USED), (char*)&locBuff, 10);
				SendDlgItemMessage(m_hwnd, IDC_LABEL_MAX_PACKETS_PER_PAGE, WM_SETTEXT, NOT_USED, (LPARAM)&locBuff);
                
                return (INT_PTR)TRUE;
			}

			break;
	}

    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}
