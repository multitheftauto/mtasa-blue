/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CWebBrowserInterface.h
*  PURPOSE:     Webbrowser interface class
*
*****************************************************************************/

#ifndef __CWEBCOREINTERFACE_H
#define __CWEBCOREINTERFACE_H

class CWebBrowserItem;
class CWebViewInterface;

enum eURLState
{
    WEBPAGE_NOT_LISTED,
    WEBPAGE_ALLOWED,
    WEBPAGE_DISALLOWED
};

enum eAwesomiumMouseButton
{
    AWESOMIUM_MOUSEBUTTON_LEFT = 0,
    AWESOMIUM_MOUSEBUTTON_MIDDLE = 1,
    AWESOMIUM_MOUSEBUTTON_RIGHT = 2
};

class CWebCoreInterface
{
public:
    virtual CWebViewInterface*  CreateWebView       ( unsigned int uiWidth, unsigned int uiHeight, bool bIsLocal ) = 0;
    virtual void                DestroyWebView      ( CWebViewInterface* pWebView ) = 0;
    
    virtual eURLState           GetURLState        ( const SString& strURL ) = 0;
    virtual void                ClearWhitelist     () = 0;
    virtual void                AddAllowedPage     ( const SString& strURL ) = 0;
    virtual void                RequestPages       ( const std::vector<SString>& pages ) = 0;
    virtual void                AllowPendingPages  () = 0;
    virtual void                DenyPendingPages   () = 0;

    virtual bool                IsTestModeEnabled  () = 0;
    virtual void                SetTestModeEnabled ( bool bEnabled ) = 0;

    virtual bool                CanLoadRemotePages () = 0;

    virtual bool                SetGlobalAudioVolume( float fVolume ) = 0;
};

#endif
