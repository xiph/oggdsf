// axAnxOggPlayerCtrl.cpp : Implementation of the CaxAnxOggPlayerCtrl ActiveX Control class.

#include "stdafx.h"
#include "axAnxOggPlayer.h"
#include "axAnxOggPlayerCtrl.h"
#include "axAnxOggPlayerPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CaxAnxOggPlayerCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CaxAnxOggPlayerCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CaxAnxOggPlayerCtrl, COleControl)
	DISP_FUNCTION_ID(CaxAnxOggPlayerCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CaxAnxOggPlayerCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CaxAnxOggPlayerCtrl, 1)
	PROPPAGEID(CaxAnxOggPlayerPropPage::guid)
END_PROPPAGEIDS(CaxAnxOggPlayerCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CaxAnxOggPlayerCtrl, "AXANXOGGPLAYER.axAnxOggPlayerCtrl.1",
	0x9b80daa5, 0xbcfa, 0x44f8, 0xb2, 0xaa, 0xb2, 0xec, 0xf8, 0x6, 0x2, 0xa1)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CaxAnxOggPlayerCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_DaxAnxOggPlayer =
		{ 0xB878657F, 0xFA23, 0x4915, { 0x8E, 0x80, 0x66, 0x64, 0xF1, 0x73, 0x8B, 0xEF } };
const IID BASED_CODE IID_DaxAnxOggPlayerEvents =
		{ 0x380DE0F9, 0x5B8, 0x4D01, { 0x92, 0x21, 0xC6, 0x48, 0xBC, 0x43, 0x24, 0x23 } };



// Control type information

static const DWORD BASED_CODE _dwaxAnxOggPlayerOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CaxAnxOggPlayerCtrl, IDS_AXANXOGGPLAYER, _dwaxAnxOggPlayerOleMisc)



// CaxAnxOggPlayerCtrl::CaxAnxOggPlayerCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CaxAnxOggPlayerCtrl

BOOL CaxAnxOggPlayerCtrl::CaxAnxOggPlayerCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_AXANXOGGPLAYER,
			IDB_AXANXOGGPLAYER,
			afxRegApartmentThreading,
			_dwaxAnxOggPlayerOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CaxAnxOggPlayerCtrl::CaxAnxOggPlayerCtrl - Constructor

CaxAnxOggPlayerCtrl::CaxAnxOggPlayerCtrl()
{
	InitializeIIDs(&IID_DaxAnxOggPlayer, &IID_DaxAnxOggPlayerEvents);
	// TODO: Initialize your control's instance data here.
}



// CaxAnxOggPlayerCtrl::~CaxAnxOggPlayerCtrl - Destructor

CaxAnxOggPlayerCtrl::~CaxAnxOggPlayerCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CaxAnxOggPlayerCtrl::OnDraw - Drawing function

void CaxAnxOggPlayerCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	DoSuperclassPaint(pdc, rcBounds);
}



// CaxAnxOggPlayerCtrl::DoPropExchange - Persistence support

void CaxAnxOggPlayerCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CaxAnxOggPlayerCtrl::OnResetState - Reset control to default state

void CaxAnxOggPlayerCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CaxAnxOggPlayerCtrl::AboutBox - Display an "About" box to the user

void CaxAnxOggPlayerCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_AXANXOGGPLAYER);
	dlgAbout.DoModal();
}



// CaxAnxOggPlayerCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CaxAnxOggPlayerCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}



// CaxAnxOggPlayerCtrl::IsSubclassedControl - This is a subclassed control

BOOL CaxAnxOggPlayerCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CaxAnxOggPlayerCtrl::OnOcmCommand - Handle command messages

LRESULT CaxAnxOggPlayerCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}



// CaxAnxOggPlayerCtrl message handlers
