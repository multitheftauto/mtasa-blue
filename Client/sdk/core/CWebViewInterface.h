/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CWebViewInterface.h
*  PURPOSE:     WebView interface class
*
*****************************************************************************/

#ifndef __CWEBVIEWINTERFACE_H
#define __CWEBVIEWINTERFACE_H

#include "CWebCoreInterface.h"

class CWebBrowserEventsInterface;

class CWebViewInterface
{
public:
    virtual void Initialise         () = 0;
    virtual void SetWebBrowserEvents( CWebBrowserEventsInterface* pInterface ) = 0;
    virtual bool LoadURL            ( const SString& strURL, bool bFilterEnabled = true, const SString& strPostData = SString(), bool bURLEncoded = true ) = 0;
    virtual bool IsLoading          () = 0;
    virtual void SetBeingDestroyed  ( bool state ) = 0;
    
    virtual SString GetURL          () = 0;
    virtual const SString& GetTitle () = 0;
    virtual void SetRenderingPaused ( bool bPaused ) = 0;
    virtual void Focus              ( bool state = true ) = 0;
    virtual IDirect3DTexture9* GetTexture () = 0;

    virtual bool HasInputFocus      () = 0;

    virtual void ExecuteJavascript  ( const SString& strJavascriptCode ) = 0;

    virtual bool SetProperty        ( const SString& strKey, const SString& strValue ) = 0;
    virtual bool GetProperty        ( const SString& strKey, SString& outProperty ) = 0;

    virtual void InjectMouseMove    ( int iPosX, int iPosY ) = 0;
    virtual void InjectMouseDown    ( eWebBrowserMouseButton mouseButton ) = 0;
    virtual void InjectMouseUp      ( eWebBrowserMouseButton mouseButton ) = 0;
    virtual void InjectMouseWheel   ( int iScrollVert, int iScrollHorz ) = 0;

    virtual bool IsLocal            () = 0;

    virtual float GetAudioVolume    () = 0;
    virtual bool SetAudioVolume     ( float fVolume ) = 0;

    virtual void GetSourceCode      ( const std::function<void( const std::string& code )>& callback ) = 0;

    // Ajax Handlers
    using ajax_callback_t = const std::function<const SString ( std::vector<SString>& vecGet, std::vector<SString>& vecPost )>;
    
    virtual bool RegisterAjaxHandler ( const SString& strURL ) = 0;
    virtual bool UnregisterAjaxHandler ( const SString& strURL ) = 0;

    virtual bool ToggleDevTools     ( bool visible ) = 0;

    virtual bool CanGoBack          () = 0;
    virtual bool CanGoForward       () = 0;
    virtual bool GoBack             () = 0;
    virtual bool GoForward          () = 0;
    virtual void Refresh            ( bool bIgnoreCache ) = 0;
};

#endif
