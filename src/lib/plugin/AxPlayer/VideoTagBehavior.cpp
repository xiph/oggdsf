//===========================================================================
// Copyright (C) 2010 Cristian Adam
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Cristian Adam nor the names of contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==============================================================

#include "stdafx.h"
#include <InitGuid.h>
#include "VideoTagBehavior.h"
#include "common/util.h"
#include <ddraw.h>
#include "Guid.h"
#include "uriparser/uri.h"

// VideoTagBehavior
namespace {
    const wchar_t* VIDEO_TAG = L"video";
    const wchar_t* SRC_ATTRIBUTE = L"src";
    const wchar_t* WIDTH_ATTRIBUTE = L"width";
    const wchar_t* HEIGHT_ATTRIBUTE = L"height";

    const int NOT_SET = -1;
}

VideoTagBehavior::VideoTagBehavior() : 
m_width(300),
m_height(150),
m_factoryObject(false),
m_standardsMode(true),
m_desiredVideoSize(NOT_SET, NOT_SET),
m_widthPercentage(NOT_SET),
m_heightPercentage(NOT_SET)
{
    m_sizeExtent.cx = 0;
    m_sizeExtent.cy = 0;

    LOG(logDEBUG) << this << ": " << __FUNCTIONW__; 
}

VideoTagBehavior::~VideoTagBehavior()
{
    LOG(logDEBUG) << this << ": " << __FUNCTIONW__;
}


HRESULT __stdcall VideoTagBehavior::Init(IElementBehaviorSite* pBehaviorSite)
{
    LOG(logDEBUG) << __FUNCTIONW__;

    HRESULT hr = S_OK;

    try
    {
        m_site = pBehaviorSite;
        CHECK_HR(m_site->QueryInterface(&m_omSite));
        CHECK_HR(m_site->QueryInterface(&m_paintSite));
    }
    catch(const CAtlException& except)
    {
        hr = except.m_hr;
    }

    return hr;
}

HRESULT __stdcall VideoTagBehavior::Notify(LONG lEvent, VARIANT* pVar)
{
    LOG(logDEBUG) << __FUNCTIONW__ << " lEvent: " << lEvent;

    HRESULT hr = S_OK;

    try
    {
        switch (lEvent)
        {
        case BEHAVIOREVENT_CONTENTREADY: 
            // End tag of element has been parsed (we can get at attributes)
            {
                CHECK_HR(m_site->GetElement(&m_element));

                m_embeddedAxGuid = Guid::Create().ToString().c_str();

                CString embeddedAxText;
                embeddedAxText.Format(
                    L"<object id=\"%s\""
                        L"classid=\"clsid:7cc95ae6-c1fa-40cc-ab17-3e91da2f77ca\">"
                    L"</object>", m_embeddedAxGuid);

                CHECK_HR(m_element->put_innerHTML(CComBSTR(embeddedAxText)));
            }
            break;
        case BEHAVIOREVENT_DOCUMENTREADY:	
            // HTML document has been parsed (we can get at the document object model)
            {
                m_element = 0;
                CHECK_HR(m_site->GetElement(&m_element));
                
                CComPtr<IHTMLStyle> style;
                CHECK_HR(m_element->get_style(&style));

                ParseElementAttributes();

                CHECK_HR(style->put_pixelWidth(m_width));
                CHECK_HR(style->put_pixelHeight(m_height));

                m_videoPlayer.SetWidth(m_width);
                m_videoPlayer.SetHeight(m_height);

                m_videoPlayer.Create(::GetDesktopWindow(), 0, L"PlayerWindow", WS_POPUP);
                m_videoPlayer.SetPlayerCallback(this);
                m_videoPlayer.InitializePlaybackThread();

                CHECK_HR(HTMLEvents::DispEventAdvise(m_element));

                AcquireEmbeddedAx();
            }
            break;
        }
    }
    catch (const CAtlException& except)
    {
        hr = except.m_hr;
    }

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::Detach()
{
    LOG(logDEBUG) << this << ": " << __FUNCTIONW__;

    m_videoPlayer.StopPlaybackThread();
    m_videoPlayer.DestroyWindow();

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::FindBehavior(BSTR bstrBehavior, BSTR bstrBehaviorUrl, 
                                                 IElementBehaviorSite* pSite, IElementBehavior** ppBehavior)
{
    LOG(logDEBUG) << this << " " << __FUNCTIONW__ << " bstrBehavior: " << bstrBehavior;

    if (bstrBehavior == 0)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    try
    {
        CComBSTR behavior(bstrBehavior);
        behavior.ToLower();

        if (behavior == VIDEO_TAG)
        {
            CComObject<VideoTagBehavior>* behavior;
            CHECK_HR(CComObject<VideoTagBehavior>::CreateInstance(&behavior));

            CHECK_HR(behavior->QueryInterface(IID_IElementBehavior, (void**)ppBehavior));
            CHECK_HR(behavior->SetSite(m_oleClientSite));
        }
    }
    catch (const CAtlException& except)
    {
        hr = except.m_hr;
    }

    return hr;
}

HRESULT __stdcall VideoTagBehavior::Create(IElementNamespace * pNamespace)
{
    CComBSTR tag = VIDEO_TAG;
    pNamespace->AddTag(tag, 0);

    LOG(logDEBUG) << this << ": " << __FUNCTIONW__;

    m_factoryObject = true;

    return S_OK;
}

HWND VideoTagBehavior::Create( HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName /*= NULL*/, 
                              DWORD dwStyle /*= 0*/, DWORD dwExStyle /*= 0*/, _U_MENUorID MenuOrID /*= 0U*/, 
                              LPVOID lpCreateParam /*= NULL*/ )
{
    return CComControl<VideoTagBehavior>::Create(hWndParent, rect, szWindowName, dwStyle, 
        dwExStyle, MenuOrID, lpCreateParam);
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
        HTMLPAINTER_SUPPORTS_XFORM | 
        HTMLPAINTER_HITTEST;

    pInfo->lZOrder = HTMLPAINT_ZORDER_REPLACE_ALL;

    pInfo->rcExpand.left = 0;
    pInfo->rcExpand.top = 0;
    pInfo->rcExpand.right = 0;
    pInfo->rcExpand.bottom = 0;

    return S_OK;
}


HRESULT __stdcall VideoTagBehavior::OnEmbeddedDraw(RECT rect, HDC hdc)
{
    HRESULT hr = m_videoPlayer.Draw(rect, rect, 0, hdc, 0);
    return hr;
}

HRESULT __stdcall VideoTagBehavior::Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject)
{
    // This is called only in Quirks mode
    m_standardsMode = false;

    HRESULT hr = m_videoPlayer.Draw(rcBounds, rcUpdate, lDrawFlags, hdc, pvDrawObject);
    return hr;
}

HRESULT VideoTagBehavior::OnDraw(ATL_DRAWINFO& di)
{
    RECT& rc = *(RECT*)di.prcBounds;

    if (m_embeddedAxEventsSink)
    {
        m_embeddedAxEventsSink->OnEmbeddedDraw(rc, di.hdcDraw);
    }

    return S_OK;
}


HRESULT __stdcall VideoTagBehavior::HitTestPoint(POINT pt, BOOL *pbHit, LONG *plPartID)
{
    *pbHit = TRUE;
    return S_OK;
}

VARIANT_BOOL __stdcall VideoTagBehavior::OnClick()
{
    if (m_videoPlayer.GetState() == DShowVideoPlayer::Paused ||
        m_videoPlayer.GetState() == DShowVideoPlayer::Stopped)
    {
        m_videoPlayer.Play();
    }
    else if (m_videoPlayer.GetState() == DShowVideoPlayer::Playing)
    {
        m_videoPlayer.Pause();
    }

    return VARIANT_FALSE;
}

HRESULT __stdcall VideoTagBehavior::OnResize(SIZE pt)
{
    LOG(logDEBUG) << __FUNCTIONW__ << ", "
        << "size: " << pt.cx << ", " << pt.cy;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
    m_dwCurrentSafety = m_dwCurrentSafety  & ~dwEnabledOptions | dwOptionSetMask;
    return S_OK;
}

void VideoTagBehavior::ParseElementAttributes()
{
    CComQIPtr<IHTMLDOMNode> node = m_element;
    
    CComPtr<IDispatch> disp;
    CHECK_HR(node->get_attributes(&disp));

    CComQIPtr<IHTMLAttributeCollection> attributesList = disp;
    disp = 0;

    long attributesCount = 0;
    CHECK_HR(attributesList->get_length(&attributesCount));

    for (long i = 0; i < attributesCount; ++i)
    {
        CComVariant item(i);
        CHECK_HR(attributesList->item(&item, &disp));

        CComQIPtr<IHTMLDOMAttribute> attribute = disp;
        disp = 0;

        CComBSTR attributeName;
        CHECK_HR(attribute->get_nodeName(&attributeName));

        attributeName.ToLower();

        if (attributeName == SRC_ATTRIBUTE)
        {
            CComVariant attributeValue;
            CHECK_HR(attribute->get_nodeValue(&attributeValue));

            LOG(logINFO) << attributeName << " = \"" << attributeValue << "\"";

            ParseSrcAttribute(attributeValue);
        }
        else if (attributeName == HEIGHT_ATTRIBUTE)
        {
            CComVariant attributeValue;
            CHECK_HR(attribute->get_nodeValue(&attributeValue));

            LOG(logINFO) << attributeName << " = \"" << attributeValue << "\"";
        
            ParseDimensionAttribute(attributeValue, false);
        }
        else if (attributeName == WIDTH_ATTRIBUTE)
        {
            CComVariant attributeValue;
            CHECK_HR(attribute->get_nodeValue(&attributeValue));

            LOG(logINFO) << attributeName << " = \"" << attributeValue << "\"";

            ParseDimensionAttribute(attributeValue, true);
        }
    }
}

void VideoTagBehavior::Refresh()
{
    if (m_standardsMode)
    {
        CComQIPtr<IEmbeddedAx> embeddedAx = m_embeddedAxElement;
        if (embeddedAx)
        {
            embeddedAx->EmbeddedRefresh();
        }
    }
    else if (m_paintSite)
    {
        m_paintSite->InvalidateRect(0);
    }
}


HRESULT __stdcall VideoTagBehavior::EmbeddedRefresh()
{
    FireViewChange();
    return S_OK;
}

void VideoTagBehavior::MovieSize(const CSize& movieSize)
{
    AdjustElementDimensions(movieSize);
    
    LOG(logINFO) << __FUNCTIONW__ << " Resizing the element to: " << m_width << "x" << m_height;

    try
    {
        CComPtr<IHTMLStyle> style;
        CHECK_HR(m_element->get_style(&style));

        CHECK_HR(style->put_pixelWidth(m_width));
        CHECK_HR(style->put_pixelHeight(m_height));

        style = 0;

        CHECK_HR(m_embeddedAxElement->get_style(&style));
        CHECK_HR(style->put_pixelWidth(m_width));
        CHECK_HR(style->put_pixelHeight(m_height));

        m_videoPlayer.SetWidth(m_width);
        m_videoPlayer.SetHeight(m_height);
    }
    catch (const CAtlException& /*except*/)
    {
    }
}

HRESULT __stdcall VideoTagBehavior::SetSite( IUnknown *pUnkSite )
{
    try
    {
        if (pUnkSite)
        {
            CHECK_HR(pUnkSite->QueryInterface(IID_IOleClientSite, (void**)&m_oleClientSite));
        }
    }
    catch (const CAtlException /*except*/)
    {
    }

    return IObjectWithSiteImpl<VideoTagBehavior>::SetSite(pUnkSite);
}

CString VideoTagBehavior::GetSiteURL()
{
    CString siteUrl;

    try
    {
        CComPtr<IMoniker> objectWithSiteMoniker;
        LPOLESTR displayNameOleStr;

        ATLASSERT(m_oleClientSite);
        CHECK_HR(m_oleClientSite->GetMoniker(OLEGETMONIKER_TEMPFORUSER, OLEWHICHMK_CONTAINER, &objectWithSiteMoniker));
        CHECK_HR(objectWithSiteMoniker->GetDisplayName(NULL, NULL, &displayNameOleStr));

        siteUrl = displayNameOleStr;
        ::CoTaskMemFree((LPVOID)displayNameOleStr);
    }
    catch (const CAtlException& /*except*/)
    {
    }

    return siteUrl;
}

bool VideoTagBehavior::IsRelativeURL(const CString& url)
{
    // This is a very simple implementation
    CString lowerCaseUrl(url);
    lowerCaseUrl.MakeLower();

    bool isRelative = true;
    if (lowerCaseUrl.Find(L"http://") != -1)
    {
        isRelative = false;
    }
    else if (lowerCaseUrl.Find(L"ftp://") != -1)
    {
        isRelative = false;
    }
    else if (lowerCaseUrl.Find(L"file://") != -1)
    {
        isRelative = false;
    }

    return isRelative;
}

HRESULT __stdcall VideoTagBehavior::SetClientSite(IOleClientSite* pSite)
{
    m_oleClientSite = pSite;
    return CComControlBase::IOleObject_SetClientSite(pSite);
}

void VideoTagBehavior::AcquireEmbeddedAx()
{
    CComPtr<IDispatch> disp;
    CHECK_HR(m_element->get_children(&disp));

    CComQIPtr<IHTMLElementCollection> elementCollectionList = disp;
    disp = 0;

    long childrenCount = 0;
    CHECK_HR(elementCollectionList->get_length(&childrenCount));
    ATLASSERT(childrenCount == 1 && "We should have only one child");

    for (long i = 0; i < childrenCount; ++i)
    {
        CComVariant itemId(m_embeddedAxGuid);
        CComVariant item(i);
        CHECK_HR(elementCollectionList->item(itemId, item, &disp));

        CComQIPtr<IHTMLElement> element = disp;
        disp = 0;

        if (!element)
        {
            continue;
        }

        m_embeddedAxElement = element;

        // Configure the communication between the <video> tag and the
        // embedded ActiveX
        CComQIPtr<IEmbeddedAx> embeddedAx = element;
        CComQIPtr<IEmbeddedAxEventsSink> embeddedAxEventsSink = this;
        
        embeddedAx->SetEventsSink(embeddedAxEventsSink);
    }
}

LRESULT VideoTagBehavior::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    if (m_embeddedAxEventsSink)
    {
        m_embeddedAxEventsSink->OnLeftButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    }

    return 0;
}

HRESULT __stdcall VideoTagBehavior::OnLeftButtonDown(LONG /*x*/, LONG /*y*/)
{
    OnClick();
    return 0;
}

HRESULT __stdcall VideoTagBehavior::SetEventsSink(IUnknown *events)
{
    if (!events)
    {
        return E_POINTER;
    }

    HRESULT hr = events->QueryInterface(&m_embeddedAxEventsSink);
    return hr;
}

void VideoTagBehavior::ParseSrcAttribute(const CComVariant& attributeValue)
{
    CString src(attributeValue);

    if (IsRelativeURL(src))
    {
        UriUriW relativeSrc;
        UriParserStateW state;
        state.uri = &relativeSrc;

        if (uriParseUriW(&state, src) != URI_SUCCESS)
        {
            LOG(logERROR) << "Failed to parse src: \"" << src << "\"";
            uriFreeUriMembersW(&relativeSrc);
            return;
        }

        UriUriW absoluteBase;
        state.uri = &absoluteBase;

        CString siteUrl = GetSiteURL();
        if (uriParseUriW(&state, siteUrl) != URI_SUCCESS)
        {
            LOG(logERROR) << "Failed to parse site url: \"" << siteUrl << "\"";
            uriFreeUriMembersW(&relativeSrc);
            uriFreeUriMembersW(&absoluteBase);
            return;
        }

        UriUriW absoluteDest;
        if (uriAddBaseUriW(&absoluteDest, &relativeSrc, &absoluteBase) != URI_SUCCESS)
        {
            LOG(logERROR) << "Failed to create absolute uri from relative src: \"" << src << "\""
                << " and absolute base: \"" << siteUrl << "\"";
            uriFreeUriMembersW(&relativeSrc);
            uriFreeUriMembersW(&absoluteBase);
            uriFreeUriMembersW(&absoluteDest);
            return;
        }

        CString dest;
        int charsRequired;
        if (uriToStringCharsRequiredW(&absoluteDest, &charsRequired) != URI_SUCCESS)
        {
            LOG(logERROR) << "uriToStringCharsRequiredW failed";
            return;
        }

        dest.GetBuffer(charsRequired);
        int charsWritten;

        if (uriToStringW(dest.GetBuffer(), &absoluteDest, charsRequired, &charsWritten) != URI_SUCCESS)
        {
            LOG(logERROR) << "uriToStringW failed";
            return;
        }

        dest.ReleaseBuffer(charsWritten);

        uriFreeUriMembersW(&relativeSrc);
        uriFreeUriMembersW(&absoluteBase);
        uriFreeUriMembersW(&absoluteDest);

        src = dest;
    }

    m_videoPlayer.SetSrc(src);
}

void VideoTagBehavior::ParseDimensionAttribute(const CComVariant& attributeValue, bool isHorizontal)
{
    std::wstring attributeStr(attributeValue.bstrVal);
    
    double percentage = NOT_SET;
    std::wistringstream wis(attributeStr);

    long dimension = NOT_SET;

    if (attributeStr.find_last_of(L'%') != std::wstring::npos)
    {
        long value = 0;
        wis >> value;

        if (value > 0)
        {
            percentage =  value / 100.0;
        }
    }
    else
    {
        long value = 0;
        wis >> value;

        if (value >= 0)
        {
            dimension = value;
        }
    }

    if (isHorizontal)
    {
        m_widthPercentage = percentage;
        m_desiredVideoSize.cx = dimension;
    }
    else
    {
        m_heightPercentage = percentage;
        m_desiredVideoSize.cy = dimension;
    }
}

void VideoTagBehavior::AdjustElementDimensions(const CSize &movieSize)
{
    // Take percentage into consideration
    if (m_widthPercentage == NOT_SET && m_heightPercentage != NOT_SET)
    {
        m_widthPercentage = m_heightPercentage;
    }
    else if (m_widthPercentage != NOT_SET && m_heightPercentage == NOT_SET)
    {
        m_heightPercentage = m_widthPercentage;
    }
    else if (m_widthPercentage == NOT_SET && m_heightPercentage == NOT_SET)
    {
        m_widthPercentage = m_heightPercentage = 1.0;
    }

    // Take desired dimensions into consideration
    double aspectRatio = 4 / 3.0;
    if (movieSize.cy != 0)
    {
        aspectRatio = movieSize.cx / static_cast<double>(movieSize.cy);
    }

    if (m_desiredVideoSize.cx == NOT_SET && m_desiredVideoSize.cy == NOT_SET)
    {
        m_width = static_cast<unsigned int>(movieSize.cx * m_widthPercentage);
        m_height = static_cast<unsigned int>(movieSize.cy * m_heightPercentage);
    }
    else if (m_desiredVideoSize.cx != NOT_SET)
    {
        m_width = m_desiredVideoSize.cx;
        m_height =  static_cast<unsigned long>(m_desiredVideoSize.cx / aspectRatio);
    }
    else if (m_desiredVideoSize.cy != NOT_SET)
    {
        m_height = m_desiredVideoSize.cy;
        m_width = static_cast<unsigned long>(m_desiredVideoSize.cy * aspectRatio);
    }
}