#pragma once

// axAnxOggPlayerPropPage.h : Declaration of the CaxAnxOggPlayerPropPage property page class.


// CaxAnxOggPlayerPropPage : See axAnxOggPlayerPropPage.cpp for implementation.

class CaxAnxOggPlayerPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CaxAnxOggPlayerPropPage)
	DECLARE_OLECREATE_EX(CaxAnxOggPlayerPropPage)

// Constructor
public:
	CaxAnxOggPlayerPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_AXANXOGGPLAYER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

