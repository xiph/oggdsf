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

// VideoTagBehavior
namespace {
    const wchar_t* VIDEO_TAG = L"video";
    const wchar_t* SRC_ATTRIBUTE = L"src";
    const wchar_t* WIDTH_ATTRIBUTE = L"width";
    const wchar_t* HEIGHT_ATTRIBUTE = L"height";
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
    LOG(logDEBUG) << __FUNCTIONW__ << " lEvent: " << lEvent;

    HRESULT hr = S_OK;

    try
    {
        switch (lEvent)
        {
        case BEHAVIOREVENT_CONTENTREADY: 
            // End tag of element has been parsed (we can get at attributes)
            break;
        case BEHAVIOREVENT_DOCUMENTREADY:	
            // HTML document has been parsed (we can get at the document object model)
            {
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
    m_videoPlayer.StopPlaybackThread();
    m_videoPlayer.DestroyWindow();

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
        HTMLPAINTER_SUPPORTS_XFORM | 
        HTMLPAINTER_OVERLAY |
        HTMLPAINTER_SURFACE |
        HTMLPAINTER_HITTEST;

    pInfo->lZOrder = HTMLPAINT_ZORDER_REPLACE_ALL;

    pInfo->iidDrawObject = IID_IDirectDrawSurface;

    pInfo->rcExpand.left = 0;
    pInfo->rcExpand.top = 0;
    pInfo->rcExpand.right = 0;
    pInfo->rcExpand.bottom = 0;

    return S_OK;
}

HRESULT __stdcall VideoTagBehavior::Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject)
{
    HRESULT hr = m_videoPlayer.Draw(rcBounds, rcUpdate, lDrawFlags, hdc, pvDrawObject);
    return hr;
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

            LOG(logINFO) << SRC_ATTRIBUTE << " = \"" << attributeValue << "\"";

            m_videoPlayer.SetSrc(attributeValue);
        }
    }
}

void VideoTagBehavior::Refresh()
{
    if (m_paintSite)
    {
        m_paintSite->InvalidateRect(0);
    }
}

void VideoTagBehavior::MovieSize(const CSize& movieSize)
{
    m_width = movieSize.cx;
    m_height = movieSize.cy;

    try
    {
        CComPtr<IHTMLStyle> style;
        CHECK_HR(m_element->get_style(&style));

        CHECK_HR(style->put_pixelWidth(m_width));
        CHECK_HR(style->put_pixelHeight(m_height));

        m_videoPlayer.SetWidth(m_width);
        m_videoPlayer.SetHeight(m_height);
    }
    catch (const CAtlException& /*except*/)
    {
    }
}
