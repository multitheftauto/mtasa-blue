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

class CWebCoreInterface
{
public:
    virtual CWebViewInterface*  CreateWebView       ( unsigned int uiWidth, unsigned int uiHeight, IDirect3DSurface9* pD3DSurface ) = 0;
    virtual void                Update              () = 0;
    
    virtual eURLState           GetURLState        ( const SString& strURL ) = 0;
    virtual void                ClearWhitelist     () = 0;
    virtual void                AddAllowedPage     ( const SString& strURL ) = 0;
    virtual void                RequestPages       ( const std::vector<SString>& pages ) = 0;
    virtual void                AllowPendingPages  () = 0;
    virtual void                DenyPendingPages   () = 0;
};

#endif
