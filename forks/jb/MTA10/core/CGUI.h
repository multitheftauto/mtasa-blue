/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGUI.h
*  PURPOSE:     Header file for core graphical user interface class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLocalGUI;

#ifndef __CLOCALGUI_H
#define __CLOCALGUI_H

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A // Defined only when including Windows.h -> Not getting defined? (<=XP only?)
#endif

#define DIRECT3D_VERSION         0x0900
#include "d3d9.h"
#include "d3dx9.h"

#include <gui/CGUI.h>

#include "CConsole.h"
#include "CFilePathTranslator.h"
#include "CLogger.h"
#include "CMainMenu.h"
#include "CSetCursorPosHook.h"
#include "CSingleton.h"
#include "CCommunityRegistration.h"
#include "CVersionUpdater.h"
#include "CNewsBrowser.h"

#include <windows.h>

class CChat;
class CDebugView;

class CLocalGUI : public CSingleton < CLocalGUI >
{
public:
                        CLocalGUI                        ( void );
                        ~CLocalGUI                       ( void );

    void                SetSkin                     ( const char* szName );

    void                CreateWindows               ( bool bGameIsAlreadyLoaded );
    void                DestroyWindows              ( void );

    void                CreateObjects               ( IUnknown* pDevice );
    void                DestroyObjects              ( void );

    void                DoPulse                     ( void );

    void                Draw                        ( void );
    void                Invalidate                  ( void );
    void                Restore                     ( void );

    void                DrawMouseCursor             ( void );

    CConsole*           GetConsole                  ( void );
    void                SetConsoleVisible           ( bool bVisible );
    bool                IsConsoleVisible            ( void );
    void                EchoConsole                 ( const char* szText );

    CMainMenu*          GetMainMenu                 ( void );
    void                SetMainMenuVisible          ( bool bVisible );
    bool                IsMainMenuVisible           ( void );

    CChat*              GetChat                     ( void );
    void                SetChatBoxVisible           ( bool bVisible );
    bool                IsChatBoxVisible            ( void );
    void                SetChatBoxInputEnabled      ( bool bInputEnabled );
    bool                IsChatBoxInputEnabled       ( void );
    void                EchoChat                    ( const char* szText, bool bColorCoded );

    CDebugView*         GetDebugView                ( void );
    void                SetDebugViewVisible         ( bool bVisible );
    bool                IsDebugViewVisible          ( void );
    void                EchoDebug                   ( const char* szText );

    CCommunityRegistration* GetCommunityRegistration ( void )               { return &m_CommunityRegistration; };

    bool                ProcessMessage              ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    bool                InputGoesToGUI              ( void );
    inline bool         IsCursorForcedVisible       ( void )                { return m_bForceCursorVisible; }
    void                ForceCursorVisible          ( bool bVisible );

    void                KeyDownHandler              ( bool bHandled );

    void                ShownHandler                ( bool bHandled );
    void                HiddenHandler               ( bool bHandled );

    int                 GetVisibleWindows           ( );
    void                SetVisibleWindows           ( bool bEnable );

    void                InitiateUpdate              ( const char* szType, const char* szData, const char* szHost )      { m_pVersionUpdater->InitiateUpdate ( szType, szData, szHost ); }
    bool                IsOptionalUpdateInfoRequired( const char* szHost )                          { return m_pVersionUpdater->IsOptionalUpdateInfoRequired ( szHost ); }
    void                InitiateDataFilesFix        ( void )                                        { m_pVersionUpdater->InitiateDataFilesFix (); }

private:
    void                    UpdateCursor                ( void );

    DWORD                   TranslateScanCodeToGUIKey   ( DWORD dwCharacter );

    CConsole*               m_pConsole;
    CMainMenu*              m_pMainMenu;
    CChat*                  m_pChat;
    CDebugView*             m_pDebugView;

    CCommunityRegistration  m_CommunityRegistration;
    CVersionUpdaterInterface* m_pVersionUpdater;


    CGUILabel*              m_pLabelVersionTag;

    int                     m_iVisibleWindows;
    bool                    m_bVisibleWindows;

    bool                    m_bForceCursorVisible;
    bool                    m_bChatboxVisible;
    bool                    m_pDebugViewVisible;
    bool                    m_bGUIHasInput;
    int                     m_uiActiveCompositionSize;

    int                     m_LastSettingsRevision; // the revision number the last time we saw the skin change
    SString                 m_LastSkinName;
};

#endif
