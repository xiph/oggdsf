#pragma once

#include <fstream>
using namespace std;
// CControlDialog dialog

class CControlDialog : public CDialog
{
	DECLARE_DYNAMIC(CControlDialog)

public:
	CControlDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CControlDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	IWebBrowser2* mBrowser;
	fstream debugLog;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	void setBrowser(IWebBrowser2* inBrowser);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
