#include "stdafx.h"
#include "propsabout.h"


PropsAbout::PropsAbout(LPUNKNOWN inUnk, HRESULT* outHR)
	:	CBasePropertyPage(NAME("illiminable Directshow Filters"), inUnk, IDD_ILLIMINABLE_ABOUT, IDS_ILLI_ABOUT_STRING_TITLE)

{
	*outHR = S_OK;
}

PropsAbout::~PropsAbout(void)
{
}

CUnknown* PropsAbout::CreateInstance(LPUNKNOWN inUnk, HRESULT* outHR)
{
    return new PropsAbout(inUnk, outHR);
}
