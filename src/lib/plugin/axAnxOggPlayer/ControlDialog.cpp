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
}

CControlDialog::~CControlDialog()
{
}

void CControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CControlDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CControlDialog message handlers

void CControlDialog::OnBnClickedButton1()
{
	//PLay button.
	// TODO: Add your control notification handler code here

}

void CControlDialog::setBrowser(IWebBrowser2* inBrowser) {
	mBrowser = inBrowser;

}
