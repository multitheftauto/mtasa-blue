/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.cpp
*  PURPOSE:     Web view class
*
*****************************************************************************/

#include "StdInc.h"
#include "CWebView.h"

CWebView::CWebView ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface )
{
    m_pD3DSurface = pD3DSurface;

    // Create Awesomium webview as offscreen webview
    //m_pWebView = g_pCore->GetWebBrowser()->
}

bool CWebView::LoadURL ( const SString& strURL )
{
    // Todo: Might be better to move the url checking to CClientWebBrowser
    /*if (!IsURLAllowed(webURL.host()))
        return false; */

    m_pWebView->LoadURL ( Awesomium::WebURL ( ToWebString (strURL ) ) );
    return true;
}

bool CWebView::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

void CWebView::GetURL ( SString& outURL )
{
    outURL = static_cast<SString> ( ToSString ( m_pWebView->url ().spec () ) );
}

void CWebView::GetTitle(SString& outTitle)
{
    outTitle = static_cast<SString> ( ToSString ( m_pWebView->title() ) );
}

////////////////////////////////////////////////////////////////////
//                                                                //
//      Implementation: CefClient::GetRenderHandler               //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html#GetRenderHandler() //
//                                                                //
////////////////////////////////////////////////////////////////////
/*CefRefPtr<CefRenderHandler> CWebView::GetRenderHandler()
{
    // Well, we're a CefRenderHandler
    return this;
}*/

////////////////////////////////////////////////////////////////////
//                                                                //
//      Implementation: CefRenderHandler::GetViewRect             //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#GetViewRect(CefRefPtr%3CCefBrowser%3E,CefRect&) //
//                                                                //
////////////////////////////////////////////////////////////////////
/*bool CWebView::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    D3DSURFACE_DESC SurfaceDesc;
    m_pD3DSurface->GetDesc(&SurfaceDesc);

    rect.x = 0;
    rect.y = 0;
    rect.width = SurfaceDesc.Width;
    rect.height = SurfaceDesc.Height;
    return true;
}*/

////////////////////////////////////////////////////////////////////
//                                                                //
//      Implementation: CefRenderHandler::OnPaint                 //
// http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html#OnPaint(CefRefPtr%3CCefBrowser%3E,PaintElementType,constRectList&,constvoid*,int,int) //
//                                                                //
////////////////////////////////////////////////////////////////////
/*void CWebView::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType paintType, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (!m_pD3DSurface)
        return;

    D3DLOCKED_RECT LockedRect;
    D3DSURFACE_DESC SurfaceDesc;
    
    // First, lock the surface and request some information
    m_pD3DSurface->GetDesc(&SurfaceDesc);
    m_pD3DSurface->LockRect(&LockedRect, NULL, 0);

    // Paint buffer to surface (amount of pixels = width * height * 32bit color (ARGB))
    memcpy(LockedRect.pBits, buffer, width * height * 4);
    
    // Finally, unlock the surface
    m_pD3DSurface->UnlockRect();
}*/

Awesomium::WebString CWebView::ToWebString ( const SString& strString )
{
    return Awesomium::WSLit ( strString.c_str () );
}

SString CWebView::ToSString ( const Awesomium::WebString& webString )
{
    return SharedUtil::ToUTF8 ( std::wstring( (wchar_t*)webString.data () ) );
}
