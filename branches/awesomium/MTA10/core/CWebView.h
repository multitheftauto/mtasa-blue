/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CWebView.h
*  PURPOSE:     Web view class
*
*****************************************************************************/

#ifndef __CWEBVIEW_H
#define __CWEBVIEW_H

#include <core/CWebViewInterface.h>
#include <Awesomium/WebView.h>
#include <Awesomium/STLHelpers.h>
#include <d3d9.h>
#include <SString.h>

class CWebView : public CWebViewInterface
{
    friend class CRenderItemManager;

public:
    CWebView        ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface );

    // Exported methods
    bool LoadURL    ( const SString& strURL );
    bool IsLoading  ();
    void GetURL     ( SString& outURL );
    void GetTitle   ( SString& outTitle );

    
private:
    Awesomium::WebString    ToWebString ( const SString& strString );
    SString                 ToSString   ( const Awesomium::WebString& webString );

    Awesomium::WebView* m_pWebView;
    IDirect3DSurface9*  m_pD3DSurface;
};

#endif
