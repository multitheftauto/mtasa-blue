/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CCoreInterface.h
*  PURPOSE:     Core interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOREINTERFACE_H
#define __CCOREINTERFACE_H

#include "CConsoleInterface.h"
#include "CCommandsInterface.h"
#include "CCommunityInterface.h"
#include "CGraphicsInterface.h"
#include "CModManagerInterface.h"
#include "CKeyBindsInterface.h"
#include "CCVarsInterface.h"
#include "xml/CXML.h"
#include <gui/CGUI.h>

typedef bool (*pfnProcessMessage) ( HWND, UINT, WPARAM, LPARAM );

class CMultiplayer;
class CNet;
class CGame;

enum eChatFont { CHAT_FONT_DEFAULT,CHAT_FONT_CLEAR,CHAT_FONT_BOLD,CHAT_FONT_ARIAL,CHAT_FONT_MAX };

enum eCoreVersion
{
    MTACORE_20 = 1,
};

class CCoreInterface
{
public:
    // Note: Always leave this on the top as the client must use this to verify
    //       correct MTA version before trying to use any other interface funcs.
    virtual eCoreVersion                GetVersion                      ( void ) = 0;

    virtual CConsoleInterface*          GetConsole                      ( void ) = 0;
    virtual CCommandsInterface*         GetCommands                     ( void ) = 0;
    virtual CGame*                      GetGame                         ( void ) = 0;
    virtual CGraphicsInterface*         GetGraphics                     ( void ) = 0;
    virtual CGUI*                       GetGUI                          ( void ) = 0;
    virtual CModManagerInterface*       GetModManager                   ( void ) = 0;
    virtual CMultiplayer*               GetMultiplayer                  ( void ) = 0;
    virtual CNet*                       GetNetwork                      ( void ) = 0;
    virtual CXML*                       GetXML                          ( void ) = 0;
    virtual CKeyBindsInterface*         GetKeyBinds                     ( void ) = 0;
    virtual CXMLNode*                   GetConfig                       ( void ) = 0;
    virtual CCVarsInterface*            GetCVars                        ( void ) = 0;
    virtual CCommunityInterface*        GetCommunity                    ( void ) = 0;
    

    // Temporary functions for r1
    virtual void                    DebugEcho                       ( const char* szText ) = 0;
    virtual void                    DebugPrintf                     ( const char* szFormat, ... ) = 0;
    virtual void                    SetDebugVisible                 ( bool bVisible ) = 0;
    virtual bool                    IsDebugVisible                  ( void ) = 0;
    virtual void                    DebugEchoColor                  ( const char* szText, unsigned char R, unsigned char G, unsigned char B )=0;
    virtual void                    DebugPrintfColor                ( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... ) = 0;
    virtual void                    DebugClear                      ( void )=0;
    virtual void                    ChatEcho                        ( const char* szText, bool bColorCoded = false ) = 0;
    virtual void                    ChatEchoColor                   ( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false ) = 0;
    virtual void                    ChatPrintf                      ( const char* szFormat, bool bColorCoded, ... ) = 0;
    virtual void                    ChatPrintfColor                 ( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... ) = 0;
    virtual void                    SetChatVisible                  ( bool bVisible ) = 0;
    virtual bool                    IsChatVisible                   ( void ) = 0;
    virtual void                    TakeScreenShot                  ( void ) = 0;
    virtual void                    EnableChatInput                 ( char* szCommand, DWORD dwColor ) = 0;
    virtual bool                    IsChatInputEnabled              ( void ) = 0;
    virtual bool                    IsSettingsVisible               ( void ) = 0;
    virtual bool                    IsMenuVisible                   ( void ) = 0;
    virtual bool                    IsCursorForcedVisible           ( void ) = 0;
    virtual bool                    IsCursorControlsToggled         ( void ) = 0;
    virtual void                    CallSetCursorPos                ( int X, int Y ) = 0;

    virtual void                    SetConnected                    ( bool bConnected ) = 0;
    virtual void                    SetOfflineMod                   ( bool bOffline ) = 0;

    virtual bool                    IsConnected                     ( void ) = 0;
    virtual bool                    Reconnect                       ( const char* szHost, unsigned short usPort, const char* szPassword ) = 0;

    virtual const char *            GetInstallRoot                  ( void ) = 0;
    virtual const char *            GetModInstallRoot               ( const char * szModName )=0;
    virtual const char *            GetGTAInstallRoot               ( void ) = 0;

    virtual void                    ShowServerInfo                  ( unsigned int WindowType ) = 0;

    virtual void                    ForceCursorVisible              ( bool bVisible, bool bToggleControls = true ) = 0;
    virtual void                    SetMessageProcessor             ( pfnProcessMessage pfnMessageProcessor ) = 0;
    virtual void                    ShowMessageBox                  ( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler = NULL ) = 0;
    virtual void                    RemoveMessageBox                ( bool bNextFrame = false ) = 0;
    virtual void                    HideMainMenu                    ( void ) = 0;
    virtual HWND                    GetHookedWindow                 ( void ) = 0;
    virtual bool                    IsFocused                       ( void ) = 0;
    virtual bool                    IsWindowMinimized               ( void ) = 0;

    virtual void                    SaveConfig                      ( void ) = 0;
    virtual void                    UpdateRecentlyPlayed            ( void ) = 0;

    virtual void                    SwitchRenderWindow              ( HWND hWnd, HWND hWndInput ) = 0;
    virtual void                    SetCenterCursor                 ( bool bEnabled ) = 0;

    // CGUI Callbacks
    virtual bool                    OnMouseClick                    ( CGUIMouseEventArgs Args ) = 0;
    virtual bool                    OnMouseDoubleClick              ( CGUIMouseEventArgs Args ) = 0;

    virtual void                    Quit                            ( bool bInstantly = true) = 0;
    virtual void                    InitiateUpdate                  ( const char* szType, const char* szData, const char* szHost ) = 0;
    virtual bool                    IsOptionalUpdateInfoRequired    ( const char* szHost ) = 0;
    virtual void                    InitiateDataFilesFix            ( void ) = 0;

    virtual uint                    GetFrameRateLimit               ( void ) = 0;
    virtual void                    RecalculateFrameRateLimit       ( uint uiServerFrameRateLimit = -1 ) = 0;
    virtual void                    ApplyFrameRateLimit             ( void ) = 0;
};

#endif
