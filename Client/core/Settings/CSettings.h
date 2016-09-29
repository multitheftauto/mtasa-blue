/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSettings.h
*  PURPOSE:     Header file for in-game settings window class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CSettings;

#ifndef __CSETTINGS_H
#define __CSETTINGS_H

#include <core/CCoreInterface.h>
#include "CMainMenu.h"
#include "CCore.h"

class CColor;

#define CORE_MTA_FILLER                 "cgui\\images\\mta_filler.png"
#define CORE_SETTINGS_UPDATE_INTERVAL   30         // Settings update interval in frames
#define CORE_SETTINGS_HEADERS           3
#define CORE_SETTINGS_HEADER_SPACER     " "
#define CORE_SETTINGS_NO_KEY            " "
#define SKINS_PATH                    "skins/*"
#define CHAT_PRESETS_PATH             "mta/config/chatboxpresets.xml"
#define CHAT_PRESETS_ROOT             "chatboxpresets"


#include "CAdvancedSettingsTab.h"
#include "CAudioSettingsTab.h"
#include "CBindSettingsTab.h"
#include "CControlsSettingsTab.h"
#include "CInterfaceSettingsTab.h"
#include "CMultiplayerSettingsTab.h"
#include "CVideoSettingsTab.h"
#include "CWebSettingsTab.h"


//#define SHOWALLSETTINGS

class CSettings
{
    friend class CCore;

public:
                        CSettings               ( void );
                        ~CSettings              ( void ) = default;

    void                CreateGUI               ( void );
    void                DestroyGUI              ( void );


    void                Update(void) {
        m_pControlsTab->Update();
    }

    void                SetVisible              ( bool bVisible );
    bool                IsVisible               ( void );

    void                SetIsModLoaded          ( bool bLoaded );

    void                LoadData                ( void );
    bool IsCapturingKey() { return m_pBindTab->IsCapturingKey(); }
    bool                ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return m_pBindTab->ProcessMessage(uMsg, wParam, lParam);
    }
    void RequestNewNickname() { m_pMultiplayerTab->RequestNewNickname(); }

    void                ShowRestartQuestion     ( void );
    void                ShowDisconnectQuestion  ( void );

    void                TabSkip                 ( bool bBackwards );

    bool                IsActive                ( void );

    void                SetSelectedIndex        ( unsigned int uiIndex );


    bool                OnOKButtonClick(CGUIElement* pElement);
    bool                OnCancelButtonClick(CGUIElement* pElement);
    void                SaveData(void);
    CGUIElement* GetWindow() { return m_pWindow.get(); }
protected:
    // Keep these protected so we can access them in the event handlers of CClientGame
    std::unique_ptr<CGUIElement>         m_pWindow;
    std::unique_ptr<CGUITabPanel>       m_pTabs;
    std::unique_ptr<CGUIButton>         m_pButtonOK;
    std::unique_ptr<CGUIButton>         m_pButtonCancel;
    
    // Tabs
    std::unique_ptr<CAdvancedSettingsTab>    m_pAdvancedTab;
    std::unique_ptr<CAudioSettingsTab>       m_pAudioTab;
    std::unique_ptr<CBindSettingsTab>        m_pBindTab;
    std::unique_ptr<CControlsSettingsTab>    m_pControlsTab;
    std::unique_ptr<CInterfaceSettingsTab>   m_pInterfaceTab;
    std::unique_ptr<CMultiplayerSettingsTab> m_pMultiplayerTab;
    std::unique_ptr<CVideoSettingsTab>       m_pVideoTab;
    std::unique_ptr<CWebSettingsTab>         m_pWebTab;
};

#endif
