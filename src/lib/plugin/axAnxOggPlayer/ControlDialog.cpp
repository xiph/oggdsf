// ControlDialog.cpp : implementation file
//

#include "stdafx.h"
#include "axAnxOggPlayer.h"
#include "ControlDialog.h"
#include ".\controldialog.h"


// CControlDialog dialog

IMPLEMENT_DYNAMIC(CControlDialog, CDialog)
CControlDialog::CControlDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDialog::IDD, pParent)
	, mBrowser(NULL)
{
	debugLog.open("G:\\logs\\axanx.log", ios_base::out);
}

CControlDialog::~CControlDialog()
{
	debugLog.close();
	if (mBrowser != NULL) {
		mBrowser->Release();
	}
}

void CControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CControlDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()


// CControlDialog message handlers

void CControlDialog::OnBnClickedButton1()
{
	//PLay button.
	debugLog<<"Play button pushed"<<endl;
	mPlayer.loadFile(L"G:\\a.ogg", this->m_hWnd, 10, 100, 384, 240);
	//mPlayer.loadFile("G:\\h.ogv");
	mPlayer.play();
}

void CControlDialog::setBrowser(IWebBrowser2* inBrowser) {
	debugLog<<"Setting browser instance to "<<(int)inBrowser<<endl;
	mBrowser = inBrowser;

	BSTR locURL = NULL;
	mBrowser->get_LocationURL(&locURL);
	wstring locS = locURL;
	SysFreeString(locURL);
	mPlayer.loadFile(locS);
	mPlayer.play();

}

void CControlDialog::OnBnClickedButton2()
{
	//Pause button
	debugLog<<"Pause button pushed..."<<endl;
	mPlayer.pause();
}

void CControlDialog::OnBnClickedButton3()
{
	//Stop button
	debugLog<<"Stop button pushed.."<<endl;
}
