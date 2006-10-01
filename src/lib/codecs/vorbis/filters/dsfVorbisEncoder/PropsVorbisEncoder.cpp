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
    return S_OK;
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
    // Did not handle the message.
    return CBasePropertyPage::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}
HRESULT PropsVorbisEncoder::OnApplyChanges(void)
{
	if (mVorbisEncodeSettings == NULL) {
		return E_POINTER;
	}
    //TODO::: Apply the settings.

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