/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSettings.cpp
*  PURPOSE:     In-game settings window
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

#include "StdInc.h"
#include <game/CGame.h>

extern CCore* g_pCore;

CSettings::CSettings ( void )
{
    m_pWindow = NULL;

    CreateGUI ();

    // Disable progress animation if required
    if ( GetApplicationSettingInt( GENERAL_PROGRESS_ANIMATION_DISABLE ) )
    {
        SetApplicationSettingInt( GENERAL_PROGRESS_ANIMATION_DISABLE, 0 );
        CVARS_SET ( "progress_animation", 0 );
    }
}

void CSettings::CreateGUI ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Init


    CVector2D vecTemp;
    CVector2D vecSize;

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();

    CVector2D contentSize( 640, 480 );
    float fBottomButtonAreaHeight = 38;
    CVector2D tabPanelPosition;
    CVector2D tabPanelSize = contentSize - CVector2D( 0, fBottomButtonAreaHeight );

    // Window size is content size plus window frame edge dims
    CVector2D windowSize = contentSize + CVector2D( 9 + 9, 20 + 2 );

    if ( windowSize.fX <= resolution.fX && windowSize.fY  <= resolution.fY )
    {
        // Create window (with frame) if it will fit inside the screen resolution
        auto pWindow = std::unique_ptr<CGUIWindow> ( pManager->CreateWnd ( NULL, _("SETTINGS") ) );
        pWindow->SetCloseButtonEnabled ( true );
        pWindow->SetMovable ( true );
        pWindow->SetPosition ( ( resolution - windowSize ) / 2 );
        pWindow->SetSize ( windowSize );
        pWindow->SetSizingEnabled ( false );
        pWindow->SetAlwaysOnTop ( true );
        pWindow->BringToFront ();
        m_pWindow = std::move(pWindow);
        tabPanelPosition = CVector2D( 0, 20 );
    }
    else
    {
        // Otherwise use black background image
        auto pFiller = std::unique_ptr<CGUIStaticImage> ( pManager->CreateStaticImage () );
        pFiller->LoadFromFile ( CORE_MTA_FILLER );
        pFiller->SetVisible ( false );
        pFiller->SetZOrderingEnabled ( false );
        pFiller->SetAlwaysOnTop ( true );
        pFiller->MoveToBack ();
        pFiller->SetPosition ( ( resolution - contentSize ) / 2 );
        pFiller->SetSize ( contentSize );
        m_pWindow = std::move(pFiller);
        tabPanelPosition = CVector2D( 0, 0 );
    }

    // Create the tab panel and necessary tabs
    m_pTabs = std::unique_ptr<CGUITabPanel> ( pManager->CreateTabPanel ( m_pWindow.get() ) );
    m_pTabs->SetPosition ( tabPanelPosition );
    m_pTabs->SetSize ( tabPanelSize );

    m_pMultiplayerTab = std::make_unique<CMultiplayerSettingsTab>(m_pTabs->CreateTab(_("Multiplayer")), this);
    m_pVideoTab = std::make_unique<CVideoSettingsTab>(m_pTabs->CreateTab(_("Video")));
    m_pAudioTab = std::make_unique<CAudioSettingsTab>(m_pTabs->CreateTab(_("Audio")));
    m_pBindTab = std::make_unique<CBindSettingsTab>(m_pTabs->CreateTab(_("Binds")));
    m_pControlsTab = std::make_unique<CControlsSettingsTab>(m_pTabs->CreateTab(_("Controls")));
    m_pInterfaceTab = std::make_unique<CInterfaceSettingsTab>(m_pTabs->CreateTab(_("Interface")), this);
    m_pWebTab = std::make_unique<CWebSettingsTab>(m_pTabs->CreateTab(_("Web Browser")));
    m_pAdvancedTab = std::make_unique<CAdvancedSettingsTab>(m_pTabs->CreateTab(_("Advanced")));

    // Create buttons
    //  OK button
    m_pButtonOK = std::unique_ptr<CGUIButton> ( pManager->CreateButton ( m_pWindow.get(), _("OK") ) );
    m_pButtonOK->SetPosition ( CVector2D ( contentSize.fX * 0.5f, tabPanelSize.fY + tabPanelPosition.fY + 8 ) );
    m_pButtonOK->SetZOrderingEnabled ( false );

    //  Cancel button
    m_pButtonCancel = std::unique_ptr<CGUIButton> ( pManager->CreateButton ( m_pWindow.get(), _("Cancel") ) );
    m_pButtonCancel->SetPosition ( CVector2D ( contentSize.fX * 0.78f, tabPanelSize.fY + tabPanelPosition.fY + 8 ) );
    m_pButtonCancel->SetZOrderingEnabled ( false );

    // Set up the events
    m_pWindow->SetEnterKeyHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonCancel->SetClickHandler(GUI_CALLBACK(&CSettings::OnCancelButtonClick, this));

    m_pInterfaceTab->LoadChatPresets();
    m_pInterfaceTab->LoadSkins();
}

void RestartCallBack ( void* ptr, unsigned int uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();

    if ( uiButton == 1 )
    {
        SetOnQuitCommand ( "restart" );
        CCore::GetSingleton ().Quit ();
    }
}


void CSettings::ShowRestartQuestion ( void )
{
    SString strMessage = _("Some settings will be changed when you next start MTA");
    strMessage += _("\n\nDo you want to restart now?");
    CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    pQuestionBox->Reset ();
    pQuestionBox->SetTitle ( _("RESTART REQUIRED") );
    pQuestionBox->SetMessage ( strMessage );
    pQuestionBox->SetButton ( 0, _("No") );
    pQuestionBox->SetButton ( 1, _("Yes") );
    pQuestionBox->SetCallback ( RestartCallBack );
    pQuestionBox->Show ();
}


void DisconnectCallback ( void* ptr, unsigned int uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();

    if ( uiButton == 1 )
    {
        CCommands::GetSingleton ().Execute ( "disconnect", "" );
    }
}


void CSettings::ShowDisconnectQuestion ( void )
{
    SString strMessage = _("Some settings will be changed when you disconnect the current server");
    strMessage += _("\n\nDo you want to disconnect now?");
    CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    pQuestionBox->Reset ();
    pQuestionBox->SetTitle ( _("DISCONNECT REQUIRED") );
    pQuestionBox->SetMessage ( strMessage );
    pQuestionBox->SetButton ( 0, _("No") );
    pQuestionBox->SetButton ( 1, _("Yes") );
    pQuestionBox->SetCallback ( DisconnectCallback );
    pQuestionBox->Show ();
}



//
// Update GUI elements for the Joypad Tab
//

void CSettings::SetVisible ( bool bVisible )
{
    // Hide / show the form
    m_pWindow->SetVisible ( bVisible );

    // Load the config file if the dialog is shown
    if ( bVisible )
    {
#ifdef MTA_DEBUG
        if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) != 0 )
            CreateGUI ();   // Recreate GUI (for adjusting layout with edit and continue)
#endif
        m_pWindow->BringToFront ();
        m_pWindow->Activate ();
        LoadData ();

        // Clear the binds list
        m_pBindTab->ClearBindsList();

        // Re-initialize the binds list
        m_pBindTab->Initialize ();
    }

    m_pWindow->SetZOrderingEnabled(!bVisible); //Message boxes dont appear on top otherwise
}


bool CSettings::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}

void CSettings::SetIsModLoaded ( bool bLoaded )
{
    m_pInterfaceTab->SetIsModLoaded(bLoaded);
}


bool CSettings::OnOKButtonClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    // Process keybinds
    m_pBindTab->ProcessKeyBinds();
    m_pControlsTab->ProcessJoypad();
    
    // Invalid nickname?
    std::string strNick = m_pMultiplayerTab->GetNickname();
    if ( !CCore::GetSingleton ().IsValidNick ( strNick.c_str() ) )
    {
        CCore::GetSingleton ().ShowMessageBox ( _("Error"), _("Your nickname contains invalid characters!"), MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Save the config
    SaveData ();

    // Close the window
    m_pWindow->SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}


bool CSettings::OnCancelButtonClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    m_pWindow->SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

    // restore old audio settings
    m_pAudioTab->Reset();

    return true;
}

void CSettings::LoadData ( void )
{
    CClientVariables::GetSingleton().ValidateValues ();

    m_pMultiplayerTab->LoadData();
    m_pAudioTab->LoadData();
    m_pBindTab->LoadData();
    m_pWebTab->LoadData();
    m_pAdvancedTab->LoadData();
    m_pControlsTab->LoadData();
    m_pInterfaceTab->LoadData();
    m_pVideoTab->LoadData();
}

void CSettings::SaveData ( void )
{
    m_pMultiplayerTab->SaveData();
    m_pAudioTab->SaveData();
    m_pBindTab->SaveData();
    m_pWebTab->SaveData();
    m_pAdvancedTab->SaveData();
    m_pControlsTab->SaveData();
    m_pInterfaceTab->SaveData();
    m_pVideoTab->SaveData();

    // Ensure CVARS ranges ok
    CClientVariables::GetSingleton().ValidateValues ();

    // Save the config here
    CCore::GetSingleton ().SaveConfig ();
    // Save the single player settings (e.g. video mode, volume)
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->Save ();

    bool bAskRestart =
        m_pVideoTab->ChangeRequiresRestart() ||
        m_pMultiplayerTab->ChangeRequiresRestart();

    bool bAskDisconnect = m_pWebTab->ChangeRequiresDisconnect();

    // Ask to restart?
    if (bAskRestart )
        ShowRestartQuestion();
    else if ( CModManager::GetSingleton ().IsLoaded () && bAskDisconnect)
        ShowDisconnectQuestion();
}

void CSettings::SetSelectedIndex(unsigned int uiIndex)
{
    unsigned int uiTabCount = m_pTabs->GetTabCount ( );

    if ( uiIndex < uiTabCount )
    {
        m_pTabs->SetSelectedIndex ( uiIndex );
    }
}

void CSettings::TabSkip ( bool bBackwards )
{
    unsigned int uiTabCount = m_pTabs->GetTabCount ( );

    if ( bBackwards )
    {
        unsigned int uiIndex = m_pTabs->GetSelectedIndex ( ) - 1;

        if ( m_pTabs->GetSelectedIndex ( ) == 0 )
        {
            uiIndex = uiTabCount - 1;
        }

        SetSelectedIndex ( uiIndex );
    }
    else
    {
        unsigned int uiIndex = m_pTabs->GetSelectedIndex ( ) + 1;
        unsigned int uiNewIndex = uiIndex % uiTabCount;

        SetSelectedIndex ( uiNewIndex );
    }
}

bool CSettings::IsActive ( void )
{
    return m_pWindow->IsActive ();
}
