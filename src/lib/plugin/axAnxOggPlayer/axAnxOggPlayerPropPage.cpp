// axAnxOggPlayerPropPage.cpp : Implementation of the CaxAnxOggPlayerPropPage property page class.

#include "stdafx.h"
#include "axAnxOggPlayer.h"
#include "axAnxOggPlayerPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CaxAnxOggPlayerPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CaxAnxOggPlayerPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CaxAnxOggPlayerPropPage, "AXANXOGGPLAYER.axAnxOggPlayerPropPage.1",
	0x82de93ec, 0x2644, 0x4b7f, 0xa9, 0xde, 0x37, 0x85, 0x83, 0x46, 0x36, 0x3e)



// CaxAnxOggPlayerPropPage::CaxAnxOggPlayerPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CaxAnxOggPlayerPropPage

BOOL CaxAnxOggPlayerPropPage::CaxAnxOggPlayerPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_AXANXOGGPLAYER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CaxAnxOggPlayerPropPage::CaxAnxOggPlayerPropPage - Constructor

CaxAnxOggPlayerPropPage::CaxAnxOggPlayerPropPage() :
	COlePropertyPage(IDD, IDS_AXANXOGGPLAYER_PPG_CAPTION)
{
}



// CaxAnxOggPlayerPropPage::DoDataExchange - Moves data between page and properties

void CaxAnxOggPlayerPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CaxAnxOggPlayerPropPage message handlers
