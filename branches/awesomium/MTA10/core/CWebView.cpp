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
    m_pWebView = Awesomium::WebCore::instance()->CreateWebView ( uiWidth, uiHeight, NULL, Awesomium::kWebViewType_Offscreen );
}

CWebView::~CWebView()
{
    // Release WebView
    m_pWebView->Destroy ();
}

bool CWebView::LoadURL ( const SString& strURL )
{
    Awesomium::WebURL webURL ( CWebCore::ToWebString ( strURL ) );
    if ( !g_pCore->GetWebCore ()->IsURLAllowed ( CWebCore::ToSString ( webURL.host() ) ) )
        return false;

    m_pWebView->LoadURL ( webURL );
    return true;
}

bool CWebView::IsLoading ()
{
    return m_pWebView->IsLoading ();
}

void CWebView::GetURL ( SString& outURL )
{
    outURL = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->url ().spec () ) );
}

void CWebView::GetTitle(SString& outTitle)
{
    outTitle = static_cast<SString> ( CWebCore::ToSString ( m_pWebView->title() ) );
}

void CWebView::InjectMouseMove ( int iPosX, int iPosY )
{
    m_pWebView->InjectMouseMove ( iPosX, iPosY );
}

void CWebView::InjectMouseDown ( int mouseButton )
{
    m_pWebView->InjectMouseDown ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectMouseUp ( int mouseButton )
{
    m_pWebView->InjectMouseUp ( (Awesomium::MouseButton) mouseButton );
}

void CWebView::InjectKeyboardEvent ( const SString& strKey, bool bKeyDown, bool bCharacter )
{
    // Hack fix
    SString key = strKey;
    if (key == " ")
        key = "space";

    Awesomium::WebKeyboardEvent keyboardEvent;
    keyboardEvent.is_system_key = false;
    keyboardEvent.modifiers = Awesomium::WebKeyboardEvent::kModIsAutorepeat;

    if ( !bCharacter )
    {
        // Get BindableKey structure (to be able to process MTA's key names correctly)
        const SBindableKey* pBindableKey = g_pCore->GetKeyBinds ()->GetBindableFromKey ( key.c_str () );
        if ( !pBindableKey )
            return;

        keyboardEvent.virtual_key_code = pBindableKey->ulCode;
        //keyboardEvent.native_key_code = pBindableKey->ulCode;
        keyboardEvent.type = bKeyDown ? Awesomium::WebKeyboardEvent::kTypeKeyDown : Awesomium::WebKeyboardEvent::kTypeKeyUp;
    }
    else
    {
        std::wstring strCharacter (MbUTF8ToUTF16(strKey));
        if ( strCharacter.length() > 4 )
            return;
        memcpy ( keyboardEvent.text, strCharacter.c_str (), strCharacter.length () * 2 );
        keyboardEvent.type = Awesomium::WebKeyboardEvent::kTypeChar;
    }
    
    m_pWebView->InjectKeyboardEvent ( keyboardEvent );
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
