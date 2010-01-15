// VideoTagBehavior.cpp : Implementation of VideoTagBehavior

#include "stdafx.h"
#include "VideoTagBehavior.h"
#include "common/util.h"

// VideoTagBehavior
namespace {
    const wchar_t* VIDEO_TAG = L"VIDEO";
}

VideoTagBehavior::VideoTagBehavior() : 
m_width(320),
m_height(240)
{
    LOG(logDEBUG) << this << ": " << __FUNCTIONW__; 
}

HRESULT __stdcall VideoTagBehavior::Init(IElementBehaviorSite* pBehaviorSite)
{
    HRESULT hr = S_OK;

    m_site = pBehaviorSite;
    hr = m_site->QueryInterface(&m_omSite);
    hr = m_site->QueryInterface(&m_paintSite);

    LOG(logDEBUG) << __FUNCTIONW__;

    return hr;
}

HRESULT __stdcall VideoTagBehavior::Notify(LONG lEvent, VARIANT* pVar)
{
    LOG(logDEBUG) << __FUNCTIONW__ << " lEvent: " << lEvent << ", pVar: " << ToString((CComVariant)pVar);

    switch (lEvent)
    {
    case BEHAVIOREVENT_CONTENTREADY: 
        // End tag of element has been parsed (we can get at attributes)
        break;
    case BEHAVIOREVENT_DOCUMENTREADY:	
        // HTML document has been parsed (we can get at the document object model)
        {
            HRESULT hr = m_site->GetElement(&m_element);
            
            CComPtr<IHTMLStyle> style;
            m_element->get_style(&style);

            style->put_pixelWidth(m_width);
            style->put_pixelHeight(m_height);

            if (m_paintSite)
            {
                m_paintSite->InvalidateRect(0);
            }
        }

        break;
    }
    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::Detach()
{
    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::FindBehavior(BSTR bstrBehavior, BSTR bstrBehaviorUrl, 
                                                 IElementBehaviorSite* pSite, IElementBehavior** ppBehavior)
{
    LOG(logDEBUG) << __FUNCTIONW__ << " bstrBehavior: " << bstrBehavior;

    if (bstrBehavior == 0)
    {
        return E_POINTER;
    }

    HRESULT hr = E_FAIL;

    if (wcsicmp(bstrBehavior, VIDEO_TAG) == 0)
    {
        CComObject<VideoTagBehavior>* behavior;
        hr = CComObject<VideoTagBehavior>::CreateInstance(&behavior);

        if (FAILED(hr))
        {
            return hr;
        }

        hr = behavior->QueryInterface(IID_IElementBehavior, (void**)ppBehavior);
    }

    return hr;
}

HRESULT __stdcall VideoTagBehavior::Create(IElementNamespace * pNamespace)
{
    CComBSTR tag = VIDEO_TAG;
    pNamespace->AddTag(tag, 0);

    LOG(logDEBUG) << this << ": " << __FUNCTIONW__;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::Resolve(BSTR bstrNamespace, BSTR bstrTagName, BSTR bstrAttrs, 
                                            IElementNamespace* pNamespace)
{
    return S_OK;
}

HRESULT VideoTagBehavior::FinalConstruct()
{
    util::ConfigureLogSettings();

    return S_OK;
}

void VideoTagBehavior::FinalRelease()
{

}

HRESULT __stdcall VideoTagBehavior::GetLayoutInfo(LONG *pLayoutInfo)
{
    LOG(logDEBUG) << __FUNCTIONW__;
    *pLayoutInfo = BEHAVIORLAYOUTINFO_FULLDELEGATION;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::GetPosition(LONG flags, POINT *pTopLeft)
{
    LOG(logDEBUG) << __FUNCTIONW__ << ", "
        << "flags: " << flags << ", "
        << "pTopLeft: " << pTopLeft->x << ", " << pTopLeft->y;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::GetSize(LONG flags, SIZE sizeContent, POINT *pTranslateBy, POINT *pTopLeft, SIZE *pSize)
{
    LOG(logDEBUG) << __FUNCTIONW__ << ", "
        << "flags: " << flags << ", "
        << "sizeContent: " << sizeContent.cx << ", " << sizeContent.cy << ", "
        << "pTopLeft: " << pTopLeft->x << ", " << pTopLeft->y << ", "
        << "pSize: " << pSize->cx << ", " << pSize->cy;

    pSize->cx = m_width;
    pSize->cy = m_height;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::MapSize(SIZE *pSizeIn, RECT *pRectOut)
{
    LOG(logDEBUG) << __FUNCTIONW__ << ", "
        << "pSizeIn: " << pSizeIn->cx << ", " << pSizeIn->cy << ", "
        << "pRectOut: " << pRectOut->left << ", " << pRectOut->top << " - " 
                        << pRectOut->right << ", " << pRectOut->bottom;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::GetPainterInfo(HTML_PAINTER_INFO *pInfo)
{
    pInfo->lFlags = 
        HTMLPAINTER_NOSAVEDC | 
        HTMLPAINTER_SUPPORTS_XFORM;

    pInfo->lZOrder = HTMLPAINT_ZORDER_REPLACE_ALL;

    memset(&pInfo->iidDrawObject, 0, sizeof(IID));

    pInfo->rcExpand.left = 0;
    pInfo->rcExpand.top = 0;
    pInfo->rcExpand.right = 0;
    pInfo->rcExpand.bottom = 0;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject)
{
    CRect rect(rcBounds.left, rcBounds.top,
        rcBounds.left + m_width, rcBounds.top + m_height);

    FillRect(hdc, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::HitTestPoint(POINT pt, BOOL *pbHit, LONG *plPartID)
{
    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::OnResize(SIZE pt)
{
    LOG(logDEBUG) << __FUNCTIONW__ << ", "
        << "size: " << pt.cx << ", " << pt.cy;

    return S_OK;
}
