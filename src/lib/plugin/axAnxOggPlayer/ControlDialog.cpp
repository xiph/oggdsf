// ControlDialog.cpp : implementation file
//

#include "stdafx.h"
#include "axAnxOggPlayer.h"
#include "ControlDialog.h"


// CControlDialog dialog

IMPLEMENT_DYNAMIC(CControlDialog, CDialog)
CControlDialog::CControlDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDialog::IDD, pParent)
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
END_MESSAGE_MAP()


// CControlDialog message handlers
