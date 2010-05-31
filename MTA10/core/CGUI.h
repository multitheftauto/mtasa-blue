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

// DirectX rendering library (some abstraction would be nice here to avoid this)
#ifdef FORCE_GUI_DX8
    #include "d3d/include/CD3DMGE.H"
#else
    #ifdef FORCE_GUI_DX9
        #include "d3d/include/CD3DMGE_D3D9.h"
    #else
        #ifdef COMPILE_FOR_SA
            #include "d3d/include/CD3DMGE_D3D9.h"
        #else
            #include "d3d/include/CD3DMGE.H"
        #endif
    #endif
#endif

#include <gui/CGUI.h>

#include "CConsole.h"
#include "CFilePathTranslator.h"
#include "CLogger.h"
#include "CMainMenu.h"
#include "CSetCursorPosHook.h"
#include "CSingleton.h"
#include "CCommunityRegistration.h"
#include "CVersionUpdater.h"

#include <windows.h>

class CChat;
class CDebugView;

class CLocalGUI : public CSingleton < CLocalGUI >
{
public:
                        CLocalGUI                        ( void );
                        ~CLocalGUI                       ( void );

    void                CreateWindows               ( void );
    void                DestroyWindows              ( void );

    void                CreateObjects               ( IUnknown* pDevice );
    void                DestroyObjects              ( void );
    CD3DMGEng*          GetRenderingLibrary         ( void );

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

    //CChatBox*           GetChatBox                  ( void );
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

    void                InitiateUpdate              ( const char* szType, const char* szHost )      { m_VersionUpdater.InitiateUpdate ( szType, szHost ); }
    bool                IsOptionalUpdateInfoRequired( const char* szHost )                          { return m_VersionUpdater.IsOptionalUpdateInfoRequired ( szHost ); }

private:
    void                    UpdateCursor                ( void );

    DWORD                   TranslateScanCodeToGUIKey   ( DWORD dwCharacter );

    CConsole*               m_pConsole;
    CMainMenu*              m_pMainMenu;
    //CChatBox*             m_pChatBox;
    CChat*                  m_pChat;
    CDebugView*             m_pDebugView;
    CD3DMGEng*              m_pRendererLibrary;

    CCommunityRegistration  m_CommunityRegistration;
    CVersionUpdater         m_VersionUpdater;


    CGUILabel*              m_pLabelVersionTag;

    int                     m_iVisibleWindows;
    bool                    m_bVisibleWindows;

    bool                    m_bForceCursorVisible;
    bool                    m_bChatboxVisible;
    bool                    m_pDebugViewVisible;
    bool                    m_bGUIHasInput;
};

#endif
