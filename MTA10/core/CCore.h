/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CCore.h
*  PURPOSE:		Header file for base core class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCore;

#ifndef __CCORE_H
#define __CCORE_H

#include "../version.h"

#include "CClientTime.h"
#include "CClientVariables.h"
#include "CCommands.h"
#include "CFileSystemHook.h"
#include "CModuleLoader.h"
#include "CSingleton.h"
#include "CGUI.h"
#include "CConnectManager.h"
#include "CDirect3DHookManager.h"
#include "CDirectInputHookManager.h"
#include "CGraphics.h"
#include "CSetCursorPosHook.h"
#include "CMessageLoopHook.h"
#include "CLogger.h"
#include "CModManager.h"
#include <core/CClientBase.h>
#include <core/CCoreInterface.h>
#include "CDirect3DData.h"
#include "CResManager.h"
#include "tracking/CTCPManager.h"
#include "CMainConfig.h"
#include "CKeyBinds.h"
#include "CScreenShot.h"
#include "CVideoManager.h"
#include "CCommunity.h"
#include <xml/CXML.h>
#include <ijsify.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define BLUE_VERSION_STRING     "Multi Theft Auto 2.0\n" \
                                "Copyright (C) 2003 - 2008 Multi Theft Auto" \

class CCore : public CCoreInterface, public CSingleton < CCore >
{
public:
                            CCore                           ( void );
                            ~CCore                          ( void );

    eCoreVersion            GetVersion                      ( void );

    CConsoleInterface*      GetConsole                      ( void );
    CCommandsInterface*     GetCommands                     ( void );
    inline CConnectManager* GetConnectManager               ( void )                { return m_pConnectManager; };
    CGame*                  GetGame                         ( void );
	CGUI*					GetGUI					        ( void );
    CGraphicsInterface*     GetGraphics                     ( void );
    CModManagerInterface*   GetModManager                   ( void );
    CMultiplayer*           GetMultiplayer                  ( void );
    CNet*                   GetNetwork                      ( void );
    CXML*                   GetXML                          ( void )                { return m_pXML; };
    CMainConfig*            GetConfig                       ( void );
    CKeyBindsInterface*     GetKeyBinds                     ( void );
    CLocalGUI*              GetLocalGUI                     ( void );
	CVideoManager*			GetVMR9Manager					( void );
    CCommunityInterface*    GetCommunity                    ( void )                { return &m_Community; };

    #ifndef MTA_DEBUG
    CResManager *           GetResManager                   ( void );
    #endif

	void					OnModUnload						( void );

    char*                   GetDebugFileName                ( char* szBuffer, size_t bufferSize );

    void                    DebugEcho                       ( const char* szText );
    void                    DebugPrintf                     ( const char* szFormat, ... );
    void                    SetDebugVisible                 ( bool bVisible );
    bool                    IsDebugVisible                  ( void );
    void                    DebugEchoColor                  ( const char* szText, unsigned char R, unsigned char G, unsigned char B );
    void                    DebugPrintfColor                ( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    DebugClear                      ( void );
    void                    ChatEcho                        ( const char* szText, bool bColorCoded = false );
    void                    ChatEchoColor                   ( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false );
    void                    ChatPrintf                      ( const char* szFormat, bool bColorCoded, ... );
    void                    ChatPrintfColor                 ( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    SetChatVisible                  ( bool bVisible );
    bool                    IsChatVisible                   ( void );
	void					TakeScreenShot					( void );
	void					SetScreenShotPath				( const char *szPath );
    void                    SetChatInputBackgroundColor     ( DWORD dwColor );
    void                    EnableChatInput                 ( char* szCommand, DWORD dwColor );
    bool                    IsChatInputEnabled              ( void );
    bool                    IsSettingsVisible               ( void );
    bool                    IsMenuVisible                   ( void );
    bool                    IsCursorForcedVisible           ( void );
    void                    SetChatColor                    ( unsigned char R, unsigned char G, unsigned char B, unsigned char A );
    void                    SetChatInputColor               ( unsigned char R, unsigned char G, unsigned char B, unsigned char A );
    void                    SetChatInputPrefixColor         ( unsigned char R, unsigned char G, unsigned char B, unsigned char A );
    void                    SetChatInputTextColor           ( unsigned char R, unsigned char G, unsigned char B, unsigned char A );
    void                    SetChatLines                    ( unsigned int uiLines );
    void                    SetChatFont                     ( eChatFont font );
    void                    SetChatScale                    ( CVector2D& vecScale );
    void                    SetChatWidth                    ( float fWidth );
    void                    SetChatCssStyleText             ( bool bEnabled );
    void                    SetChatCssStyleBackground       ( bool bEnabled );
    void                    SetChatLineLife                 ( unsigned long ulTime );
    void                    SetChatLineFadeOut              ( unsigned long ulTime );
    void                    SetChatUseCEGUI                 ( bool bUseCEGUI );
    void                    ApplyConsoleSettings            ( void );
    void                    ApplyChatSettings               ( void );
    void                    ApplyGameSettings               ( void );
	bool					IsUsingCEGUIForText				( void );
    void                    CallSetCursorPos                ( int X, int Y ) { m_pSetCursorPosHook->CallSetCursorPos(X,Y); }

    void                    SetConnected                    ( bool bConnected );
	bool					IsConnected						( void );

    void                    SetOfflineMod                   ( bool bOffline );
    void                    ForceCursorVisible              ( bool bVisible );
    void                    SetMessageProcessor             ( pfnProcessMessage pfnMessageProcessor );
    void                    ShowMessageBox                  ( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler = NULL );
    void                    RemoveMessageBox                ( bool bNextFrame = false );

    HWND                    GetHookedWindow                 ( void );

    void                    HideMainMenu                    ( void );

    void                    ApplyHooks                      ( );

    void                    RegisterCommands                        ( );
 
	void			        SetCenterCursor							( bool bEnabled );

    void                    CreateGame                              ( );
    void                    CreateMultiplayer                       ( );
    void                    CreateNetwork                           ( );
    void                    CreateXML                               ( );
	void			        InitGUI							        ( IUnknown* pDevice );
	void			        CreateGUI						        ( void );

	void			        SetRenderDevice							( IUnknown* pDevice );
	void			        SwitchRenderWindow						( HWND hWnd, HWND hWndInput );
	bool			        GetResetNeeded							( );

    void                    DestroyGame                             ( );
    void                    DestroyMultiplayer                      ( );
    void                    DestroyNetwork                          ( );
    void                    DestroyXML                              ( );
	void			        DeinitGUI						        ( );

    void                    DoPreFramePulse                         ( );
    void                    DoPostFramePulse                        ( );

    bool                    IsOfflineMod                            ( ) { return m_bIsOfflineMod; }
    char *                  GetModInstallRoot                       ( char * szModName, char * szBuffer, size_t bufferSize );
    const char *            GetInstallRoot                          ( void );
    const char *            GetGTAInstallRoot                       ( void );
    void                    SetClientMessageProcessor               ( pfnProcessMessage pfnMessageProcessor ) { m_pfnMessageProcessor = pfnMessageProcessor; };
    pfnProcessMessage       GetClientMessageProcessor           ( ) { return m_pfnMessageProcessor; }

	// CGUI Callbacks
	bool					OnMouseClick							( CGUIMouseEventArgs Args );
	bool					OnMouseDoubleClick						( CGUIMouseEventArgs Args );

    void					SaveNick                                ( const char* szNick );
    bool					IsValidNick                             ( const char* szNick );     // Move somewhere else
    void					ChangeResolution                        ( long width, long height, long depth );

    void                    Quit                                    ( bool bInstantly = true );

    const char *            GetConnectCommandFromURI                ( const char* szURI, char* szDest, size_t destLength );
	
	bool					bScreenShot;

private:
    void			            DestroyGUI						        ( );

    // Core devices.
    CXML*                       m_pXML;
    CClientVariables *          m_pClientVariables;
    CLocalGUI *                 m_pLocalGUI;
    CGraphics *                 m_pGraphics;
    CCommands *                 m_pCommands;
    CDirect3DData *             m_pDirect3DData;
    CConnectManager*            m_pConnectManager;
    IUnknown *					m_pRenderDevice;
    CCommunity                  m_Community;

    // Hook interfaces.
    CMessageLoopHook *          m_pMessageLoopHook;
    CDirectInputHookManager *   m_pDirectInputHookManager;
    CDirect3DHookManager *      m_pDirect3DHookManager;
    //CFileSystemHook *           m_pFileSystemHook;
	CSetCursorPosHook *         m_pSetCursorPosHook;
	CTCPManager *				m_pTCPManager;

    // Module loader objects.
    CModuleLoader               m_GameModule;
    CModuleLoader               m_MultiplayerModule;
    CModuleLoader               m_NetModule;
    CModuleLoader               m_XMLModule;
	CModuleLoader				m_GUIModule;

    // Mod manager
    CModManager*                m_pModManager; 

	// VMR9 manager
	CVideoManager*				m_pVMR9Manager;

    // Resolution manager
    CResManager*                m_pResManager;

    // Module interfaces.
    CGame *                     m_pGame;
    CNet *                      m_pNet;
    CMultiplayer *              m_pMultiplayer;
	CGUI*						m_pGUI;

    // Logger utility interface.
    CLogger *                   m_pLogger;

    CMainConfig*                m_pConfig;
    CKeyBinds*                  m_pKeyBinds;

	bool						m_bResetNeeded;
    bool                        m_bFirstFrame;
    bool                        m_bIsOfflineMod;
    pfnProcessMessage           m_pfnMessageProcessor;

    CGUIMessageBox*				m_pMessageBox;

    // screen res
    DEVMODE                     m_Current;

    char                        m_szInstallRoot[MAX_PATH];
    char                        m_szGTAInstallRoot[MAX_PATH];

    bool                        m_bQuitOnPulse;
    eChatFont                   m_ChatFont;
	bool						m_bUseCEGUI;
    bool                        m_bDestroyMessageBox;
};

#endif