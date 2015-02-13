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

class CWebBrowserEventsInterface;

class CWebViewInterface
{
public:
    virtual void SetWebBrowserEvents( CWebBrowserEventsInterface* pInterface ) = 0;
    virtual bool LoadURL            ( const SString& strURL, bool bFilterEnabled = true ) = 0;
    virtual bool IsLoading          () = 0;
    virtual void SetBeingDestroyed  ( bool state ) = 0;
    
    virtual void GetURL             ( SString& outURL ) = 0;
    virtual void GetTitle           ( SString& outTitle ) = 0;
    virtual void SetRenderingPaused ( bool bPaused ) = 0;
    virtual void Focus              () = 0;

    virtual void ExecuteJavascript  ( const SString& strJavascriptCode ) = 0;

    virtual void InjectMouseMove    ( int iPosX, int iPosY ) = 0;
    virtual void InjectMouseDown    ( eWebBrowserMouseButton mouseButton ) = 0;
    virtual void InjectMouseUp      ( eWebBrowserMouseButton mouseButton ) = 0;
    virtual void InjectMouseWheel   ( int iScrollVert, int iScrollHorz ) = 0;

    virtual bool IsLocal            () = 0;
    virtual void SetTempURL         ( const SString& strTempURL ) = 0;

    virtual bool SetAudioVolume     ( float fVolume ) = 0;
};

#endif
