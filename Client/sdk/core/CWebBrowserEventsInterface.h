/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CWebBrowserEventsInterface.h
*  PURPOSE:     Webbrowser events interface class
*
*****************************************************************************/

#ifndef __CWEBBROWSEREVENTSINTERFACE_H
#define __CWEBBROWSEREVENTSINTERFACE_H

class CAjaxResourceHandlerInterface;
class CWebBrowserEventsInterface
{
public:
    virtual void Events_OnCreated () = 0;
    virtual void Events_OnLoadingStart ( const SString& strURL, bool bMainFrame ) = 0;
    virtual void Events_OnDocumentReady ( const SString& strURL ) = 0;
    virtual void Events_OnLoadingFailed ( const SString& strURL, int errorCode, const SString& errorDescription ) = 0;
    virtual void Events_OnNavigate ( const SString& strURL, bool bIsBlocked, bool bIsMainFrame ) = 0;
    virtual void Events_OnPopup ( const SString& strTargetURL, const SString& strOpenerURL ) = 0;
    virtual void Events_OnChangeCursor ( unsigned char ucCursor ) = 0;
    virtual void Events_OnTriggerEvent ( const SString& strEventName, const std::vector<std::string>& arguments ) = 0;
    virtual void Events_OnTooltip ( const SString& strTooltip ) = 0;
    virtual void Events_OnInputFocusChanged ( bool bGainedFocus ) = 0;
    virtual bool Events_OnResourcePathCheck ( SString& strURL ) = 0;
    virtual bool Events_OnResourceFileCheck ( const  SString& strURL ) = 0;
    virtual void Events_OnResourceBlocked ( const SString& strURL, const SString& strDomain, unsigned char reason ) = 0;
    virtual void Events_OnAjaxRequest ( CAjaxResourceHandlerInterface* pHandler, const SString& strURL ) = 0;
};

#endif
