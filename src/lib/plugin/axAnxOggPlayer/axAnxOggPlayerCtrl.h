#pragma once

// axAnxOggPlayerCtrl.h : Declaration of the CaxAnxOggPlayerCtrl ActiveX Control class.


// CaxAnxOggPlayerCtrl : See axAnxOggPlayerCtrl.cpp for implementation.

class CaxAnxOggPlayerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CaxAnxOggPlayerCtrl)

// Constructor
public:
	CaxAnxOggPlayerCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CaxAnxOggPlayerCtrl();

	DECLARE_OLECREATE_EX(CaxAnxOggPlayerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CaxAnxOggPlayerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CaxAnxOggPlayerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CaxAnxOggPlayerCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	};
};

