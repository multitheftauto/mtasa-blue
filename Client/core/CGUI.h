/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CGUI.h
 *  PURPOSE:     Header file for core graphical user interface class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLocalGUI;

#pragma once

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A // Defined only when including Windows.h -> Not getting defined? (<=XP only?)
#endif

#define DIRECT3D_VERSION         0x0900
#include "d3d9.h"
#include "d3dx9.h"

#include <gui/CGUI.h>

#include "CConsole.h"
#include "CMainMenu.h"
#include "CSetCursorPosHook.h"
#include "CSingleton.h"
#include "CVersionUpdater.h"

#include <windows.h>

class CChat;
class CDebugView;

class CLocalGUI : public CSingleton<CLocalGUI>
{
public:
    CLocalGUI();
    ~CLocalGUI();

    void SetSkin(const char* szName);
    void ChangeLocale(const char* szName);

    void CreateWindows(bool bGameIsAlreadyLoaded);
    void DestroyWindows();

    void CreateObjects(IUnknown* pDevice);
    void DestroyObjects();

    void DoPulse();

    void Draw();
    void Invalidate();
    void Restore();

    void DrawMouseCursor();
    void SetCursorPos(int iX, int iY, bool bForce = false, bool overrideStored = true);

    CConsole* GetConsole();
    void      SetConsoleVisible(bool bVisible);
    bool      IsConsoleVisible();
    void      EchoConsole(const char* szText);

    CMainMenu* GetMainMenu();
    void       SetMainMenuVisible(bool bVisible);
    bool       IsMainMenuVisible();

    CChat* GetChat();
    void   SetChatBoxVisible(bool bVisible, bool bInputBlocked = true);
    bool   IsChatBoxVisible();
    bool   IsChatBoxInputBlocked();
    void   SetChatBoxInputEnabled(bool bInputEnabled);
    bool   IsChatBoxInputEnabled();
    void   EchoChat(const char* szText, bool bColorCoded);

    bool IsWebRequestGUIVisible();

    CDebugView* GetDebugView();
    void        SetDebugViewVisible(bool bVisible);
    bool        IsDebugViewVisible();
    void        EchoDebug(const char* szText);

    bool ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool InputGoesToGUI();
    bool IsCursorForcedVisible() { return m_bForceCursorVisible; }
    void ForceCursorVisible(bool bVisible);

    void InitiateUpdate(const char* szType, const char* szData, const char* szHost) { m_pVersionUpdater->InitiateUpdate(szType, szData, szHost); }
    bool IsOptionalUpdateInfoRequired(const char* szHost) { return m_pVersionUpdater->IsOptionalUpdateInfoRequired(szHost); }
    void InitiateDataFilesFix() { m_pVersionUpdater->InitiateDataFilesFix(); }

private:
    void UpdateCursor();

    DWORD TranslateScanCodeToGUIKey(DWORD dwCharacter);

    CConsole*   m_pConsole;
    CMainMenu*  m_pMainMenu;
    CChat*      m_pChat;
    CDebugView* m_pDebugView;

    CVersionUpdaterInterface* m_pVersionUpdater;

    CGUILabel* m_pLabelVersionTag;

    bool  m_bForceCursorVisible;
    bool  m_bChatboxVisible;
    bool  m_bChatboxInputBlocked;
    bool  m_pDebugViewVisible;
    bool  m_bGUIHasInput;
    int   m_uiActiveCompositionSize;
    POINT m_StoredMousePosition;

    int     m_LastSettingsRevision;            // the revision number the last time we saw the skin change
    SString m_LastSkinName;
    SString m_LastLocaleName;
    uint    m_LocaleChangeCounter;
};
