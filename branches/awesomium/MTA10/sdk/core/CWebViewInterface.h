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

class CWebViewInterface
{
public:
    virtual bool LoadURL            ( const SString& strURL ) = 0;
    virtual bool IsLoading          () = 0;
    
    virtual void GetURL             ( SString& outURL ) = 0;
    virtual void GetTitle           ( SString& outTitle ) = 0;
    virtual void SetRenderingPaused ( bool bPaused ) = 0;

    virtual void InjectMouseMove    ( int iPosX, int iPosY ) = 0;
    virtual void InjectMouseDown    ( int mouseButton ) = 0;
    virtual void InjectMouseUp      ( int mouseButton ) = 0;
    virtual void InjectMouseWheel   ( int iScrollVert, int iScrollHorz ) = 0;
    virtual void InjectKeyboardEvent( const SString& strKey, bool bKeyDown = true, bool bCharacter = false ) = 0;

};

#endif
