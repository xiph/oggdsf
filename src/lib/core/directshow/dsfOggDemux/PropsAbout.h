#pragma once
#include "resource.h"


class PropsAbout
	:	public CBasePropertyPage
{
public:
	PropsAbout(LPUNKNOWN inUnk, HRESULT* outHR);
	virtual ~PropsAbout(void);

	static CUnknown* WINAPI CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR);


};
