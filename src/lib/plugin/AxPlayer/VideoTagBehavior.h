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

#ifndef VIDEOTAGBEHAVIOUR_H
#define VIDEOTAGBEHAVIOUR_H

#pragma once
#include "resource.h"       // main symbols

#include "Generated Files\AxPlayer_i.h"
#include "_IVideoTagBehaviorEvents_CP.h"

#include "DShowVideoPlayer.h"
#include <MsHtmdid.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef IDispEventImpl<1, VideoTagBehavior, &DIID_HTMLElementEvents, &LIBID_MSHTML, 4, 0> HTMLEvents;

// VideoTagBehavior

class ATL_NO_VTABLE VideoTagBehavior :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<VideoTagBehavior, &CLSID_VideoTagBehavior>,
	public IConnectionPointContainerImpl<VideoTagBehavior>,
	public CProxy_IVideoTagBehaviorEvents<VideoTagBehavior>,
    public IObjectSafetyImpl<VideoTagBehavior, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
    public IObjectWithSiteImpl<VideoTagBehavior>,
	public IDispatchImpl<IVideoTagBehavior, &IID_IVideoTagBehavior, &LIBID_AxPlayerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IElementBehavior,
    public IElementBehaviorFactory,
    public IElementBehaviorLayout,
    public IElementNamespaceFactory,
    public IElementNamespaceFactoryCallback,
    public IHTMLPainter,
    public HTMLEvents,
    public DShowVideoPlayerCallback
{
public:
	VideoTagBehavior();

    DECLARE_REGISTRY_RESOURCEID(IDR_VIDEOTAGBEHAVIOR)

    DECLARE_NOT_AGGREGATABLE(VideoTagBehavior)

    BEGIN_COM_MAP(VideoTagBehavior)
	    COM_INTERFACE_ENTRY(IVideoTagBehavior)
	    COM_INTERFACE_ENTRY(IDispatch)
	    COM_INTERFACE_ENTRY(IConnectionPointContainer)
        COM_INTERFACE_ENTRY(IObjectSafety)
	    COM_INTERFACE_ENTRY(IObjectWithSite)
        COM_INTERFACE_ENTRY(IElementBehavior)
        COM_INTERFACE_ENTRY(IElementBehaviorFactory)
        COM_INTERFACE_ENTRY(IElementBehaviorLayout)
        COM_INTERFACE_ENTRY(IElementNamespaceFactory)
        COM_INTERFACE_ENTRY(IElementNamespaceFactoryCallback)
        COM_INTERFACE_ENTRY(IHTMLPainter)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(VideoTagBehavior)
	    CONNECTION_POINT_ENTRY(__uuidof(_IVideoTagBehaviorEvents))
    END_CONNECTION_POINT_MAP()

    BEGIN_SINK_MAP(VideoTagBehavior)
        SINK_ENTRY_EX(1, DIID_HTMLElementEvents, DISPID_HTMLDOCUMENTEVENTS_ONCLICK, OnClick)
    END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease();

    // IElementBehavior
    HRESULT __stdcall Init(IElementBehaviorSite* pBehaviorSite);
    HRESULT __stdcall Notify(LONG lEvent, VARIANT* pVar);
    HRESULT __stdcall Detach();

    // IElementBehaviorFactory
    HRESULT __stdcall FindBehavior(BSTR bstrBehavior, BSTR bstrBehaviorUrl,
        IElementBehaviorSite* pSite, IElementBehavior** ppBehavior);

    // IElementBehaviorLayout
    HRESULT __stdcall GetLayoutInfo(LONG *pLayoutInfo);
    HRESULT __stdcall GetPosition(LONG flags, POINT *pTopLeft);
    HRESULT __stdcall GetSize(LONG flags, SIZE sizeContent, POINT *pTranslateBy, POINT *pTopLeft, SIZE *pSize);
    HRESULT __stdcall MapSize(SIZE *pSizeIn, RECT *pRectOut);

    // IElementNamespaceFactory
    HRESULT __stdcall Create(IElementNamespace * pNamespace);

    // IElementNamespaceFactoryCallback
    HRESULT __stdcall Resolve(BSTR bstrNamespace, BSTR bstrTagName, BSTR bstrAttrs, 
        IElementNamespace* pNamespace);

    // IHTMLPainter
    HRESULT __stdcall GetPainterInfo(HTML_PAINTER_INFO *pInfo);
    HRESULT __stdcall Draw(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject);
    HRESULT __stdcall HitTestPoint(POINT pt, BOOL *pbHit, LONG *plPartID);
    HRESULT __stdcall OnResize(SIZE pt);

    // IObjectSafety override
    HRESULT __stdcall SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

    // Events
    VARIANT_BOOL __stdcall OnClick();

    // DShowVideoPlayerCallback
    virtual void Refresh();
    virtual void MovieSize(const CSize& movieSize);

private:
    void ParseElementAttributes();
private:

    CComPtr<IElementBehaviorSite> m_site;
    CComPtr<IElementBehaviorSiteOM2> m_omSite;
    CComPtr<IHTMLPaintSite> m_paintSite;
    CComPtr<IHTMLElement> m_element;

    int m_width;
    int m_height;

    CSize m_movieSize;

    DShowVideoPlayer m_videoPlayer;
};

OBJECT_ENTRY_AUTO(__uuidof(VideoTagBehavior), VideoTagBehavior)

#endif // VIDEOTAGBEHAVIOUR_H
